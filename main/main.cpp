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

#include <esp_log.h>
#include "FlexyStepper.hpp"

FlexyStepper flexyStepper(13, 12);

extern "C" void app_main(void) {
    flexyStepper.setAccelerationInStepsPerSecondPerSecond(300000);
    flexyStepper.setDecelerationInStepsPerSecondPerSecond(300000);
    flexyStepper.setSpeedInStepsPerSecond(60000);

    flexyStepper.startAsService(1);

    flexyStepper.setTargetPositionInSteps(1000000);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    flexyStepper.setTargetPositionInSteps(-1000000);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    flexyStepper.setTargetPositionToStop();
}
