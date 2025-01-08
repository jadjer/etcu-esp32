// Copyright 2024 Pavel Suprunov
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

//
// Created by jadjer on 9/20/24.
//

#pragma once

#include <nvs_handle.hpp>

#include "configuration/interface/Configuration.hpp"

class Configuration : public interface::Configuration {
public:
  Configuration();
  ~Configuration() override;

public:
  [[nodiscard]] bool isLubricate() const override;
  [[nodiscard]] bool isManualLubricate() const override;
  [[nodiscard]] std::uint8_t getExternalPowerPin() const override;
  [[nodiscard]] std::uint8_t getPumpPin() const override;
  [[nodiscard]] std::uint8_t getWheelSensorPin() const override;
  [[nodiscard]] std::uint32_t getPumpTimeout() const override;
  [[nodiscard]] float getWheelLength() const override;
  [[nodiscard]] float getMinimalSpeed() const override;
  [[nodiscard]] float getDistanceForEnable() const override;
  [[nodiscard]] float getTotalDistance() const override;
  [[nodiscard]] float getNextDistance() const override;

public:
  void setLubricate(bool lubricate) override;
  void setManualLubricate(bool lubricate) override;
  void setPumpTimeout(std::uint32_t timeout) override;
  void setWheelLength(float wheelLength) override;
  void setMinimalSpeed(float minimalSpeed) override;
  void setDistanceForEnable(float distance) override;
  void saveTotalDistance(float distance) override;
  void saveNextDistance(float distance) override;

private:
  bool m_isManualLubricate;
  nvs_handle_t m_storageHandle;
};
