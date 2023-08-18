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

#include <list>
#include <executor/INode.hpp>

using Nodes = std::list<INodePtr>;

/**
 * @class Executor
 */
class Executor {
public:
    /**
     * Default constructor
     */
    Executor();

    /**
     * Default destructor
     */
    ~Executor();

public:
    /**
     * Node add to list
     * @param node Node ptr
     */
    void addNode(INodePtr node);
    /**
     * Node remove from list
     * @param node Node ptr
     */
    void removeNode(INodePtr const&node);

public:
    /**
     * Loop executor
     */
    void spin() const;

private:
    bool _enable;
    Nodes _nodes;
};
