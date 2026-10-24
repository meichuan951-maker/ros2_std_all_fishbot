import launch
import launch_ros.actions

def generate_launch_description():
    #声明一个launch参数
    action_declare_arg_background_green = launch.actions.DeclareLaunchArgument(
        'launch_arg_bg',
        default_value='150',
        description='The green value of the background color'
    )
  
    #产生launch描述
    action_node_turtlesim_node = launch_ros.actions.Node(
        package='turtlesim',
        executable='turtlesim_node',
        parameters=[{
            'background_green': launch.substitutions.LaunchConfiguration('launch_arg_bg',default = 150),
        }],
        output='screen',
    )
    action_node_partol_client_node = launch_ros.actions.Node(
        package='demo_cpp_service',
        executable='partol_client',
        output='log',
    )
    action_node_partol_turtle_control_node = launch_ros.actions.Node(
        package='demo_cpp_service',
        executable='turtle_control',
        output='both',
    )
    return launch.LaunchDescription([
        #action动作
        action_declare_arg_background_green,
        action_node_turtlesim_node,
        action_node_partol_client_node,
        action_node_partol_turtle_control_node,
    ])