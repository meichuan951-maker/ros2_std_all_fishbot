import os

import launch
import launch_ros
from ament_index_python.packages import get_package_share_directory


def generate_launch_description():
    robot_name_in_model = 'fishbot'
    urdf_package_path = get_package_share_directory('fishbot_description')
    default_urdf_path = os.path.join(urdf_package_path, 'urdf', 'fishbot', 'fishbot.urdf.xacro')
    default_world_path = os.path.join(urdf_package_path, 'world', 'custom_room.world')
    default_rviz_config_path = os.path.join(urdf_package_path, 'config', 'display_robot_model.rviz')

    action_declare_arg_mode_path = launch.actions.DeclareLaunchArgument(
        name='model',
        default_value=str(default_urdf_path),
        description='Absolute path to robot urdf file'
    )

    robot_description_v = launch_ros.parameter_descriptions.ParameterValue(
        launch.substitutions.Command(
            ['xacro ', launch.substitutions.LaunchConfiguration('model')]
        ),
        value_type=str
    )

    action_robot_state_publisher = launch_ros.actions.Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        name='fishbot_robot_state_publisher',
        parameters=[{
            'robot_description': robot_description_v,
            'use_sim_time': True
        }]
    )

    action_launch_gazebo = launch.actions.IncludeLaunchDescription(
        launch.launch_description_sources.PythonLaunchDescriptionSource(
            os.path.join(
                get_package_share_directory('gazebo_ros'),
                'launch',
                'gazebo.launch.py'
            )
        ),
        launch_arguments=[
            ('world', default_world_path),
            ('verbose', 'true')
        ]
    )

    spawn_entity_process = launch.actions.ExecuteProcess(
        cmd=[
            'bash', '-c',
            f'xacro "$1" | ros2 run gazebo_ros spawn_entity.py '
            f'-entity {robot_name_in_model} -stdin -timeout 60',
            'spawn_helper',
            launch.substitutions.LaunchConfiguration('model'),
        ],
        output='screen'
    )

    action_spawn_entity = launch.actions.TimerAction(
        period=3.0,
        actions=[spawn_entity_process]
    )

    # 暂时禁用 RViz,导航时由 navgation2.launch.py 启动 nav2 专用 RViz
    # action_rviz2_node = launch_ros.actions.Node(
    #     package='rviz2',
    #     executable='rviz2',
    #     arguments=['-d', default_rviz_config_path],
    #     parameters=[{'use_sim_time': True}]
    # )

    action_load_joint_state_controller = launch.actions.ExecuteProcess(
        cmd=[
            'ros2', 'control', 'load_controller', '--set-state', 'active',
            'fishbot_joint_state_broadcaster'
        ],
        output='screen'
    )

    action_load_diff_drive_controller = launch.actions.ExecuteProcess(
        cmd=[
            'ros2', 'control', 'load_controller', '--set-state', 'active',
            'fishbot_diff_drive_controller'
        ],
        output='screen'
    )

    return launch.LaunchDescription([
        action_declare_arg_mode_path,
        action_robot_state_publisher,
        action_launch_gazebo,
        action_spawn_entity,
        # action_rviz2_node,
        launch.actions.RegisterEventHandler(
            event_handler=launch.event_handlers.OnProcessExit(
                target_action=spawn_entity_process,
                on_exit=[action_load_joint_state_controller]
            )
        ),
        launch.actions.RegisterEventHandler(
            event_handler=launch.event_handlers.OnProcessExit(
                target_action=action_load_joint_state_controller,
                on_exit=[action_load_diff_drive_controller]
            )
        )
    ])
