//
// Created by jadjer on 25.01.25.
//

#pragma once

#include <cstdint>

namespace servo {

class ServoBuilder;

class Servo {
  friend ServoBuilder;

protected:
  Servo(std::uint8_t channel, std::uint8_t pinNum, std::uint16_t frequency, std::uint16_t minAngle, std::uint16_t maxAngle, std::uint16_t minWidth, std::uint16_t maxWidth);

public:
  virtual ~Servo() = default;

public:
  void setFrequency(std::uint16_t frequency);
  void setWidth(std::uint16_t minWidthUD, std::uint16_t maxWidthUS);

public:
  void move(float angle) const;

private:
  std::uint32_t calculateDuty(float angle);
  float calculateAngle(uint32_t duty);

private:
  std::uint8_t const m_pin;
  std::uint8_t const m_channel;

private:
  std::uint16_t m_maxAngle;
  std::uint16_t m_minAngle;
  std::uint16_t m_maxWidth;
  std::uint16_t m_minWidth;
  std::uint16_t m_frequency;
};

}// namespace servo

#include <memory>

using ServoPtr = std::shared_ptr<servo::Servo>;
