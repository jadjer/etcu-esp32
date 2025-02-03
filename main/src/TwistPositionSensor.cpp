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

#include "TwistPositionSensor.hpp"

#include <esp_adc/adc_cali_scheme.h>
#include <esp_adc/adc_continuous.h>
#include <esp_adc/adc_filter.h>
#include <esp_log.h>

auto const TAG = "Twist Position Sensor";

adc_unit_t const adcUnitNum = ADC_UNIT_1;
adc_channel_t const adcChannelNum = ADC_CHANNEL_3;
adc_atten_t const adcAttenuation = ADC_ATTEN_DB_12;
adc_bitwidth_t const adcBitWidth = ADC_BITWIDTH_12;
adc_digi_convert_mode_t const adcConvertMode = ADC_CONV_SINGLE_UNIT_1;
adc_digi_output_format_t const adcOutputFormat = ADC_DIGI_OUTPUT_FORMAT_TYPE2;
adc_channel_t const adcChannels[1] = {adcChannelNum};
uint32_t const frameSize = 256;

adc_digi_iir_filter_coeff_t const adcFilterCoefficient = ADC_DIGI_IIR_FILTER_COEFF_64;

adc_continuous_handle_t adcHandle = nullptr;
adc_cali_handle_t calibrationHandle = nullptr;
adc_iir_filter_handle_t filterHandle = nullptr;

TwistPositionSensor::TwistPositionSensor() : m_trashHoldVoltage_InMillivolts(10),
                                             m_lastValue_InMillivolts(0) {

  //  adc_continuous_handle_cfg_t adcHandleConfiguration = {
  //      .max_store_buf_size = 1024,
  //      .conv_frame_size = frameSize,
  //  };
  //  ESP_ERROR_CHECK(adc_continuous_new_handle(&adcHandleConfiguration, &adcHandle));
  //
  //  auto const numberOfChannelsUsed = sizeof(adcChannels) / sizeof(adc_channel_t);
  //
  //  adc_digi_pattern_config_t adcDigitalPatternConfiguration[numberOfChannelsUsed] = {};
  //  for (int i = 0; i < numberOfChannelsUsed; i++) {
  //    adcDigitalPatternConfiguration[i].atten = adcAttenuation;
  //    adcDigitalPatternConfiguration[i].channel = adcChannels[i] & 0x7;
  //    adcDigitalPatternConfiguration[i].unit = adcUnitNum;
  //    adcDigitalPatternConfiguration[i].bit_width = adcBitWidth;
  //  }
  //
  //  adc_continuous_config_t adcContinuousConfiguration = {
  //      .pattern_num = numberOfChannelsUsed,
  //      .adc_pattern = adcDigitalPatternConfiguration,
  //      .sample_freq_hz = 80 * 1000,
  //      .conv_mode = adcConvertMode,
  //      .format = adcOutputFormat};
  //  ESP_ERROR_CHECK(adc_continuous_config(adcHandle, &adcContinuousConfiguration));
  //
  //  adc_cali_line_fitting_config_t calibrationConfiguration = {
  //      .unit_id = adcUnitNum,
  //      .atten = adcAttenuation,
  //      .bitwidth = adcBitWidth,
  //  };
  //  ESP_ERROR_CHECK(adc_cali_create_scheme_line_fitting(&calibrationConfiguration, &calibrationHandle));
  //
  //  adc_continuous_iir_filter_config_t adcIirFilterConfiguration = {
  //      .unit = adcUnitNum,
  //      .channel = adcChannelNum,
  //      .coeff = adcFilterCoefficient};
  //  ESP_ERROR_CHECK(adc_new_continuous_iir_filter(adcHandle, &adcIirFilterConfiguration, &filterHandle));
  //  ESP_ERROR_CHECK(adc_continuous_iir_filter_enable(filterHandle));
  //
  //  ESP_ERROR_CHECK(adc_continuous_start(adcHandle));
}

TwistPositionSensor::~TwistPositionSensor() {
  //  ESP_ERROR_CHECK(adc_continuous_stop(adcHandle));
  //
  //  ESP_ERROR_CHECK(adc_continuous_iir_filter_disable(filterHandle));
  //  ESP_ERROR_CHECK(adc_del_continuous_iir_filter(filterHandle));
  //
  //  ESP_ERROR_CHECK(adc_cali_delete_scheme_line_fitting(calibrationHandle));
  //
  //  ESP_ERROR_CHECK(adc_continuous_deinit(adcHandle));
}

TwistPositionSensor::Voltage TwistPositionSensor::getVoltage() const {
  return m_lastValue_InMillivolts;
}

void TwistPositionSensor::process() {
  //  uint8_t frame[frameSize] = {0};
  //  uint32_t numberOfValuesInFrame = 0;
  //
  //  esp_err_t returnCode = adc_continuous_read(adcHandle, frame, frameSize, &numberOfValuesInFrame, 0);
  //  if (returnCode != ESP_OK) {
  //    return;
  //  }
  //
  //  uint32_t valueCount = 0;
  //  uint32_t sumOfRawDataPerFrame = 0;
  //
  //  for (auto i = 0; i < numberOfValuesInFrame; i += SOC_ADC_DIGI_RESULT_BYTES) {
  //    auto *adcDigitalOutputData = (adc_digi_output_data_t *) &frame[i];
  //    uint32_t const data = adcDigitalOutputData->type2.data;
  //
  //    valueCount += 1;
  //    sumOfRawDataPerFrame += data;
  //  }
  //
  //  auto const rawAverageData = sumOfRawDataPerFrame / valueCount;
  //
  //  int voltage_InMillivolts = 0;
  //
  //  ESP_ERROR_CHECK(adc_cali_raw_to_voltage(calibrationHandle, rawAverageData, &voltage_InMillivolts));
  //
  //  uint32_t voltageDifference_InMillivolts = 0;
  //
  //  if (voltage_InMillivolts == m_lastValue_InMillivolts) {
  //    return;
  //  }
  //
  //  if (voltage_InMillivolts > m_lastValue_InMillivolts) {
  //    voltageDifference_InMillivolts = voltage_InMillivolts - m_lastValue_InMillivolts;
  //  }
  //
  //  if (voltage_InMillivolts < m_lastValue_InMillivolts) {
  //    voltageDifference_InMillivolts = m_lastValue_InMillivolts - voltage_InMillivolts;
  //  }
  //
  //  if (voltageDifference_InMillivolts > m_trashHoldVoltage_InMillivolts) {
  //    m_lastValue_InMillivolts = voltage_InMillivolts;
  //  }
}
