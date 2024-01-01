/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2024  Julian Zampiccoli
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

#ifndef XENGINE_RIG_HPP
#define XENGINE_RIG_HPP

#include <map>
#include <utility>
#include <vector>
#include <string>

#include "bone.hpp"

namespace xng {
    struct XENGINE_EXPORT Rig : public Messageable {
        Rig() = default;

        explicit Rig(std::vector<Bone> bones, std::map<std::string, std::string> boneParentMapping)
                : bones(std::move(bones)), boneParentMapping(std::move(boneParentMapping)) {
            for (auto i = 0; i < this->bones.size(); i++) {
                auto &bone = this->bones.at(i);
                boneNameMapping[bone.name] = i;
            }
        }

        Rig(const Rig &other) = default;

        Rig(Rig &&other) = default;

        ~Rig() = default;

        Rig &operator=(const Rig &other) = default;

        Rig &operator=(Rig &&other) = default;

        const std::vector<Bone> &getBones() const { return bones; }

        Bone &getBone(size_t index) { return bones.at(index); }

        Bone &getBone(const std::string &name) { return bones.at(boneNameMapping.at(name)); }

        bool hasBone(const std::string &name) { return boneNameMapping.find(name) != boneNameMapping.end(); }

        Bone &getParentBone(const std::string &name) {
            return bones.at(boneNameMapping.at(boneParentMapping.at(name)));
        }

        bool hasParentBone(const std::string &name) { return boneParentMapping.find(name) != boneParentMapping.end(); }

        std::vector<std::reference_wrapper<Bone>> getChildBones(const std::string &name) {
            std::vector<std::reference_wrapper<Bone>> ret;
            for (auto &pair: boneParentMapping) {
                if (pair.second == name) {
                    ret.emplace_back(bones.at(boneNameMapping.at(pair.first)));
                }
            }
            return ret;
        }

        std::vector<std::reference_wrapper<Bone>> getRootBones() {
            std::vector<std::reference_wrapper<Bone>> ret;
            for (auto &bone: bones) {
                if (boneParentMapping.find(bone.name) == boneParentMapping.end()) {
                    ret.emplace_back(bone);
                }
            }
            return ret;
        }

        bool hasBones() {
            return !bones.empty();
        }

        Messageable &operator<<(const Message &message) override {
            message.value("bones", bones);
            boneNameMapping.clear();
            for (auto i = 0; i < this->bones.size(); i++) {
                auto &bone = this->bones.at(i);
                boneNameMapping[bone.name] = i;
            }
            for (auto &pair: message["boneParents"].asDictionary()) {
                boneParentMapping[pair.first] = pair.second.asString();
            }
            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            bones >> message["bones"];
            message["boneParents"] = Message(Message::DICTIONARY);
            for (auto &pair: boneParentMapping) {
                pair.second >> message["boneParents"].asDictionary()[pair.first];
            }
            return message;
        }

    private:
        std::vector<Bone> bones;
        std::map<std::string, size_t> boneNameMapping;
        std::map<std::string, std::string> boneParentMapping;
    };
}

#endif //XENGINE_RIG_HPP
