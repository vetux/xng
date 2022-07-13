/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2021  Julian Zampiccoli
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

#include "io/protocol/jsonprotocol.hpp"

#include "extern/json.hpp"

namespace xng {
    nlohmann::json convertMessage(const Message &message) {
        nlohmann::json ret;
        switch (message.getType()) {
            case Message::INT:
                ret = message.as<long>();
                break;
            case Message::FLOAT:
                ret = message.as<double>();
                break;
            case Message::STRING:
                ret = message.as<std::string>();
                break;
            case Message::DICTIONARY:
                for (auto &m : message.as<std::map<std::string, Message>>()) {
                    ret[m.first] = convertMessage(m.second);
                }
                break;
            case Message::LIST:
                for (auto &m : message.as<std::vector<Message>>()) {
                    ret.emplace_back(convertMessage(m));
                }
                break;
        }
        return ret;
    }

    Message convertMessage(const nlohmann::json &j) {
        if (j.is_number_integer()) {
            return j.get<long>();
        } else if (j.is_number_float()) {
            return j.get<double>();
        } else if (j.is_boolean()){
            return j.get<bool>();
        } else if (j.is_string()) {
            return j.get<std::string>();
        } else if (j.is_array()) {
            std::vector<Message> msgs;
            for (auto &n : j) {
                msgs.emplace_back(convertMessage(n));
            }
            return msgs;
        } else {
            std::map<std::string, Message> msgs;
            for (auto &it : j.get<nlohmann::json::object_t>()) {
                msgs[it.first] = convertMessage(it.second);
            }
            return msgs;
        }
    }

    void JsonProtocol::serialize(std::ostream &stream, const Message &message) {
        stream << convertMessage(message);
    }

    Message JsonProtocol::deserialize(std::istream &stream) {
        nlohmann::json j;
        stream >> j;
        return convertMessage(j);
    }
}