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

#include <boost/function.hpp>
#include <map>
#include <rclcpp/rclcpp.hpp>
#include <string>

namespace ddynamic_reconfigure
{
inline void setRange(rcl_interfaces::msg::ParameterDescriptor& p, int min_value, int max_value)
{
  p.integer_range.resize(1);
  p.integer_range.front().from_value = static_cast<int64_t>(min_value);
  p.integer_range.front().to_value = static_cast<int64_t>(max_value);
}

inline void setRange(rcl_interfaces::msg::ParameterDescriptor& p, double min_value, double max_value)
{
  p.floating_point_range.resize(1);
  p.floating_point_range.front().from_value = min_value;
  p.floating_point_range.front().to_value = max_value;
}

inline void setRange(rcl_interfaces::msg::ParameterDescriptor&, bool, bool)
{
}

inline void setRange(rcl_interfaces::msg::ParameterDescriptor&, std::string, std::string)
{
}

template <typename T>
struct RegisteredParam : rcl_interfaces::msg::ParameterDescriptor
{
  RegisteredParam(const std::string& name, const std::string& description, T min_value, T max_value, T* variable)
    : variable_(variable)
  {
    this->name = name;
    this->read_only = false;
    this->description = description;
    if (std::is_same<T, int>::value)
    {
      this->type = rclcpp::ParameterType::PARAMETER_INTEGER;
      setRange(*this, min_value, max_value);
    }
    else if (std::is_same<T, double>::value)
    {
      this->type = rclcpp::ParameterType::PARAMETER_DOUBLE;
      setRange(*this, min_value, max_value);
    }
    else if (std::is_same<T, bool>::value)
    {
      this->type = rclcpp::ParameterType::PARAMETER_BOOL;
    }
    else if (std::is_same<T, std::string>::value)
    {
      this->type = rclcpp::ParameterType::PARAMETER_STRING;
    }
    else
    {
      throw std::runtime_error("Invalid parameter type");
    }
  }

  T* variable_;
};
}  // namespace ddynamic_reconfigure
