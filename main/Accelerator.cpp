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

//
// Created by jadjer on 15.08.23.
//

#include "Accelerator.hpp"

#include <esp_log.h>
#include <esp_adc/adc_continuous.h>
#include <esp_adc/adc_filter.h>
#include <esp_adc/adc_cali_scheme.h>

constexpr char const *tag = "accelerator";
constexpr adc_unit_t adcUnitNum = ADC_UNIT_1;
constexpr adc_channel_t adcChannelNum = ADC_CHANNEL_3;
constexpr adc_atten_t adcAttenuation = ADC_ATTEN_DB_12;
constexpr adc_bitwidth_t adcBitWidth = ADC_BITWIDTH_12;
constexpr adc_digi_convert_mode_t adcConvertMode = ADC_CONV_SINGLE_UNIT_1;
constexpr adc_digi_output_format_t adcOutputFormat = ADC_DIGI_OUTPUT_FORMAT_TYPE2;
constexpr adc_digi_iir_filter_coeff_t adcFilterCoefficient = ADC_DIGI_IIR_FILTER_COEFF_64;
constexpr adc_channel_t adcChannels[1] = {adcChannelNum};

constexpr uint32_t frameSize = 1024;

adc_continuous_handle_t adcHandle = nullptr;
adc_iir_filter_handle_t filterHandle = nullptr;
adc_cali_handle_t calibrationHandle = nullptr;

Accelerator::Accelerator() {
  adc_continuous_handle_cfg_t adcHandleConfiguration = {
      .max_store_buf_size = 1024,
      .conv_frame_size = frameSize,
  };
  ESP_ERROR_CHECK(adc_continuous_new_handle(&adcHandleConfiguration, &adcHandle));

  auto const numberOfChannelsUsed = sizeof(adcChannels) / sizeof(adc_channel_t);
  adc_digi_pattern_config_t adcDigitalPatternConfiguration[SOC_ADC_PATT_LEN_MAX] = {};
  for (int i = 0; i < numberOfChannelsUsed; i++) {
    adcDigitalPatternConfiguration[i].atten = adcAttenuation;
    adcDigitalPatternConfiguration[i].channel = adcChannels[i] & 0x7;
    adcDigitalPatternConfiguration[i].unit = adcUnitNum;
    adcDigitalPatternConfiguration[i].bit_width = adcBitWidth;
  }
  adc_continuous_config_t adcContinuousConfiguration = {
      .pattern_num = numberOfChannelsUsed,
      .adc_pattern = adcDigitalPatternConfiguration,
      .sample_freq_hz = 80 * 1000,
      .conv_mode = adcConvertMode,
      .format = adcOutputFormat
  };
  ESP_ERROR_CHECK(adc_continuous_config(adcHandle, &adcContinuousConfiguration));

  adc_continuous_iir_filter_config_t adcIirFilterConfiguration = {
      .unit = adcUnitNum,
      .channel = adcChannelNum,
      .coeff = adcFilterCoefficient
  };
  ESP_ERROR_CHECK(adc_new_continuous_iir_filter(adcHandle, &adcIirFilterConfiguration, &filterHandle));

  adc_cali_curve_fitting_config_t calibrationConfiguration = {
      .unit_id = adcUnitNum,
      .chan = adcChannelNum,
      .atten = adcAttenuation,
      .bitwidth = adcBitWidth,
  };
  ESP_ERROR_CHECK(adc_cali_create_scheme_curve_fitting(&calibrationConfiguration, &calibrationHandle));

  ESP_ERROR_CHECK(adc_continuous_iir_filter_enable(filterHandle));
  ESP_ERROR_CHECK(adc_continuous_start(adcHandle));
}

Accelerator::~Accelerator() {
  ESP_ERROR_CHECK(adc_continuous_stop(adcHandle));
  ESP_ERROR_CHECK(adc_continuous_iir_filter_disable(filterHandle));

  ESP_ERROR_CHECK(adc_cali_delete_scheme_curve_fitting(calibrationHandle));
  ESP_ERROR_CHECK(adc_del_continuous_iir_filter(filterHandle));

  ESP_ERROR_CHECK(adc_continuous_deinit(adcHandle));
}

void Accelerator::registerChangeAccelerateCallback(AcceleratorChangeValueCallbackFunction const &changeValueCallbackFunction) {
  m_changeValueCallbackFunction = changeValueCallbackFunction;
}

void Accelerator::process() {
  if (not m_changeValueCallbackFunction) {
    return;
  }

  uint8_t frame[frameSize] = {};
  uint32_t numberOfValuesInFrame = 0;

  esp_err_t returnCode = adc_continuous_read(adcHandle, frame, frameSize, &numberOfValuesInFrame, 0);
  if (returnCode != ESP_OK) {
    return;
  }

  uint32_t sumOfVoltagesPerFrame = 0;

  for (auto i = 0; i < numberOfValuesInFrame; i += SOC_ADC_DIGI_RESULT_BYTES) {
    auto *p = (adc_digi_output_data_t *) &frame[i];
    uint32_t data = p->type2.data;

    int voltage;

    ESP_ERROR_CHECK(adc_cali_raw_to_voltage(calibrationHandle, data, &voltage));

    sumOfVoltagesPerFrame += voltage;
  }

  auto const frameAverage = sumOfVoltagesPerFrame / numberOfValuesInFrame;

  m_changeValueCallbackFunction(frameAverage);
}
