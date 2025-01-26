//
// Created by jadjer on 26.01.25.
//

#include "servo/ServoBuilder.hpp"

#include <driver/ledc.h>
#include <esp_log.h>

auto const TAG = "ServoBuilder";

auto const FREQUENCY_MIN = 50;
auto const FREQUENCY_MAX = 400;

namespace servo {

ServoBuilder::ServoBuilder() : m_pin(0),
                               m_channel(0),
                               m_maxAngle(360),
                               m_minAngle(0),
                               m_maxWidth(2500),
                               m_minWidth(500),
                               m_frequency(50) {
}

ServoBuilder &ServoBuilder::setPin(std::uint8_t const pinNum) {
  m_pin = pinNum;

  return *this;
}

ServoBuilder &ServoBuilder::setChannel(std::uint8_t const channelNum) {
  m_channel = channelNum;

  return *this;
}

ServoBuilder &ServoBuilder::setFrequency(std::uint16_t const frequency) {
  m_frequency = frequency;

  return *this;
}

ServoBuilder &ServoBuilder::setAngle(std::uint16_t const minAngle, std::uint16_t const maxAngle) {
  m_minAngle = minAngle;
  m_maxAngle = maxAngle;

  return *this;
}

ServoBuilder &ServoBuilder::setWidth(std::uint16_t const minWidth, std::uint16_t const maxWidth) {
  m_minWidth = minWidth;
  m_maxWidth = maxWidth;

  return *this;
}

Servo *ServoBuilder::build() const {
  ESP_LOGI(TAG, "Channel %d", m_channel);
  ESP_LOGI(TAG, "Pin %d", m_pin);
  ESP_LOGI(TAG, "Frequency %d [hz]", m_frequency);
  ESP_LOGI(TAG, "Angle range from %d to %d [degree]", m_minAngle, m_maxAngle);
  ESP_LOGI(TAG, "Width range from %d to %d [us]", m_minWidth, m_maxWidth);

  if (m_channel < 0 or m_channel > LEDC_CHANNEL_MAX) {
    ESP_LOGE(TAG, "Servo channel number out the range");
    return nullptr;
  }

  if (m_frequency < FREQUENCY_MIN or m_frequency > FREQUENCY_MAX) {
    ESP_LOGE(TAG, "Servo pwm frequency out the range");
    return nullptr;
  }

  //  uint64_t pin_mask = 0;
  //  uint32_t ch_mask = 0;
  //  for (size_t i = 0; i < config->channel_number; i++) {
  //    uint64_t _pin_mask = 1ULL << config->channels.servo_pin[i];
  //    uint32_t _ch_mask = 1UL << config->channels.ch[i];
  //    SERVO_CHECK(!(pin_mask & _pin_mask), "servo gpio has a duplicate", ESP_ERR_INVALID_ARG);
  //    SERVO_CHECK(!(ch_mask & _ch_mask), "servo channel has a duplicate", ESP_ERR_INVALID_ARG);
  //    SERVO_CHECK(GPIO_IS_VALID_OUTPUT_GPIO(config->channels.servo_pin[i]), "servo gpio invalid", ESP_ERR_INVALID_ARG);
  //    pin_mask |= _pin_mask;
  //    ch_mask |= _ch_mask;
  //  }

  ledc_timer_config_t timerConfiguration = {
      .speed_mode = LEDC_HIGH_SPEED_MODE,  // timer mode
      .duty_resolution = LEDC_TIMER_14_BIT,// resolution of PWM duty
      .timer_num = LEDC_TIMER_0,           // timer index
      .freq_hz = m_frequency,              // frequency of PWM signal
      .clk_cfg = LEDC_AUTO_CLK,

  };

  if (ledc_timer_config(&timerConfiguration) != ESP_OK) {
    ESP_LOGE(TAG, "ledc timer configuration failed");
    return nullptr;
  }

  ledc_channel_config_t channelConfiguration = {
      .gpio_num = m_pin,
      .speed_mode = LEDC_HIGH_SPEED_MODE,
      .channel = static_cast<ledc_channel_t>(m_channel),
      .intr_type = LEDC_INTR_DISABLE,
      .timer_sel = LEDC_TIMER_0,
      .duty = 0,
      .hpoint = 0,
  };

  if (ledc_channel_config(&channelConfiguration) != ESP_OK) {
    ESP_LOGE(TAG, "ledc channel configuration failed");
    return nullptr;
  }

  return new Servo{m_channel, m_pin, m_frequency, m_minAngle, m_maxAngle, m_minWidth, m_maxWidth};
}

}// namespace servo
