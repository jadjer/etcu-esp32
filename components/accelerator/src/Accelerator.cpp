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

#include <accelerator/Accelerator.hpp>
#include <driver/adc.h>
#include <esp_timer.h>

Accelerator::Accelerator() {
    ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_12));
    ESP_ERROR_CHECK(adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11));

    auto rawValue = adc1_get_raw(ADC1_CHANNEL_0);
    if (rawValue < 0) {
        rawValue = 0;
    }

    _minimalValue = rawValue;
    _lastStepTime_InUS = esp_timer_get_time();
}

Accelerator::~Accelerator() = default;

void Accelerator::registerChangeAccelerateCallback(AcceleratorChangeValueCallbackFunction const &acceleratorChangeValueCallbackFunction) {
    _acceleratorChangeValueCallbackFunction = acceleratorChangeValueCallbackFunction;
}

void Accelerator::process() {
    auto currentTime_InUS = esp_timer_get_time();
    auto diffTime_InUS = currentTime_InUS - _lastStepTime_InUS;
    if (diffTime_InUS < 10000) {
        return;
    }
    _lastStepTime_InUS = currentTime_InUS;

    auto rawValue = adc1_get_raw(ADC1_CHANNEL_0);
    if (rawValue < 0) {
        rawValue = 0;
    }

    if (rawValue < _minimalValue) {
        _minimalValue = rawValue;
    }

    auto currentValue = rawValue - _minimalValue;

    if (_acceleratorChangeValueCallbackFunction) {
        _acceleratorChangeValueCallbackFunction(currentValue);
    }
}


