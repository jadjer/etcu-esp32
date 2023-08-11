/* Blink C++ Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_log.h>
#include "FlexyStepper.hpp"

FlexyStepper flexyStepper(200, 16, 17, 18);

extern "C" void app_main(void) {
    flexyStepper.setAccelerationInStepsPerSecond(200);
    flexyStepper.setDecelerationInStepsPerSecond(200);
    flexyStepper.setSpeedInStepsPerSecond(500);

    flexyStepper.startAsService(1);

    flexyStepper.moveRelativeInSteps(500);
}
