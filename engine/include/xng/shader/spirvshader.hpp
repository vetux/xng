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

#ifndef XENGINE_SPIRVSHADER_HPP
#define XENGINE_SPIRVSHADER_HPP

#include <stdexcept>

#include "spirvblob.hpp"

namespace xng {
    /**
     * A SPIRV shader contains an non owning reference to a SPIRV blob.
     */
    struct SPIRVShader {
        SPIRVShader() = default;

        SPIRVShader(ShaderStage stage, std::string entryPoint, SPIRVBlob &blob)
                : stage(stage), entryPoint(std::move(entryPoint)), blob(&blob) {}

        SPIRVShader(SPIRVShader &&other) = default;

        SPIRVShader(const SPIRVShader &other) = default;

        ShaderStage getStage() const {
            if (blob == nullptr)
                throw std::runtime_error("Shader not initialized");
            return stage;
        }

        const std::string &getEntryPoint() const {
            if (blob == nullptr)
                throw std::runtime_error("Shader not initialized");
            return entryPoint;
        }

        const SPIRVBlob &getBlob() const {
            if (blob == nullptr)
                throw std::runtime_error("Shader not initialized");
            return *blob;
        }

        bool operator==(const SPIRVShader &other) const {
            return stage == other.stage && entryPoint == other.entryPoint && blob == other.blob;
        }

    private:
        ShaderStage stage{};
        std::string entryPoint{};
        SPIRVBlob *blob = nullptr;
    };
}
#endif //XENGINE_SPIRVSHADER_HPP
