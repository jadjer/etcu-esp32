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
// Created by jadjer on 30.08.22.
//

#include <button/Button.hpp>

#include <driver/gpio.h>

Button::Button(uint8_t pinNum, bool invertedValue) :
        m_pinNum(pinNum),
        m_invertedValue(invertedValue) {

    gpio_config_t buttonConfig = {
            .pin_bit_mask = (1ull << m_pinNum),
            .mode = GPIO_MODE_INPUT,
            .pull_up_en = GPIO_PULLUP_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_ENABLE,
            .intr_type = GPIO_INTR_DISABLE,
    };

    if (invertedValue) {
        buttonConfig.pull_up_en = GPIO_PULLUP_ENABLE;
        buttonConfig.pull_down_en = GPIO_PULLDOWN_DISABLE;
    }

    gpio_config(&buttonConfig);
}

Button::~Button() {
    gpio_isr_handler_remove(static_cast<gpio_num_t>(m_pinNum));
}

bool Button::isPressed() const {
    bool value = gpio_get_level(static_cast<gpio_num_t>(m_pinNum));

    if (m_invertedValue) {
        value = not value;
    }

    return value;
}

void Button::process() {}
