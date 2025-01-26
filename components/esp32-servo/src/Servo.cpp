//
// Created by jadjer on 25.01.25.
//

#include "servo/Servo.hpp"

#include <esp_log.h>

auto const TAG = "Servo";

auto const FREQUENCY_MIN = 50;
auto const FREQUENCY_MAX = 400;

namespace servo {

Servo::Servo(
    std::uint8_t const channel,
    std::uint8_t const pinNum,
    std::uint16_t const frequency,
    std::uint16_t const minAngle,
    std::uint16_t const maxAngle,
    std::uint16_t const minWidth,
    std::uint16_t const maxWidth) : m_pin(pinNum),
                                    m_channel(channel),
                                    m_maxAngle(maxAngle),
                                    m_minAngle(minAngle),
                                    m_maxWidth(maxWidth),
                                    m_minWidth(minWidth),
                                    m_frequency(frequency) {
}

void Servo::setFrequency(std::uint16_t const frequency) {
  m_frequency = frequency;
}

void Servo::setWidth(std::uint16_t const minWidthUD, std::uint16_t const maxWidthUS) {
  m_minWidth = minWidthUD;
  m_maxWidth = maxWidthUS;
}

void Servo::move(float angle) const {
  ESP_LOGI(TAG, "Move servo to angle %f", angle);
}

std::uint32_t Servo::calculateDuty(float angle) {
  return 0;
}

float Servo::calculateAngle(uint32_t duty) {
  return 0;
}

}// namespace servo
