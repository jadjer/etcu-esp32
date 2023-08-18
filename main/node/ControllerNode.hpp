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
// Created by jadjer on 17.08.23.
//

#pragma once

#include <memory>

#include <executor/INode.hpp>

#include "Controller.hpp"

using ControllerPtr = std::shared_ptr<Controller>;

class ControllerNode : public INode {
public:
    explicit ControllerNode(ControllerPtr  controllerPtr);

    ~ControllerNode() override;

protected:
    void spinOnce() override;

private:
    ControllerPtr _controllerPtr;
};
