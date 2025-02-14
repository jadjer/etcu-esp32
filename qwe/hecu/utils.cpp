//
// Created by jadjer on 24.08.22.
//

#include "utils.hpp"

float calcValueDivide256(uint8_t val) {
  //convert to dec, multiple by 5, then divide result by 256
  //used for TPS Volt, ECT Volt, IAT Volt, MAP Volt
  return static_cast<float>(val) * 5 / 256;
}

uint8_t calcValueMinus40(uint8_t val) {
  //value minus 40
  //used for ECT Temp, IAT Temp
  return val - 40;
}

float calcValueDivide16(uint8_t val) {
  //value divided by 16 and times 10
  //used for TPS%
  return static_cast<float>(val) * 10 / 16;
}

float calcValueDivide10(uint8_t val) {
  return static_cast<float>(val) / 10;
}

uint16_t calcChecksum(uint8_t const* data, size_t len) {
  uint16_t checkSum = 0;

  for (size_t i = 0; i < len; i++) {
    checkSum -= data[i];
  }

  return checkSum;
}
