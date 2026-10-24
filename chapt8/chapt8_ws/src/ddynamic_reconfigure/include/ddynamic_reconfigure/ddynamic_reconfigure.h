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

#pragma once

#include <ddynamic_reconfigure/ddynamic_reconfigure_utils.h>
#include <ddynamic_reconfigure/registered_param.h>

#include <atomic>
#include <functional>
#include <memory>
#include <rclcpp/rclcpp.hpp>
#include <rclcpp_lifecycle/lifecycle_node.hpp>

namespace ddynamic_reconfigure
{
/**
 * @brief The DDynamicReconfigure class allows to use ROS1 ddynamic reconfigure interface using the ROS2 parameter API
 */
template <typename NodeT>
class DDynamicReconfigure
{
public:
  /**
   * @param node Pointer to the ROS node
   */
  explicit DDynamicReconfigure(NodeT node);
  // TODO(Yannick de Hoop): Since C++17 template deduction in constructors is supported.
  /**
   * @brief registerVariable register a variable to be modified via the
   * dynamic_reconfigure API. When a change is made, it will be reflected in the
   * variable directly
   * @deprecated In the future this method will be merged with the registerVariable
   * that takes a pointer and a callback, but with the callback being optional
   */
  template <typename T>
  void registerVariable(const std::string& name, T* variable, const std::string& description = "", T min = getMin<T>(),
                        T max = getMax<T>());

  virtual ~DDynamicReconfigure() = default;

  /**
   * @brief publishServicesTopics starts the server once all the needed variables are
   * registered
   */
  virtual void publishServicesTopics();

  typedef std::function<void()> UserCallbackType;
  typedef std::function<void(const std::vector<rclcpp::Parameter>&)> UserCallbackType2;

  /**
   * @brief setUserCallback An optional callback that will be called whenever a value is
   * changed
   */
  virtual void setUserCallback(const UserCallbackType& callback);

  virtual void clearUserCallback();

  virtual void setUserCallback2(const UserCallbackType2& callback);

  virtual void clearUserCallback2();

protected:
  NodeT node_;

  rcl_interfaces::msg::SetParametersResult paramUpdatedCallback(const std::vector<rclcpp::Parameter>& parameters);
  rclcpp::node_interfaces::OnSetParametersCallbackHandle::SharedPtr param_updated_cb_;

  template <typename T>
  std::vector<std::shared_ptr<RegisteredParam<T>>>& getRegisteredVector();

  // Registered variables
  std::vector<std::shared_ptr<RegisteredParam<int>>> registered_int_;
  std::vector<std::shared_ptr<RegisteredParam<double>>> registered_double_;
  std::vector<std::shared_ptr<RegisteredParam<bool>>> registered_bool_;
  std::vector<std::shared_ptr<RegisteredParam<std::string>>> registered_string_;

  UserCallbackType user_callback_;
  UserCallbackType2 user_callback2_;
};

template <typename NodeT>
using DDynamicReconfigurePtr = std::shared_ptr<DDynamicReconfigure<NodeT>>;

template class DDynamicReconfigure<rclcpp::Node::SharedPtr>;
template class DDynamicReconfigure<rclcpp_lifecycle::LifecycleNode::SharedPtr>;
}  // namespace ddynamic_reconfigure
