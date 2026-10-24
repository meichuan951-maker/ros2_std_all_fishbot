/**
 * Copyright 2019 PAL Robotics S.L.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <ddynamic_reconfigure/ddynamic_reconfigure.h>

#include <rclcpp/rclcpp.hpp>
#include <rclcpp_lifecycle/lifecycle_node.hpp>

using namespace ddynamic_reconfigure;

void callback()
{
  std::cout << "callback!" << std::endl;
}

int main(int argc, char** argv)
{
  rclcpp::init(argc, argv);

  auto node = std::make_shared<rclcpp::Node>("fake_dynamic_reconfigure");
  auto lifecycle_node = std::make_shared<rclcpp_lifecycle::LifecycleNode>("fake_dynamic_reconfigure");
  {
    double double_test = 0.0;
    double double_range = 2;
    int int_test = 0;
    bool bool_test = false;
    std::string str_test = "";

    DDynamicReconfigure<rclcpp::Node::SharedPtr> ddr(node);
    DDynamicReconfigure<rclcpp_lifecycle::LifecycleNode::SharedPtr> ddr_ln(lifecycle_node);

    ddr.registerVariable("bool_test", &bool_test, "An awesome boolean!");
    ddr.registerVariable("double_test", &double_test, "An awesome double!");
    ddr.registerVariable("double_range_test", &double_range, "Double range awesome!", 0., 10.);
    ddr.registerVariable("int_test", &int_test, "Cool int!");
    ddr.registerVariable("str_test", &str_test, "I am a little string!");
    ddr.publishServicesTopics();
    ddr.setUserCallback(std::bind(&callback));

    ddr_ln.registerVariable("bool_test", &bool_test, "An awesome boolean!");
    ddr_ln.registerVariable("double_test", &double_test, "An awesome double!");
    ddr_ln.registerVariable("double_range_test", &double_range, "Double range awesome!", 0., 10.);
    ddr_ln.registerVariable("int_test", &int_test, "Cool int!");
    ddr_ln.registerVariable("str_test", &str_test, "I am a little string!");
    ddr_ln.publishServicesTopics();
    ddr_ln.setUserCallback(std::bind(&callback));

    while (rclcpp::ok())
    {
      rclcpp::spin_some(node);
      std::cout << "double_test " << double_test << std::endl;
      std::cout << "double_range_test (0..10) " << double_range << std::endl;
      std::cout << "int_test " << int_test << std::endl;
      std::cout << "bool_test " << bool_test << std::endl;
      std::cout << "str_test " << str_test << std::endl;
      std::cout << "*********" << std::endl;
      rclcpp::sleep_for(std::chrono::seconds(1));
    }
  }
  rclcpp::shutdown();
  return 0;
}
