/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2023  Julian Zampiccoli
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

#ifndef XENGINE_MESSAGE_HPP
#define XENGINE_MESSAGE_HPP

#include <map>
#include <vector>
#include <stdexcept>
#include <set>
#include <string>

namespace xng {
    class XENGINE_EXPORT Message;

    template<typename T>
    T &operator<<(T &v, const Message &message);

    class XENGINE_EXPORT Message {
    public:
        enum DataType {
            NUL,
            INT,
            FLOAT,
            STRING,
            DICTIONARY,
            LIST
        };

        static std::string getDataTypeName(DataType type) {
            switch (type) {
                case NUL:
                    return "NUL";
                case INT:
                    return "INT";
                case FLOAT:
                    return "FLOAT";
                case STRING:
                    return "STRING";
                case DICTIONARY:
                    return "DICTIONARY";
                case LIST:
                    return "LIST";
                default:
                    throw std::runtime_error("Invalid data type");
            }
        }

        explicit Message(DataType type = NUL) : type(type) {}

        Message(int value) : type(INT) { ival = value; }

        Message(long value) : type(INT) { ival = value; }

        Message(long long value) : type(INT) { ival = static_cast<int>(value); }

        Message(unsigned int value) : type(INT) { ival = static_cast<int>(value); }

        Message(unsigned long value) : type(INT) { ival = static_cast<int>(value); }

        Message(unsigned long long value) : type(INT) { ival = static_cast<int>(value); }

        Message(float value) : type(FLOAT) { fval = value; }

        Message(double value) : type(FLOAT) { fval = value; }

        Message(const std::string &value) : type(STRING) { sval = value; }

        Message(const std::map<std::string, Message> &value) : type(DICTIONARY) { mval = value; }

        Message(const std::vector<Message> &value) : type(LIST) { vval = value; }

        Message &operator[](const char *name) {
            if (type != DICTIONARY)
                throw std::runtime_error(
                        "Attempted to call array operator on message of type " + getDataTypeName(type));
            return mval[name];
        }

        const Message &operator[](const char *name) const {
            if (type != DICTIONARY)
                throw std::runtime_error(
                        "Attempted to call array operator on message of type " + getDataTypeName(type));
            return mval.at(name);
        }

        Message &operator[](int index) {
            if (type != LIST)
                throw std::runtime_error("Type error");
            if (vval.size() >= index)
                throw std::runtime_error(
                        "Attempted to call array operator on message of type " + getDataTypeName(type));
            return vval.at(index);
        }

        const Message &operator[](int index) const {
            if (type != LIST)
                throw std::runtime_error(
                        "Attempted to call array operator on message of type " + getDataTypeName(type));
            return vval.at(index);
        }

        bool has(const char *name) const {
            if (type != DICTIONARY)
                throw std::runtime_error("Attempted to call has on message of type " + getDataTypeName(type));
            return mval.find(name) != mval.end();
        }

        Message &at(const char *name) {
            if (type != DICTIONARY)
                throw std::runtime_error("Attempted to call at on message of type " + getDataTypeName(type));
            return mval.at(name);
        }

        const Message &at(const char *name) const {
            if (type != DICTIONARY)
                throw std::runtime_error("Attempted to call at on message of type " + getDataTypeName(type));
            return mval.at(name);
        }

        explicit operator int() const {
            return ival;
        }

        explicit operator long() const {
            return ival;
        }

        explicit operator long long() const {
            return ival;
        }

        explicit  operator unsigned int() const {
            return ival;
        }

        explicit  operator unsigned long() const {
            return ival;
        }

        explicit operator unsigned long long() const {
            return ival;
        }

        explicit operator bool() const {
            return ival;
        }

        explicit operator float() const {
            if (type == INT)
                return ival;
            return fval;
        }

        explicit operator double() const {
            if (type == INT)
                return ival;
            return fval;
        }

        explicit operator std::string() const {
            return sval;
        }

        explicit operator std::map<std::string, Message>() const {
            return mval;
        }

        explicit  operator std::vector<Message>() const {
            return vval;
        }

        DataType getType() const { return type; }

        template<typename T>
        T as() const {
            return static_cast<T>(*this);
        }

        int asInt() const {
            return as<int>();
        }

        long asLong() const {
            return as<long>();
        }

        bool asBool() const {
            return as<bool>();
        }

        float asFloat() const {
            return as<float>();
        }

        double asDouble() const {
            return as<double>();
        }

        std::string asString() const {
            return as<std::string>();
        }

        std::map<std::string, Message> asDictionary() const {
            return as<std::map<std::string, Message>>();
        }

        std::vector<Message> asList() const {
            return as<std::vector<Message>>();
        }

        template<typename T>
        bool valueOf(const std::set<std::string> &names, T &v, const T &defaultValue = T()) const {
            for (auto &name: names) {
                if (value(name, v, defaultValue)) {
                    return true;
                }
            }
            v = defaultValue;
            return false;
        }


        template<typename T>
        bool value(const std::string &name, T &v, const T &defaultValue = T()) const {
            auto it = mval.find(name);
            if (it != mval.end()) {
                v << it->second;
                return true;
            } else {
                v = defaultValue;
                return false;
            }
        }

        const Message &getMessageOf(const std::set<std::string> &names, const Message &defaultValue = Message()) const {
            for (auto &name: names) {
                auto it = mval.find(name);
                if (it != mval.end()) {
                    return it->second;
                }
            }
            return defaultValue;
        }

        const Message &getMessage(const std::string &name, const Message &defaultValue = Message()) const {
            return getMessageOf({name}, defaultValue);
        }

    private:
        DataType type;

        long ival = 0;
        double fval = 0;
        std::string sval = {};
        std::map<std::string, Message> mval = {};
        std::vector<Message> vval = {};
    };

    /**
     * These operators are invoked for any type which does not have an explicitly defined stream operator (Messageable or statically defined)
     * and is convertable from/to Message.
     *
     * @tparam T
     * @param v
     * @param message
     * @return
     */
    template<typename T>
    T &operator<<(T &v, const Message &message) {
        v = message.as<T>();
        return v;
    }

    template<typename T>
    Message &operator>>(const T &v, Message &message) {
        message = Message(v);
        return message;
    }

    /**
     * These operators invoke the streaming operators on a vector or map.
     *
     * @tparam T
     * @param vec
     * @param message
     * @return
     */
    template<typename T>
    std::vector<T> &operator<<(std::vector<T> &vec, const Message &message) {
        vec.clear();
        if (message.getType() == Message::LIST) {
            for (auto &msg: message.asList()) {
                T val;
                val << msg;
                vec.emplace_back(val);
            }
        }
        return vec;
    }

    template<typename T>
    Message &operator>>(const std::vector<T> &vec, Message &message) {
        std::vector<Message> msgs;
        for (auto &v: vec) {
            Message msg;
            v >> msg;
            msgs.emplace_back(msg);
        }
        message = Message(msgs);
        return message;
    }

    template<typename T>
    std::map<std::string, T> &operator<<(std::map<std::string, T> &map, const Message &message) {
        map.clear();
        if (message.getType() == Message::DICTIONARY) {
            for (auto &pair: message.asDictionary()) {
                T val;
                val << pair.second;
                map[pair.first] = val;
            }
        }
        return map;
    }

    template<typename T>
    Message &operator>>(const std::map<std::string, T> &map, Message &message) {
        std::map<std::string, Message> msgs;
        for (auto &pair: map) {
            Message msg;
            pair.second >> msg;
            msgs[pair.first] = msg;
        }
        message = Message(msgs);
        return message;
    }


    template<typename T>
    std::set<T> &operator<<(std::set<T> &set, const Message &message) {
        set.clear();
        if (message.getType() == Message::LIST) {
            for (auto &msg: message.asList()) {
                T value;
                value << msg;
                set.insert(value);
            }
        }
        return set;
    }

    template<typename T>
    Message &operator>>(const std::set<T> &set, Message &message) {
        std::vector<Message> msgs;
        for (auto &value: set) {
            Message msg;
            value >> msg;
            msgs.emplace_back(msg);
        }
        message = Message(msgs);
        return message;
    }
}

#endif //XENGINE_MESSAGE_HPP
