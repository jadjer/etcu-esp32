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

#include "bluetooth/Bluetooth.hpp"
#include "configuration/Configuration.hpp"
#include "controller/Controller.hpp"

//#include <motor/MotorController.hpp>
#include <motor/driver/TMC2209.hpp>
#include <esp_log.h>

//motor::MotorController motorController(motorDriver);

//motor::driver::TMC2209 motorDriver(1, 115200, 23, 22, motor::driver::SLAVE_ADDRESS_0);

TMC2209 motorDriver(1, SERIAL_ADDRESS_0, 22, 23);

extern "C" void app_main() {
  auto const settings = motorDriver.getSettings();
  ESP_LOGI("TMC", "SETTINGS ------>");
  ESP_LOGI("TMC", "Version: %d", motorDriver.getVersion());
  ESP_LOGI("TMC", "settings.is_communicating = %d", settings.is_communicating);
  ESP_LOGI("TMC", "settings.is_setup = %d", settings.is_setup);
  ESP_LOGI("TMC", "settings.software_enabled = %d", settings.software_enabled);
  ESP_LOGI("TMC", "settings.microsteps_per_step = %d", settings.microsteps_per_step);
  ESP_LOGI("TMC", "settings.inverse_motor_direction_enabled = %d", settings.inverse_motor_direction_enabled);
  ESP_LOGI("TMC", "settings.stealth_chop_enabled = %d", settings.stealth_chop_enabled);
  ESP_LOGI("TMC", "settings.irun_percent = %d", settings.irun_percent);
  ESP_LOGI("TMC", "settings.irun_register_value = %d", settings.irun_register_value);
  ESP_LOGI("TMC", "settings.ihold_percent = %d", settings.ihold_percent);
  ESP_LOGI("TMC", "settings.ihold_register_value = %d", settings.ihold_register_value);
  ESP_LOGI("TMC", "settings.iholddelay_register_value = %d", settings.iholddelay_register_value);
  ESP_LOGI("TMC", "settings.automatic_current_scaling_enabled = %d", settings.automatic_current_scaling_enabled);
  ESP_LOGI("TMC", "settings.automatic_gradient_adaptation_enabled = %d", settings.automatic_gradient_adaptation_enabled);
  ESP_LOGI("TMC", "settings.pwm_offset = %d", settings.pwm_offset);
  ESP_LOGI("TMC", "settings.pwm_gradient = %d", settings.pwm_gradient);
  ESP_LOGI("TMC", "settings.cool_step_enabled = %d", settings.cool_step_enabled);
  ESP_LOGI("TMC", "settings.analog_current_scaling_enabled = %d", settings.analog_current_scaling_enabled);
  ESP_LOGI("TMC", "settings.internal_sense_resistors_enabled = %d", settings.internal_sense_resistors_enabled);

  auto const status = motorDriver.getStatus();
  ESP_LOGI("TMC", "STATUS ------>");
  ESP_LOGI("TMC", "status.over_temperature_warning = %d", status.over_temperature_warning);
  ESP_LOGI("TMC", "status.over_temperature_shutdown = %d", status.over_temperature_shutdown);
  ESP_LOGI("TMC", "status.short_to_ground_a = %d", status.short_to_ground_a);
  ESP_LOGI("TMC", "status.short_to_ground_b = %d", status.short_to_ground_b);
  ESP_LOGI("TMC", "status.low_side_short_a = %d", status.low_side_short_a);
  ESP_LOGI("TMC", "status.low_side_short_b = %d", status.low_side_short_b);
  ESP_LOGI("TMC", "status.open_load_a = %d", status.open_load_a);
  ESP_LOGI("TMC", "status.open_load_b = %d", status.open_load_b);
  ESP_LOGI("TMC", "status.over_temperature_120c = %d", status.over_temperature_120c);
  ESP_LOGI("TMC", "status.over_temperature_143c = %d", status.over_temperature_143c);
  ESP_LOGI("TMC", "status.over_temperature_150c = %d", status.over_temperature_150c);
  ESP_LOGI("TMC", "status.over_temperature_157c = %d", status.over_temperature_157c);
  ESP_LOGI("TMC", "status.current_scaling = %d", status.current_scaling);
  ESP_LOGI("TMC", "status.stealth_chop_mode = %d", status.stealth_chop_mode);
  ESP_LOGI("TMC", "status.standstill = %d", status.standstill);

  //  auto configuration = std::make_shared<Configuration>();

  //  Bluetooth bluetooth(configuration);
  //  bluetooth.advertise();

  //  Controller controller(configuration);
  //  controller.spin();
}
