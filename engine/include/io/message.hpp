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

#ifndef XENGINE_MESSAGE_HPP
#define XENGINE_MESSAGE_HPP

#include <map>
#include <vector>
#include <stdexcept>

namespace xng {
    class XENGINE_EXPORT Message;

    class XENGINE_EXPORT Message {
    public:
        enum DataType {
            INT,
            FLOAT,
            STRING,
            DICTIONARY,
            LIST
        };

        Message() : type(INT) { ival = 0; }

        Message(int value) : type(INT) { ival = value; }

        Message(long value) : type(INT) { ival = value; }

        Message(float value) : type(FLOAT) { fval = value; }

        Message(double value) : type(FLOAT) { fval = value; }

        Message(const std::string &value) : type(STRING) { sval = value; }

        Message(const char *value) : type(STRING) { sval = std::string(value); }

        Message(const std::map<std::string, Message> &value) : type(DICTIONARY) { mval = value; }

        Message(const std::vector<Message> &value) : type(LIST) { vval = value; }

        Message &operator[](const char *name) {
            if (type != DICTIONARY)
                throw std::runtime_error("Type error");
            return mval[name];
        }

        const Message &operator[](const char *name) const {
            if (type != DICTIONARY)
                throw std::runtime_error("Type error");
            return mval.at(name);
        }

        Message &operator[](int index) {
            if (type != LIST)
                throw std::runtime_error("Type error");
            return vval.at(index);
        }

        const Message &operator[](int index) const {
            if (type != LIST)
                throw std::runtime_error("Type error");
            return vval.at(index);
        }

        Message &at(const char *name) {
            if (type != DICTIONARY)
                throw std::runtime_error("Type error");
            return mval.at(name);
        }

        const Message &at(const char *name) const {
            if (type != DICTIONARY)
                throw std::runtime_error("Type error");
            return mval.at(name);
        }

        operator int() const {
            if (type != INT)
                throw std::runtime_error("Type mismatch");
            return this->ival;
        }

        operator long() const {
            if (type != INT)
                throw std::runtime_error("Type mismatch");
            return ival;
        }

        operator bool() const {
            if (type != INT)
                throw std::runtime_error("Type mismatch");
            return ival;
        }

        operator float() const {
            if (type == INT)
                return ival;
            if (type != FLOAT)
                throw std::runtime_error("Type mismatch");
            return fval;
        }

        operator double() const {
            if (type == INT)
                return ival;
            if (type != FLOAT)
                throw std::runtime_error("Type mismatch");
            return fval;
        }

        operator std::string() const {
            if (type != STRING)
                throw std::runtime_error("Type mismatch");
            return sval;
        }

        operator std::map<std::string, Message>() const {
            if (type != DICTIONARY)
                throw std::runtime_error("Type mismatch");
            return mval;
        }

        operator std::vector<Message>() const {
            if (type != LIST)
                throw std::runtime_error("Type mismatch");
            return vval;
        }

        DataType getType() const { return type; }

        template<typename T>
        T get() const {
            return static_cast<T>(*this);
        }

        int getInt() const {
            return get<int>();
        }

        long getLong() const {
            return get<long>();
        }

        bool getBool() const {
            return get<bool>();
        }

        float getFloat() const {
            return get<float>();
        }

        double getDouble() const {
            return get<double>();
        }

        std::string getString() const {
            return get<std::string>();
        }

        std::map<std::string, Message> getMap() const {
            return get<std::map<std::string, Message>>();
        }

        std::vector<Message> getVector() const {
            return get<std::vector<Message>>();
        }

        template<typename T>
        T value(const std::string &name, T defValue) const {
            auto it = mval.find(name);
            if (it != mval.end())
                return it->second;
            else
                return defValue;
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
