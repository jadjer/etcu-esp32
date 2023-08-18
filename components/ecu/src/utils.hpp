// Copyright 2023 Pavel Suprunov
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
// Created by jadjer on 24.08.22.
//

#pragma once

#include <cstdlib>

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
