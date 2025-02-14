//
// Created by jadjer on 24.08.22.
//

#pragma once

#include <Arduino.h>

/**
 * @brief
 * @param val
 * @return
 */
float calcValueDivide256(uint8_t val);

/**
 * @brief
 * @param val
 * @return
 */
uint8_t calcValueMinus40(uint8_t val);

/**
 * @brief
 * @param val
 * @return
 */
float calcValueDivide16(uint8_t val);

/**
 * @brief
 * @param val
 * @return
 */
float calcValueDivide10(uint8_t val);

/**
 * @brief
 * @param data
 * @param len
 * @return
 */
uint16_t calcChecksum(uint8_t const* data, size_t len);
