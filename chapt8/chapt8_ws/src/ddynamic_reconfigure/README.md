# ddynamic_reconfigure

ROS2 port of the [ddynamic_reconfigure ROS1 package](https://github.com/pal-robotics/ddynamic_reconfigure) in order
to facilitate a smooth transition from `ros1` to `ros2`. Not everything is ported but the API should be more or less
consistent for a relative easy port of your ros1 app to ros2.

If you are looking for a dedicated solution, it might be better to check out alternatives,
e.g. https://github.com/PickNikRobotics/generate_parameter_library

## Example

```
$ ros2 run ddynamic_reconfigure fake_dynamic_reconfigure_server 
double_test 0
double_range_test (0..10) 2
int_test 0
bool_test 0
str_test 
*********
```

```
$ ros2 param list
/fake_dynamic_reconfigure:
  bool_test
  double_range_test
  double_test
  int_test
  qos_overrides./parameter_events.publisher.depth
  qos_overrides./parameter_events.publisher.durability
  qos_overrides./parameter_events.publisher.history
  qos_overrides./parameter_events.publisher.reliability
  str_test
  use_sim_time
```

```
$ ros2 param describe /fake_dynamic_reconfigure bool_test double_range_test double_test int_test str_test
Parameter name: bool_test
  Type: boolean
  Description: An awesome boolean!
  Constraints:
Parameter name: double_range_test
  Type: double
  Description: Double range awesome!
  Constraints:
    Min value: 0.0
    Max value: 10.0
Parameter name: double_test
  Type: double
  Description: An awesome double!
  Constraints:
    Min value: 2.2250738585072014e-308
    Max value: 1.7976931348623157e+308
Parameter name: int_test
  Type: integer
  Description: Cool int!
  Constraints:
    Min value: -2147483648
    Max value: 2147483647
Parameter name: str_test
  Type: string
  Description: I am a little string!
  Constraints:
```

```
$ ros2 param set /fake_dynamic_reconfigure bool_test true
Set parameter successful
```
