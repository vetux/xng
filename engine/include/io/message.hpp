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

#ifndef XENGINE_MESSAGE_HPP
#define XENGINE_MESSAGE_HPP

#include <map>
#include <vector>
#include <stdexcept>
#include <set>

namespace xng {
    class XENGINE_EXPORT Message;

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

        Message(unsigned int value) : type(INT) { ival = value; }

        Message(long value) : type(INT) { ival = value; }

        Message(unsigned long value) : type(INT) { ival = value; }

        Message(float value) : type(FLOAT) { fval = value; }

        Message(double value) : type(FLOAT) { fval = value; }

        Message(const std::string &value) : type(STRING) { sval = value; }

        Message(const char *value) : type(STRING) { sval = std::string(value); }

        Message(const std::map<std::string, Message> &value) : type(DICTIONARY) { mval = value; }

        Message(const std::vector<Message> &value) : type(LIST) { vval = value; }

        Message &operator[](const char *name) {
            if (type != DICTIONARY)
                throw std::runtime_error("Attempted to call array operator on message of type " + getDataTypeName(type));
            return mval[name];
        }

        const Message &operator[](const char *name) const {
            if (type != DICTIONARY)
                throw std::runtime_error("Attempted to call array operator on message of type " + getDataTypeName(type));
            return mval.at(name);
        }

        Message &operator[](int index) {
            if (type != LIST)
                throw std::runtime_error("Type error");
            if (vval.size() >= index)
                throw std::runtime_error("Attempted to call array operator on message of type " + getDataTypeName(type));
            return vval.at(index);
        }

        const Message &operator[](int index) const {
            if (type != LIST)
                throw std::runtime_error("Attempted to call array operator on message of type " + getDataTypeName(type));
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

        operator int() const {
            if (type != INT)
                throw std::runtime_error(
                        "Attempted to perform invalid cast from message of type " + getDataTypeName(type) +
                        " to int");
            return this->ival;
        }

        operator long() const {
            if (type != INT)
                throw std::runtime_error(
                        "Attempted to perform invalid cast from message of type " + getDataTypeName(type) +
                        " to long");
            return ival;
        }

        operator unsigned long () const {
            if (type != INT)
                throw std::runtime_error(
                        "Attempted to perform invalid cast from message of type " + getDataTypeName(type) +
                        " to long");
            return ival;
        }

        operator bool() const {
            if (type != INT)
                throw std::runtime_error(
                        "Attempted to perform invalid cast from message of type " + getDataTypeName(type) +
                        " to bool");            return ival;
        }

        operator float() const {
            if (type == INT)
                return ival;
            if (type != FLOAT)
                throw std::runtime_error(
                        "Attempted to perform invalid cast from message of type " + getDataTypeName(type) +
                        " to float");            return fval;
        }

        operator double() const {
            if (type == INT)
                return ival;
            if (type != FLOAT)
                throw std::runtime_error(
                        "Attempted to perform invalid cast from message of type " + getDataTypeName(type) +
                        " to double");            return fval;
        }

        operator std::string() const {
            if (type != STRING)
                throw std::runtime_error(
                        "Attempted to perform invalid cast from message of type " + getDataTypeName(type) +
                        " to string");            return sval;
        }

        operator std::map<std::string, Message>() const {
            if (type != DICTIONARY)
                throw std::runtime_error(
                        "Attempted to perform invalid cast from message of type " + getDataTypeName(type) +
                        " to dictionary");            return mval;
        }

        operator std::vector<Message>() const {
            if (type != LIST)
                throw std::runtime_error(
                        "Attempted to perform invalid cast from message of type " + getDataTypeName(type) +
                        " to list");            return vval;
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

        std::map<std::string, Message> asMap() const {
            return as<std::map<std::string, Message>>();
        }

        std::vector<Message> asList() const {
            return as<std::vector<Message>>();
        }

        template<typename T>
        T valueOf(const std::set<std::string> &names, T defValue) const {
            for (auto &name: names) {
                auto it = mval.find(name);
                if (it != mval.end())
                    return it->second;
            }
            return defValue;
        }

        template<typename T>
        T value(const std::string &name, T defValue) const {
            return valueOf({name}, defValue);
        }

        Message value(const std::string &name) const {
            return valueOf({name}, Message());
        }

    private:
        DataType type;

        long ival;
        double fval;
        std::string sval;
        std::map<std::string, Message> mval;
        std::vector<Message> vval;
    };
}

#endif //XENGINE_MESSAGE_HPP
