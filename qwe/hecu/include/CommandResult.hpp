//
// Created by jadjer on 31.08.22.
//

#pragma once

#include <Arduino.h>

struct CommandResult {
  uint8_t code;
  uint8_t command;
  uint8_t length;
  uint8_t checksum;
  uint8_t* data;
};
