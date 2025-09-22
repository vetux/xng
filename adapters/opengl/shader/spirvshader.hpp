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

#ifndef XENGINE_SPIRVSHADER_HPP
#define XENGINE_SPIRVSHADER_HPP

#include <stdexcept>
#include <utility>

#include "shader/spirvblob.hpp"
#include "shader/shaderenvironment.hpp"

namespace xng {
    struct SPIRVShader {
        SPIRVShader() = default;

        SPIRVShader(ShaderEnvironment environment, ShaderStageType stage, std::string entryPoint, SPIRVBlob blob)
                : environment(environment), stage(stage), entryPoint(std::move(entryPoint)), blob(std::move(blob)) {}

        SPIRVShader(SPIRVShader &&other) = default;

        SPIRVShader(const SPIRVShader &other) = default;

        SPIRVShader &operator=(const SPIRVShader &other) = default;

        ShaderEnvironment getEnvironment() const {
            return environment;
        }

        ShaderStageType getStage() const {
            return stage;
        }

        const std::string &getEntryPoint() const {
            return entryPoint;
        }

        const SPIRVBlob &getBlob() const {
            return blob;
        }

        bool operator==(const SPIRVShader &other) const {
            return environment == other.environment
                   && stage == other.stage
                   && entryPoint == other.entryPoint
                   && blob == other.blob;
        }

    private:
        ShaderEnvironment environment{};
        ShaderStageType stage{};
        std::string entryPoint{};
        SPIRVBlob blob;
    };
}
#endif //XENGINE_SPIRVSHADER_HPP
