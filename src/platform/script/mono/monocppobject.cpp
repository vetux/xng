/**
 *  XEngine - C++ game engine library
 *  Copyright (C) 2021  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "platform/script/mono/monocppobject.hpp"

#include <stdexcept>

#ifndef BUILD_ENGINE_SCRIPT_MONO

#define ERROR throw std::runtime_error("Mono support not built");

xengine::MonoCppObject::MonoCppObject() { ERROR }

xengine::MonoCppObject::MonoCppObject(void *objectPointer, bool pinned) { ERROR }

xengine::MonoCppObject::~MonoCppObject() {}

xengine::MonoCppObject::MonoCppObject(xengine::MonoCppObject &&other) noexcept {}

xengine::MonoCppObject &xengine::MonoCppObject::operator=(xengine::MonoCppObject &&other) noexcept { return other; }

xengine::MonoCppObject
xengine::MonoCppObject::invokeMethod(const std::string &name, const xengine::MonoCppArguments &args) const { ERROR }

void xengine::MonoCppObject::setField(const std::string &name, const xengine::MonoCppValue &value) const { ERROR }

void xengine::MonoCppObject::getFieldValuePtr(const std::string &name, void *data) const { ERROR }

bool xengine::MonoCppObject::isNull() const { ERROR }

bool xengine::MonoCppObject::isPinned() const { ERROR }

void *xengine::MonoCppObject::getObjectPointer() const { ERROR }

std::string xengine::MonoCppObject::getClassNamespace() const { ERROR }

std::string xengine::MonoCppObject::getClassName() const { ERROR }

#else

#include <mono/jit/jit.h>
#include <mono/metadata/loader.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>

namespace xengine {
    MonoCppObject::MonoCppObject() : objectPointer(nullptr), gcHandle(0), pinned(false) {}

    MonoCppObject::MonoCppObject(void *objectPointer, bool pinned) : objectPointer(objectPointer),
                                                                     gcHandle(0),
                                                                     pinned(pinned) {
        if (pinned) {
            gcHandle = mono_gchandle_new(static_cast<MonoObject *>(objectPointer), true);
        }
    }

    MonoCppObject::MonoCppObject(MonoCppObject &&other) noexcept {
        objectPointer = other.objectPointer;
        gcHandle = other.gcHandle;
        pinned = other.pinned;
        other.objectPointer = nullptr;
        other.gcHandle = 0;
        other.pinned = false;
    }

    MonoCppObject::~MonoCppObject() {
        if (pinned) {
            mono_gchandle_free(gcHandle);
        }
    }

    MonoCppObject &MonoCppObject::operator=(MonoCppObject &&other) noexcept {
        objectPointer = other.objectPointer;
        gcHandle = other.gcHandle;
        pinned = other.pinned;
        other.objectPointer = nullptr;
        other.gcHandle = 0;
        other.pinned = false;
        return *this;
    }

    MonoCppObject MonoCppObject::invokeMethod(const std::string &name, const MonoCppArguments &args) const {
        if (objectPointer == nullptr)
            throw std::runtime_error("Null object");

        auto *classPointer = mono_object_get_class(static_cast<MonoObject *>(objectPointer));
        auto *method = mono_class_get_method_from_name(classPointer, name.c_str(), args.count());
        if (method == nullptr)
            throw std::runtime_error("Failed to find method " + name);

        void *a[args.count()];
        for (int i = 0; i < args.count(); i++) {
            a[i] = args.data()[i];
        }

        return std::move(MonoCppObject(mono_runtime_invoke(method, objectPointer, a, nullptr)));
    }

    void MonoCppObject::setField(const std::string &name, const MonoCppValue &value) const {
        if (objectPointer == nullptr)
            throw std::runtime_error("Null object");
        auto *classPointer = mono_object_get_class(static_cast<MonoObject *>(objectPointer));
        auto *f = mono_class_get_field_from_name(classPointer, name.c_str());
        if (f == nullptr)
            throw std::runtime_error("Field not found " + name);
        mono_field_set_value(static_cast<MonoObject *>( objectPointer), f, value.ptr);
    }

    MonoCppObject MonoCppObject::getField(const std::string &name) const {
        if (objectPointer == nullptr)
            throw std::runtime_error("Null object");
        auto *classPointer = mono_object_get_class(static_cast<MonoObject *>(objectPointer));
        auto *f = mono_class_get_field_from_name(classPointer, name.c_str());
        if (f == nullptr)
            throw std::runtime_error("Field not found " + name);
        void *ob = nullptr;
        mono_field_get_value(static_cast<MonoObject *>(objectPointer), f, &ob);
        return std::move(MonoCppObject(ob));
    }

    void MonoCppObject::getFieldValuePtr(const std::string &name, void *data) const {
        if (objectPointer == nullptr)
            throw std::runtime_error("Null object");
        auto *classPointer = mono_object_get_class(static_cast<MonoObject *>(objectPointer));
        auto *f = mono_class_get_field_from_name(classPointer, name.c_str());
        if (f == nullptr)
            throw std::runtime_error("Field not found " + name);
        mono_field_get_value(static_cast<MonoObject *>(objectPointer), f, data);
    }

    bool MonoCppObject::isNull() const {
        return objectPointer == nullptr;
    }

    bool MonoCppObject::isPinned() const {
        return pinned;
    }

    void *MonoCppObject::getObjectPointer() const {
        return objectPointer;
    }

    std::string MonoCppObject::getClassNamespace() const {
        auto *c = mono_object_get_class(static_cast<MonoObject *>(objectPointer));
        if (c == nullptr)
            throw std::runtime_error("Failed to get class");
        return mono_class_get_namespace(c);
    }

    std::string MonoCppObject::getClassName() const {
        auto *c = mono_object_get_class(static_cast<MonoObject *>(objectPointer));
        if (c == nullptr)
            throw std::runtime_error("Failed to get class");
        return mono_class_get_name(c);
    }

    void *MonoCppObject::unbox() const {
        return mono_object_unbox(static_cast<MonoObject *>(objectPointer));
    }
}

#endif