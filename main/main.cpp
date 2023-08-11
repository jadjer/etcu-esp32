/* Blink C++ Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_log.h>
#include "FlexyStepper.hpp"

FlexyStepper flexyStepper;

extern "C" void app_main(void) {
    flexyStepper.connectToPins(17, 18);
    flexyStepper.setEnablePin(19);
    flexyStepper.setResetPin(20);

    flexyStepper.setAccelerationInStepsPerSecond(200);
    flexyStepper.setDecelerationInStepsPerSecond(200);
    flexyStepper.setSpeedInStepsPerSecond(500);

    flexyStepper.startAsService(1);

    flexyStepper.moveRelativeInSteps(500);
}
