/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 3 of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef XENGINE_SHADERSCOPE_HPP
#define XENGINE_SHADERSCOPE_HPP

#include "xng/rendergraph/shader/shader.hpp"
#include "xng/rendergraph/shaderscript/shaderobject.hpp"

namespace xng::ShaderScript
{
    class ShaderScope
    {
    public:
        static ShaderScope& get()
        {
            if (getCurrent() == nullptr)
            {
                throw std::runtime_error("Current ShaderScope uninitialized");
            }
            return *getCurrent();
        }

        explicit ShaderScope(Shader::Stage stage)
            : stage(stage)
        {
            if (getCurrent() != nullptr)
            {
                throw std::runtime_error("Nested ShaderScope");
            }
            getCurrent() = this;
            BlockScope::getObjectCounter() = 0;
        }

        ~ShaderScope()
        {
            getCurrent() = nullptr;
        }

        //TODO / Maybe: Implement function overload support in DSL
        void addFunction(ShaderFunction func)
        {
            if (hasFunction(func.name))
            {
                throw std::runtime_error("Multiple Function Definition of function " + func.name);
            }
            functions.emplace_back(std::move(func));
        }

        bool hasFunction(const std::string& name) const
        {
            for (auto& f : functions)
            {
                if (f.name == name)
                {
                    return true;
                }
            }
            return false;
        }

        bool hasFunction(const std::string &name, const ShaderDataType &returnType, const std::vector<ShaderFunction::Argument> &args) const
        {
            for (auto& f : functions)
            {
                if (f.name == name && f.returnType == returnType && f.arguments == args)
                {
                    return true;
                }
            }
            return false;
        }

        void addTypeDefinition(ShaderStructDef type)
        {
            for (auto& t : typeDefinitions)
            {
                if (t.typeName == type.typeName)
                {
                    if (!(t == type))
                    {
                        throw std::runtime_error("Conflicting type definition: " + type.typeName);
                    }
                    return;
                }
            }
            typeDefinitions.emplace_back(type);
        }

        template <typename T>
        T addInput(const std::string& name)
        {
            if (inputLayout.checkElement(name))
            {
                throw std::runtime_error("Input already exists");
            }
            inputLayout.addElement(name, T::TYPE.getPrimitive());
            return T(ShaderOperand::inputAttribute(name));
        }

        template <typename T>
        T addOutput(const std::string& name)
        {
            if (outputLayout.checkElement(name))
            {
                throw std::runtime_error("Output already exists");
            }
            outputLayout.addElement(name, T::TYPE.getPrimitive());
            return T(ShaderOperand::outputAttribute(name));
        }

        void addParameter(const std::string& name, const ShaderPrimitiveType& type)
        {
            auto it = parameters.find(name);
            if (it != parameters.end())
            {
                if (it->second == type)
                {
                    return;
                }
                throw std::runtime_error("Parameter redefinition with different type");
            }
            parameters.emplace(name, type);
        }

        void addBuffer(const std::string& name, const ShaderBuffer& buffer)
        {
            auto it = buffers.find(name);
            if (it != buffers.end())
            {
                if (it->second == buffer)
                {
                    return;
                }
                throw std::runtime_error("Buffer redefinition with different type");
            }
            buffers.emplace(name, buffer);
        }

        void addTextureArray(const std::string& name, const ShaderTextureArray& textureArray)
        {
            auto it = textureArrays.find(name);
            if (it != textureArrays.end())
            {
                if (it->second == textureArray)
                {
                    return;
                }
                throw std::runtime_error("Texture Array redefinition wiith different type");
            }
            textureArrays.emplace(name, textureArray);
        }

        void setGeometryInput(RenderPrimitive input)
        {
            if (stage != Shader::GEOMETRY) throw std::runtime_error(
                "Attempted to set geometry input in non geometry stage");
            geometryInput = input;
        }

        void setGeometryOutput(RenderPrimitive output, size_t maxVertices)
        {
            if (stage != Shader::GEOMETRY) throw std::runtime_error(
                "Attempted to set geometry output in non geometry stage");
            geometryOutput = output;
            geometryMaxVertices = maxVertices;
        }

        Shader build()
        {
            ShaderFunction main;
            std::vector<ShaderFunction> funcs;
            for (auto& f : functions)
            {
                if (f.name == "main")
                {
                    main = f;
                }
                else
                {
                    funcs.push_back(f);
                }
            }
            return {
                stage,
                geometryInput,
                geometryOutput,
                geometryMaxVertices,
                inputLayout,
                outputLayout,
                parameters,
                buffers,
                textureArrays,
                typeDefinitions,
                main.body,
                funcs
            };
        }

    private:
        Shader::Stage stage;
        RenderPrimitive geometryInput{}; // The input primitive for geometry shaders. Must match the pipeline primitive.
        RenderPrimitive geometryOutput{}; // The output primitive for geometry shaders
        size_t geometryMaxVertices{}; // The maximum number of output vertices for geometry shaders.

        ShaderAttributeLayout inputLayout;
        ShaderAttributeLayout outputLayout;

        std::unordered_map<std::string, ShaderPrimitiveType> parameters{};
        std::unordered_map<std::string, ShaderBuffer> buffers{};
        std::unordered_map<std::string, ShaderTextureArray> textureArrays{};

        std::vector<ShaderStructDef> typeDefinitions{};

        std::vector<ShaderFunction> functions{};

        XENGINE_EXPORT static ShaderScope*& getCurrent();
    };
}

#endif //XENGINE_SHADERSCOPE_HPP
