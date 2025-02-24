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
#include <foc/encoder/BaseEncoder.hpp>

class AS5600 : public foc::BaseEncoder {
public:
  using Byte = std::uint8_t;
  using Angle = std::uint16_t;
  using Device = std::unique_ptr<i2c::Device>;
  using BusMaster = std::unique_ptr<i2c::Master>;

public:
  enum PowerMode {
    POWER_MODE_NORMAL = 0b00,
    POWER_MODE_LOW_1 = 0b01,
    POWER_MODE_LOW_2 = 0b10,
    POWER_MODE_LOW_3 = 0b11,
  };

  enum Hysteresis {
    HYSTERESIS_OFF = 0b00,
    HYSTERESIS_LSB = 0b01,
    HYSTERESIS_2_LSB = 0b10,
    HYSTERESIS_3_LSB = 0b11,
  };

  enum OutputStage {
    OUTPUT_STAGE_ANALOG_FULL = 0b00,
    OUTPUT_STAGE_ANALOG_REDUCED = 0b01,
    OUTPUT_STAGE_DIGITAL_PWM = 0b10,
  };

  enum PWMFrequency {
    FREQUENCY_115_HZ = 0b00,
    FREQUENCY_230_HZ = 0b01,
    FREQUENCY_460_HZ = 0b10,
    FREQUENCY_920_HZ = 0b11,
  };

  enum SlowFilter {
    SLOW_FILTER_X16 = 0b00,
    SLOW_FILTER_X8 = 0b01,
    SLOW_FILTER_X4 = 0b10,
    SLOW_FILTER_X2 = 0b11,
  };

  enum FastFilterThreshold {
    SLOW_FILTER_ONLY = 0b000,
    FAST_FILTER_THRESHOLD_6_LSB = 0b001,
    FAST_FILTER_THRESHOLD_7_LSB = 0b010,
    FAST_FILTER_THRESHOLD_9_LSB = 0b011,
    FAST_FILTER_THRESHOLD_18_LSB = 0b100,
    FAST_FILTER_THRESHOLD_21_LSB = 0b101,
    FAST_FILTER_THRESHOLD_24_LSB = 0b110,
    FAST_FILTER_THRESHOLD_10_LSB = 0b111,
  };

  struct __attribute__((__packed__)) Configuration {
    PowerMode powerMode : 2;
    Hysteresis hysteresis : 2;
    OutputStage outputStage : 2;
    PWMFrequency pwmFrequency : 2;
    SlowFilter slowFilter : 2;
    FastFilterThreshold fastFilterThreshold : 3;
    bool watchdog : 1;
    std::uint8_t reserved : 2;
  };

  struct __attribute__((__packed__)) Status {
    std::uint8_t reserved_1 : 3;
    bool magnetHigh : 1;
    bool magnetLow : 1;
    bool magnetDetected : 1;
    std::uint8_t reserved_2 : 2;
  };

public:
  AS5600();
  ~AS5600() override = default;

public:
  void setPowerMode(PowerMode powerMode);
  void setHysteresis(Hysteresis hysteresis);
  void setOutputStage(OutputStage outputStage);
  void setPWMFrequency(PWMFrequency pwmFrequency);
  void setSlowFilter(SlowFilter slowFilter);
  void setFastFilterThreshold(FastFilterThreshold fastFilterThreshold);
  void setWatchdog(bool enable);

public:
  float getMechanicalAngle() override;
  float getAngle() override;
  double getPreciseAngle() override;
  float getVelocity() override;
  int32_t getFullRotations() override;
  void update() override;
  int needsSearch() override;

protected:
  float getSensorAngle() override;
  void init() override;

public:
  [[nodiscard]] AS5600::Status getStatus();
  [[nodiscard]] AS5600::Configuration getConfiguration();

private:
  AS5600::Device m_device = nullptr;
  AS5600::BusMaster m_busMaster = nullptr;

private:
  Status m_status = {};
  Configuration m_configuration = {};
};
