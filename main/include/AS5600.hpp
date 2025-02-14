// Copyright 2025 Pavel Suprunov
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <i2c/Device.hpp>
#include <i2c/Master.hpp>

class AS5600 {
public:
  using Device = std::unique_ptr<i2c::Device>;
  using BusMaster = std::unique_ptr<i2c::Master>;

public:
  AS5600();

private:
  AS5600::Device m_device = nullptr;
  AS5600::BusMaster m_busMaster = nullptr;
};
