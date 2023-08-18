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
// Created by jadjer on 15.08.23.
//

#pragma once

#include <cstdlib>
#include <functional>

using AcceleratorChangeValueCallbackFunction = std::function<void(uint32_t)>;

class Accelerator {
public:
    Accelerator();
    ~Accelerator();

public:
    void registerChangeAccelerateCallback(AcceleratorChangeValueCallbackFunction const& acceleratorChangeValueCallbackFunction);

public:
    void process();

private:
    AcceleratorChangeValueCallbackFunction _acceleratorChangeValueCallbackFunction = nullptr;

private:
    uint32_t _minimalValue;
    uint32_t _lastStepTime_InUS;
};
