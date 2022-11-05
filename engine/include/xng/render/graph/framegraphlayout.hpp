/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2022  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef XENGINE_FRAMEGRAPHLAYOUT_HPP
#define XENGINE_FRAMEGRAPHLAYOUT_HPP

#include <memory>
#include <vector>
#include <map>

#include "framegraphpass.hpp"

namespace xng {
    /**
     * A frame graph layout describes a set of passes and the dependencies between them.
     */
    class FrameGraphLayout {
    public:
        FrameGraphLayout() = default;

        FrameGraphLayout(const std::vector<std::shared_ptr<FrameGraphPass>> &passes,
                         std::map<std::type_index, std::type_index> dependencies)
                : passes(passes),
                  dependencies(dependencies) {
            std::set<std::type_index> passTypes;
            for (auto &pass: passes) {
                passTypes.insert(pass->getTypeName());
            }

            for (auto &dep: dependencies) {
                if (passTypes.find(dep.second) == passTypes.end()) {
                    throw std::runtime_error("Dependency for pass was not found");
                }
            }

            std::map<int, std::shared_ptr<FrameGraphPass>> passesCopy;
            std::vector<PassTreeElement> tree;
            while (!passesCopy.empty()) {
                std::set<int> delPasses;
                for (auto &pair: passesCopy) {
                    auto &pass = *pair.second;
                    auto it = dependencies.find(pass.getTypeName());
                    if (it == dependencies.end()) {
                        PassTreeElement element{.pass = *pair.second};
                        tree.emplace_back(element);
                        delPasses.insert(pair.first);
                    } else {
                        for (auto &treeElement: tree) {
                            auto *ptr = findTreeElementRecursive(it->second, treeElement);
                            if (ptr) {
                                PassTreeElement element{.pass =*pair.second};
                                element.pass = *pair.second;
                                ptr->childPasses.emplace_back(element);
                                delPasses.insert(pair.first);
                            }
                        }
                    }
                }
                for (auto &i: delPasses) {
                    passesCopy.erase(i);
                }
            }

            for (auto &treeElement: tree) {
                auto v = generatePassOrderRecursive(treeElement);
                passOrder.insert(passOrder.begin(), v.begin(), v.end());
            }
        }

        const std::vector<std::reference_wrapper<FrameGraphPass>> &getOrderedPasses() const { return passOrder; }

    private:
        struct PassTreeElement {
            std::reference_wrapper<FrameGraphPass> pass;
            std::vector<PassTreeElement> childPasses;
        };

        PassTreeElement *findTreeElementRecursive(std::type_index type, PassTreeElement &element) {
            if (element.pass.get().getTypeName() == type) {
                return &element;
            } else {
                for (auto &pass: element.childPasses) {
                    auto *ptr = findTreeElementRecursive(type, pass);
                    if (ptr) {
                        return ptr;
                    }
                }
                return nullptr;
            }
        }

        std::vector<std::reference_wrapper<FrameGraphPass>> generatePassOrderRecursive(const PassTreeElement &tree) {
            std::vector<std::reference_wrapper<FrameGraphPass>> ret;
            ret.emplace_back(tree.pass);
            for (auto &pass: tree.childPasses) {
                auto v = generatePassOrderRecursive(pass);
                ret.insert(ret.begin(), v.begin(), v.end());
            }
            return ret;
        }

        std::vector<std::shared_ptr<FrameGraphPass>> passes;
        std::map<std::type_index, std::type_index> dependencies;

        std::vector<std::reference_wrapper<FrameGraphPass>> passOrder;
    };
}

#endif //XENGINE_FRAMEGRAPHLAYOUT_HPP
