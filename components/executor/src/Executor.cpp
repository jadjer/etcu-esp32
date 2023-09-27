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

#include "Executor/Executor.hpp"

namespace Executor
{

Executor::Executor() : m_nodes(), m_isEnabled(true) {}

Executor::~Executor()
{
    m_isEnabled = false;
}

void Executor::addNode(Interface::NodePtr const& node)
{
    std::lock_guard lock(m_mutex);
    m_nodes.push_back(node);
}

void Executor::removeNode(Interface::NodePtr const& node)
{
    std::lock_guard lock(m_mutex);
    m_nodes.remove(node);
}

void Executor::spin() const
{
    while (m_isEnabled)
    {
        for (auto const& node : m_nodes)
        {
            node->spinOnce();
        }
    }
}

} // namespace Executor
