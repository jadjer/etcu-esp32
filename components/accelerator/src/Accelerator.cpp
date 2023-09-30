// Copyright 2023 Pavel Suprunov
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

//
// Created by jadjer on 15.08.23.
//

#include "accelerator/Accelerator.hpp"

#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

namespace accelerator
{

constexpr adc_unit_t adcUnitNum = ADC_UNIT_1;
constexpr adc_channel_t adcChannelNum = ADC_CHANNEL_3;
constexpr adc_atten_t adcAttenuation = ADC_ATTEN_DB_11;
constexpr adc_bitwidth_t adcBitWidth = ADC_BITWIDTH_12;

adc_oneshot_unit_handle_t adcHandle = nullptr;
adc_cali_handle_t adcCalibrationHandle = nullptr;

uint32_t getVoltageFromAdc()
{
    int rawValue = 0;
    int voltage = 0;

    ESP_ERROR_CHECK(adc_oneshot_read(adcHandle, adcChannelNum, &rawValue));
    ESP_ERROR_CHECK(adc_cali_raw_to_voltage(adcCalibrationHandle, rawValue, &voltage));

    return voltage;
}

Accelerator::Accelerator()
{
    adc_oneshot_unit_init_cfg_t unitConfiguration = {
        .unit_id = adcUnitNum,
        .clk_src = ADC_RTC_CLK_SRC_DEFAULT,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&unitConfiguration, &adcHandle));

    adc_oneshot_chan_cfg_t channelConfiguration = {
        .atten = adcAttenuation,
        .bitwidth = adcBitWidth,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adcHandle, adcChannelNum, &channelConfiguration));

    adc_cali_curve_fitting_config_t calibrationConfig = {
        .unit_id = adcUnitNum,
        .chan = adcChannelNum,
        .atten = adcAttenuation,
        .bitwidth = adcBitWidth,
    };
    ESP_ERROR_CHECK(adc_cali_create_scheme_curve_fitting(&calibrationConfig, &adcCalibrationHandle));
}

Accelerator::~Accelerator()
{
    ESP_ERROR_CHECK(adc_cali_delete_scheme_curve_fitting(adcCalibrationHandle));
    ESP_ERROR_CHECK(adc_oneshot_del_unit(adcHandle));
}

void Accelerator::registerChangeAccelerateCallback(AcceleratorChangeValueCallbackFunction const& changeValueCallbackFunction)
{
    m_changeValueCallbackFunction = changeValueCallbackFunction;
}

void Accelerator::process()
{
    if (not m_changeValueCallbackFunction)
    {
        return;
    }

    auto currentAcceleratorVoltage = getVoltageFromAdc();

    m_changeValueCallbackFunction(currentAcceleratorVoltage);
}

} // namespace accelerator
