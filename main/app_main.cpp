// Copyright 2025 Pavel Suprunov
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

#include <servo/ServoBuilder.hpp>

extern "C" void app_main() {
  auto const servo = servo::ServoBuilder()
                         .setChannel(0)
                         .setPin(19)
                         .setAngle(0, 90)
                         .setWidth(1000, 2000)
                         .setFrequency(400)
                         .build();

  if (not servo) {
    return;
  }

  servo->move(45);
}
