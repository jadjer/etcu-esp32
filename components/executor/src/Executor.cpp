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

#include <executor/Executor.hpp>

Executor::Executor() :
        _enable(true),
        _nodes() {
}

Executor::~Executor() {
    _enable = false;
    _nodes.clear();
}

void Executor::addNode(INodePtr node) {
    _nodes.push_back(std::move(node));
}

void Executor::removeNode(INodePtr const &node) {
    _nodes.remove(node);
}

void Executor::spin() const {
    while (_enable) {
        for (auto const &node: _nodes) {
            node->spinOnce();
        }
    }
}
