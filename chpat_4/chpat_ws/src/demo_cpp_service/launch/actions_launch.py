import launch
import launch.launch_description_sources
import launch_ros.actions
from ament_index_python.packages import get_package_share_directory


def generate_launch_description():
    #动作声明1.包含启动其他launch文件
    action_declare_startup_rqt = launch.actions.DeclareLaunchArgument(
        'startup_rqt',
        default_value='true',
    )
    startup_rqt = launch.substitutions.LaunchConfiguration('startup_rqt',default='false')
    multisim_launch_path = get_package_share_directory('turtlesim') + '/launch/multisim.launch.py'
    action_include_launch = launch.actions.IncludeLaunchDescription(
        launch.launch_description_sources.PythonLaunchDescriptionSource(
            multisim_launch_path
        )
    )
#动作2,打印日志
    action_log_info = launch.actions.LogInfo(
        msg=str(multisim_launch_path)
    )
#动作3,打印话题列表
    action_topic_list = launch.actions.ExecuteProcess(
        condition=launch.conditions.IfCondition(startup_rqt),
        cmd=['rqt']
    )
#动作4,组动作
    action_group = launch.actions.GroupAction([
        #动作5定时器
        launch.actions.TimerAction(period=2.0, actions=[action_include_launch]),
        launch.actions.TimerAction(period=4.0, actions=[action_topic_list]),
    ])
    return launch.LaunchDescription([
        #action动作
        action_declare_startup_rqt,
        action_log_info,
        action_group
    ])
