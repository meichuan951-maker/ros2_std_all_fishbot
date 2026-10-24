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

#include <utility>

namespace ddynamic_reconfigure
{
template <typename NodeT>
DDynamicReconfigure<NodeT>::DDynamicReconfigure(NodeT node) : node_(node)
{
}

template <>
template <>
std::vector<std::shared_ptr<RegisteredParam<int>>>& DDynamicReconfigure<rclcpp::Node::SharedPtr>::getRegisteredVector()
{
  return registered_int_;
}

template <>
template <>
std::vector<std::shared_ptr<RegisteredParam<int>>>&
DDynamicReconfigure<rclcpp_lifecycle::LifecycleNode::SharedPtr>::getRegisteredVector()
{
  return registered_int_;
}

template <>
template <>
std::vector<std::shared_ptr<RegisteredParam<double>>>&
DDynamicReconfigure<rclcpp::Node::SharedPtr>::getRegisteredVector()
{
  return registered_double_;
}

template <>
template <>
std::vector<std::shared_ptr<RegisteredParam<double>>>&
DDynamicReconfigure<rclcpp_lifecycle::LifecycleNode::SharedPtr>::getRegisteredVector()
{
  return registered_double_;
}

template <>
template <>
std::vector<std::shared_ptr<RegisteredParam<bool>>>& DDynamicReconfigure<rclcpp::Node::SharedPtr>::getRegisteredVector()
{
  return registered_bool_;
}

template <>
template <>
std::vector<std::shared_ptr<RegisteredParam<bool>>>&
DDynamicReconfigure<rclcpp_lifecycle::LifecycleNode::SharedPtr>::getRegisteredVector()
{
  return registered_bool_;
}

template <>
template <>
std::vector<std::shared_ptr<RegisteredParam<std::string>>>&
DDynamicReconfigure<rclcpp::Node::SharedPtr>::getRegisteredVector()
{
  return registered_string_;
}

template <>
template <>
std::vector<std::shared_ptr<RegisteredParam<std::string>>>&
DDynamicReconfigure<rclcpp_lifecycle::LifecycleNode::SharedPtr>::getRegisteredVector()
{
  return registered_string_;
}

template <typename NodeT>
template <typename T>
void DDynamicReconfigure<NodeT>::registerVariable(const std::string& name, T* variable, const std::string& description,
                                                  T min, T max)
{
  auto param = std::make_shared<RegisteredParam<T>>(name, description, min, max, variable);
  *param->variable_ = declare_parameter_if_not_declared(node_, param->name, *param->variable_, *param);
  getRegisteredVector<T>().push_back(param);
}

template <typename NodeT>
void DDynamicReconfigure<NodeT>::publishServicesTopics()
{
  param_updated_cb_ = node_->add_on_set_parameters_callback(
      std::bind(&DDynamicReconfigure::paramUpdatedCallback, this, std::placeholders::_1));
}

template <typename NodeT>
void DDynamicReconfigure<NodeT>::setUserCallback(const DDynamicReconfigure::UserCallbackType& callback)
{
  user_callback_ = callback;
}

template <typename NodeT>
void DDynamicReconfigure<NodeT>::clearUserCallback()
{
  user_callback_ = {};
}

template <typename NodeT>
void DDynamicReconfigure<NodeT>::setUserCallback2(const DDynamicReconfigure::UserCallbackType2& callback)
{
  user_callback2_ = callback;
}

template <typename NodeT>
void DDynamicReconfigure<NodeT>::clearUserCallback2()
{
  user_callback2_ = {};
}

rcl_interfaces::msg::SetParametersResult getResult(bool successful = true, const std::string& reason = "")
{
  rcl_interfaces::msg::SetParametersResult result;
  result.successful = successful;
  result.reason = reason;
  return result;
}

template <typename T>
std::shared_ptr<RegisteredParam<T>> getParam(const std::string& name,
                                             std::vector<std::shared_ptr<RegisteredParam<T>>> params)
{
  for (const auto& p : params)
  {
    if (p->name == name)
    {
      return p;
    }
  }
  return nullptr;
}

template <typename NodeT>
rcl_interfaces::msg::SetParametersResult
DDynamicReconfigure<NodeT>::paramUpdatedCallback(const std::vector<rclcpp::Parameter>& parameters)
{
  RCLCPP_DEBUG(node_->get_logger(), "paramUpdatedCallback");

  bool param_changed = false;
  try
  {
    for (const auto& p : parameters)
    {
      if (p.get_type() == rclcpp::ParameterType::PARAMETER_INTEGER)
      {
        auto param = getParam(p.get_name(), registered_int_);
        if (param)
        {
          *getParam(p.get_name(), registered_int_)->variable_ = static_cast<int>(p.as_int());
          param_changed = true;
        }
      }
      else if (p.get_type() == rclcpp::ParameterType::PARAMETER_DOUBLE)
      {
        auto param = getParam(p.get_name(), registered_double_);
        if (param)
        {
          *getParam(p.get_name(), registered_double_)->variable_ = p.as_double();
          param_changed = true;
        }
      }
      else if (p.get_type() == rclcpp::ParameterType::PARAMETER_BOOL)
      {
        auto param = getParam(p.get_name(), registered_bool_);
        if (param)
        {
          *getParam(p.get_name(), registered_bool_)->variable_ = p.as_bool();
          param_changed = true;
        }
      }
      else if (p.get_type() == rclcpp::ParameterType::PARAMETER_STRING)
      {
        auto param = getParam(p.get_name(), registered_string_);
        if (param)
        {
          *getParam(p.get_name(), registered_string_)->variable_ = p.as_string();
          param_changed = true;
        }
      }
      else
      {
        throw std::runtime_error("Invalid type '" + p.get_type_name() + "' for parameter '" + p.get_name() + "'");
      }
    }
  }
  catch (const std::runtime_error& e)
  {
    return getResult(false, e.what());
  }

  if (param_changed)
  {
    if (user_callback_)
    {
      user_callback_();
    }

    if (user_callback2_)
    {
      user_callback2_(parameters);
    }
  }

  return getResult();
}

// Explicit int instantiations
template void DDynamicReconfigure<rclcpp::Node::SharedPtr>::registerVariable(const std::string& name, int* variable,
                                                                             const std::string& description, int min,
                                                                             int max);
template void DDynamicReconfigure<rclcpp_lifecycle::LifecycleNode::SharedPtr>::registerVariable(
    const std::string& name, int* variable, const std::string& description, int min, int max);

// Explicit double instantiations
template void DDynamicReconfigure<rclcpp::Node::SharedPtr>::registerVariable(const std::string& name, double* variable,
                                                                             const std::string& description, double min,
                                                                             double max);
template void DDynamicReconfigure<rclcpp_lifecycle::LifecycleNode::SharedPtr>::registerVariable(
    const std::string& name, double* variable, const std::string& description, double min, double max);

// Explicit bool instantiations
template void DDynamicReconfigure<rclcpp::Node::SharedPtr>::registerVariable(const std::string& name, bool* variable,
                                                                             const std::string& description, bool min,
                                                                             bool max);
template void DDynamicReconfigure<rclcpp_lifecycle::LifecycleNode::SharedPtr>::registerVariable(
    const std::string& name, bool* variable, const std::string& description, bool min, bool max);

// Explicit std::string instantiations
template void DDynamicReconfigure<rclcpp::Node::SharedPtr>::registerVariable(const std::string& name,
                                                                             std::string* variable,
                                                                             const std::string& description,
                                                                             std::string min, std::string max);
template void DDynamicReconfigure<rclcpp_lifecycle::LifecycleNode::SharedPtr>::registerVariable(
    const std::string& name, std::string* variable, const std::string& description, std::string min, std::string max);
}  // namespace ddynamic_reconfigure
