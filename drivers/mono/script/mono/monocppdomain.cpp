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

#include "script/mono/monocppdomain.hpp"

#include <iterator>
#include <string>
#include <sstream>

#include <mono/jit/jit.h>
#include <mono/metadata/loader.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/mono-config.h>

namespace xng {
    MonoCppDomain::MonoCppDomain()
            : msCorLib(nullptr, nullptr, nullptr) {
        domainPointer = mono_jit_init("DefaultDomain");
        if (domainPointer == nullptr)
            throw std::runtime_error("Failed to create mono domain DefaultDomain");
        mono_config_parse(nullptr);
        msCorLib = MonoCppAssembly(domainPointer, mono_image_get_assembly(mono_get_corlib()), mono_get_corlib());
    }

    MonoCppDomain::MonoCppDomain(const std::string &domainName)
            : msCorLib(nullptr, nullptr, nullptr) {
        domainPointer = mono_jit_init(domainName.c_str());
        if (domainPointer == nullptr)
            throw std::runtime_error("Failed to create mono domain " + domainName);
        mono_config_parse(nullptr);
        msCorLib = MonoCppAssembly(domainPointer, mono_image_get_assembly(mono_get_corlib()), mono_get_corlib());
    }

    MonoCppDomain::~MonoCppDomain() {
        mono_jit_cleanup(static_cast<MonoDomain *>(domainPointer));
    }

    MonoCppAssembly &MonoCppDomain::getMsCorLibAssembly() {
        return msCorLib;
    }

    std::unique_ptr<MonoCppAssembly> MonoCppDomain::loadAssembly(const std::string &filePath) {
        auto *ap = mono_domain_assembly_open(static_cast<MonoDomain *>(domainPointer), filePath.c_str());
        if (ap == nullptr)
            throw std::runtime_error("Failed to load assembly " + filePath);
        return std::make_unique<MonoCppAssembly>(domainPointer, ap, mono_assembly_get_image(ap));
    }

    std::unique_ptr<MonoCppAssembly> MonoCppDomain::loadAssembly(std::istream &source) {
        auto ret = std::make_unique<MonoCppAssembly>();
        ret->imageBytes = std::string((std::istreambuf_iterator<char>(source)),
                                      std::istreambuf_iterator<char>());
        MonoImageOpenStatus status;
        auto *image = mono_image_open_from_data(ret->imageBytes.data(), ret->imageBytes.size(), false, &status);
        if (image == nullptr || status != MONO_IMAGE_OK)
            throw std::runtime_error("Failed to open image " + std::to_string(status));

        auto *assembly = mono_assembly_load_from(image, "", &status);
        if (assembly == nullptr || status != MONO_IMAGE_OK)
            throw std::runtime_error("Failed to create assembly " + std::to_string(status));

        ret->imagePointer = image;
        ret->assemblyPointer = assembly;
        ret->domainPointer = domainPointer;

        return ret;
    }

    MonoCppObject MonoCppDomain::stringFromUtf8(const std::string &str, bool pinned) {
        auto *p = mono_string_new(static_cast<MonoDomain *>(domainPointer), str.c_str());
        return std::move(MonoCppObject(p, pinned));
    }

    std::string MonoCppDomain::stringToUtf8(const MonoCppObject &strObject) {
        return mono_string_to_utf8(static_cast<MonoString *>(strObject.getObjectPointer()));
    }
}