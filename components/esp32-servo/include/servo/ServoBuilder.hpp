//
// Created by jadjer on 26.01.25.
//

#pragma once

#include <cstdint>
#include <servo/Servo.hpp>

namespace servo {

class ServoBuilder {
public:
  explicit ServoBuilder();

public:
  ServoBuilder &setPin(std::uint8_t pinNum);
  ServoBuilder &setChannel(std::uint8_t channelNum);
  ServoBuilder &setFrequency(std::uint16_t frequency);
  ServoBuilder &setAngle(std::uint16_t minAngle, std::uint16_t maxAngle);
  ServoBuilder &setWidth(std::uint16_t minWidth, std::uint16_t maxWidth);

public:
  [[nodiscard]] Servo *build() const;

private:
  std::uint8_t m_pin;
  std::uint8_t m_channel;
  std::uint16_t m_maxAngle;
  std::uint16_t m_minAngle;
  std::uint16_t m_maxWidth;
  std::uint16_t m_minWidth;
  std::uint16_t m_frequency;
};

}// namespace servo
