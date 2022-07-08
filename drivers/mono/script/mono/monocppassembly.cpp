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

#include <stdexcept>

#include "script/mono/monocppassembly.hpp"

#include "mono.hpp"

namespace xng {
    MonoCppAssembly::MonoCppAssembly(void *domainPointer, void *assemblyPointer, void *imagePointer)
            : domainPointer(domainPointer),
              assemblyPointer(assemblyPointer),
              imagePointer(imagePointer) {}

    MonoCppAssembly::~MonoCppAssembly() = default; //TODO: Unload assembly in destructor

    MonoCppObject MonoCppAssembly::invokeStaticMethod(const std::string &nameSpace,
                                                      const std::string &className,
                                                      const std::string &functionName,
                                                      const MonoCppArguments &args) const {
        auto *c = mono_class_from_name(reinterpret_cast<MonoImage *>(imagePointer),
                                       nameSpace.c_str(),
                                       className.c_str());
        if (c == nullptr)
            throw std::runtime_error("Failed to find class " + className);

        auto *m = mono_class_get_method_from_name(c, functionName.c_str(), args.count());
        if (m == nullptr)
            throw std::runtime_error("Failed to find method " + functionName);

        void *a[args.data().size()];
        for (int i = 0; i < args.count(); i++) {
            a[i] = args.data()[i];
        }

        return MonoCppObject(mono_runtime_invoke(m, nullptr, a, nullptr));
    }

    void MonoCppAssembly::setStaticField(const std::string &nameSpace,
                                         const std::string &className,
                                         const std::string &fieldName,
                                         MonoCppValue value) const {
        auto *c = mono_class_from_name(reinterpret_cast<MonoImage *>(imagePointer), nameSpace.c_str(),
                                       className.c_str());
        if (c == nullptr)
            throw std::runtime_error("Failed to find class " + className);
        auto *f = mono_class_get_field_from_name(c, fieldName.c_str());
        if (f == nullptr)
            throw std::runtime_error("Failed to find field " + fieldName);
        mono_field_static_set_value(mono_class_vtable(static_cast<MonoDomain *>(domainPointer), c), f, value.ptr);
    }

    MonoCppObject MonoCppAssembly::getStaticField(const std::string &nameSpace,
                                                  const std::string &className,
                                                  const std::string &fieldName) const {
        auto *c = mono_class_from_name(reinterpret_cast<MonoImage *>(imagePointer), nameSpace.c_str(),
                                       className.c_str());
        if (c == nullptr)
            throw std::runtime_error("Failed to find class " + className);
        auto *f = mono_class_get_field_from_name(c, fieldName.c_str());
        if (f == nullptr)
            throw std::runtime_error("Failed to find field " + fieldName);
        void *p = nullptr;
        mono_field_static_get_value(mono_class_vtable(static_cast<MonoDomain *>(domainPointer), c), f, &p);
        return std::move(MonoCppObject(p));
    }

    MonoCppObject MonoCppAssembly::createObject(const std::string &nameSpace,
                                                const std::string &className,
                                                bool pinned) const {
        auto *monoClass = mono_class_from_name(reinterpret_cast<MonoImage *>(imagePointer), nameSpace.c_str(),
                                               className.c_str());
        if (monoClass == nullptr)
            throw std::runtime_error("Class not found " + nameSpace + "." + className);
        auto *o = mono_object_new(static_cast<MonoDomain *>(domainPointer), monoClass);
        mono_runtime_object_init(o);
        return std::move(MonoCppObject(o, pinned));
    }
}