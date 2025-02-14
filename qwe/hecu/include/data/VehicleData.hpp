//
// Created by jadjer on 07.09.22.
//

#pragma once

#include <string>

struct VehicleData {
  std::string id;
  float batteryVolts;
  uint8_t speed;
  uint8_t state;
};
