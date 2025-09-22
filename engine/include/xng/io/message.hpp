/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
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
            SIGNED_INTEGER,
            UNSIGNED_INTEGER,
            FLOAT,
            STRING,
            DICTIONARY,
            LIST
        };

        static std::string getDataTypeName(DataType type) {
            switch (type) {
                case NUL:
                    return "NUL";
                case SIGNED_INTEGER:
                    return "INT";
                case UNSIGNED_INTEGER:
                    return "UINT";
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

        explicit Message(DataType type = NUL) : type(type) {
        }

        Message(int value) : type(SIGNED_INTEGER) { ival = value; }

        Message(long value) : type(SIGNED_INTEGER) { ival = value; }

        Message(long long value) : type(SIGNED_INTEGER) { ival = value; }

        Message(unsigned int value) : type(UNSIGNED_INTEGER) { uival = value; }

        Message(unsigned long value) : type(UNSIGNED_INTEGER) { uival = value; }

        Message(unsigned long long value) : type(UNSIGNED_INTEGER) { uival = value; }

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

        Message &operator[](const std::string &name) {
            if (type != DICTIONARY)
                throw std::runtime_error(
                    "Attempted to call array operator on message of type " + getDataTypeName(type));
            return mval[name];
        }

        const Message &operator[](const std::string &name) const {
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

        explicit operator char() const {
            if (type == UNSIGNED_INTEGER) {
                return static_cast<char>(uival);
            } else if (type == SIGNED_INTEGER) {
                return static_cast<char>(ival);
            } else {
                throw std::runtime_error("Attempted to cast message of type " + getDataTypeName(type) + " to char");
            }
        }

        explicit operator unsigned char() const {
            if (type == UNSIGNED_INTEGER) {
                return static_cast<unsigned char>(uival);
            } else if (type == SIGNED_INTEGER) {
                return static_cast<unsigned char>(ival);
            } else {
                throw std::runtime_error(
                    "Attempted to cast message of type " + getDataTypeName(type) + " to unsigned char");
            }
        }

        explicit operator int() const {
            if (type == UNSIGNED_INTEGER) {
                return static_cast<int>(uival);
            } else if (type == SIGNED_INTEGER) {
                return static_cast<int>(ival);
            } else {
                throw std::runtime_error("Attempted to cast message of type " + getDataTypeName(type) + " to int");
            }
        }

        explicit operator long() const {
            if (type == UNSIGNED_INTEGER) {
                return static_cast<long>(uival);
            } else if (type == SIGNED_INTEGER) {
                return static_cast<long>(ival);
            } else {
                throw std::runtime_error("Attempted to cast message of type " + getDataTypeName(type) + " to long");
            }
        }

        explicit operator long long() const {
            if (type == UNSIGNED_INTEGER) {
                return static_cast<long long>(uival);
            } else if (type == SIGNED_INTEGER) {
                return static_cast<long long>(ival);
            } else {
                throw std::runtime_error(
                    "Attempted to cast message of type " + getDataTypeName(type) + " to long long");
            }
        }

        explicit operator unsigned int() const {
            if (type == UNSIGNED_INTEGER) {
                return static_cast<unsigned int>(uival);
            } else {
                throw std::runtime_error(
                    "Attempted to cast message of type " + getDataTypeName(type) + " to unsigned int");
            }
        }

        explicit operator unsigned long() const {
            if (type == UNSIGNED_INTEGER) {
                return static_cast<unsigned long>(uival);
            } else {
                throw std::runtime_error(
                    "Attempted to cast message of type " + getDataTypeName(type) + " to unsigned long");
            }
        }

        explicit operator unsigned long long() const {
            if (type == UNSIGNED_INTEGER) {
                return static_cast<unsigned long long>(uival);
            } else {
                throw std::runtime_error(
                    "Attempted to cast message of type " + getDataTypeName(type) + " to unsigned long long");
            }
        }

        explicit operator bool() const {
            if (type == UNSIGNED_INTEGER) {
                return static_cast<bool>(uival);
            } else if (type == SIGNED_INTEGER) {
                return static_cast<bool>(ival);
            } else {
                throw std::runtime_error("Attempted to cast message of type " + getDataTypeName(type) + " to bool");
            }
        }

        explicit operator float() const {
            if (type == UNSIGNED_INTEGER) {
                return static_cast<float>(uival);
            } else if (type == SIGNED_INTEGER) {
                return static_cast<float>(ival);
            } else if (type == FLOAT) {
                return static_cast<float>(fval);
            } else {
                throw std::runtime_error("Attempted to cast message of type " + getDataTypeName(type) + " to float");
            }
        }

        explicit operator double() const {
            if (type == UNSIGNED_INTEGER) {
                return static_cast<double>(uival);
            } else if (type == SIGNED_INTEGER) {
                return static_cast<double>(ival);
            } else if (type == FLOAT) {
                return static_cast<double>(fval);
            } else {
                throw std::runtime_error("Attempted to cast message of type " + getDataTypeName(type) + " to double");
            }
        }

        explicit operator std::string() const {
            if (type == STRING) {
                return sval;
            } else {
                throw std::runtime_error("Attempted to cast message of type " + getDataTypeName(type) + " to string");
            }
        }

        explicit operator std::map<std::string, Message>() const {
            if (type == DICTIONARY) {
                return mval;
            } else {
                throw std::runtime_error(
                    "Attempted to cast message of type " + getDataTypeName(type) + " to dictionary");
            }
        }

        explicit operator std::vector<Message>() const {
            if (type == LIST) {
                return vval;
            } else {
                throw std::runtime_error("Attempted to cast message of type " + getDataTypeName(type) + " to list");
            }
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

        long long asLongLong() const {
            return as<long long>();
        }

        unsigned int asUInt() const {
            return as<unsigned int>();
        }

        unsigned long asULong() const {
            return as<unsigned long>();
        }

        unsigned long long asULongLong() const {
            return as<unsigned long long>();
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
            return as<std::map<std::string, Message> >();
        }

        std::vector<Message> asList() const {
            return as<std::vector<Message> >();
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

        long long ival = 0;
        unsigned long long uival = 0;
        double fval = 0;
        std::string sval = {};
        std::map<std::string, Message> mval = {};
        std::vector<Message> vval = {};
    };

    // Default Operators
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

    // Vector Operators
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

    // String Map operators
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

    // size_t Map operators
    template<typename T>
    std::map<size_t, T> &operator<<(std::map<size_t, T> &map, const Message &message) {
        map.clear();
        if (message.getType() == Message::DICTIONARY) {
            for (auto &pair: message.asDictionary()) {
                T val;
                val << pair.second;
                map[std::stoul(pair.first)] = val;
            }
        }
        return map;
    }

    template<typename T>
    Message &operator>>(const std::map<size_t, T> &map, Message &message) {
        std::map<std::string, Message> msgs;
        for (auto &pair: map) {
            Message msg;
            pair.second >> msg;
            msgs[std::to_string(pair.first)] = msg;
        }
        message = Message(msgs);
        return message;
    }

    // Set Operators
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
