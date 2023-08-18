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
// Created by jadjer on 16.08.23.
//

#pragma once

#include <executor/INode.hpp>
#include <accelerator/Accelerator.hpp>

using AcceleratorPtr = std::shared_ptr<Accelerator>;

class AcceleratorNode : public INode {
public:
    explicit AcceleratorNode(AcceleratorPtr acceleratorPtr);
    ~AcceleratorNode() override;

protected:
    void spinOnce() override;

private:
    AcceleratorPtr _acceleratorPtr;
};
