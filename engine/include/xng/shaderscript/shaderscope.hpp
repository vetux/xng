/**
 *   xEngine - C++ Game Engine Library
 *   Copyright (C) 2026 Julia Zampiccoli
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the Lesser General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef XENGINE_SHADERSCOPE_HPP
#define XENGINE_SHADERSCOPE_HPP

#include "xng/rendergraph/shader/shader.hpp"
#include "xng/shaderscript/shaderobject.hpp"

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

        explicit ShaderScope(rg::Shader::Stage stage)
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
        void addFunction(rg::ShaderFunction func)
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

        bool hasFunction(const std::string &name, const rg::ShaderDataType &returnType, const std::vector<rg::ShaderFunction::Argument> &args) const
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

        void addTypeDefinition(rg::ShaderStructDef type)
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
            return T(rg::ShaderOperand::inputAttribute(name));
        }

        template <typename T>
        T addOutput(const std::string& name)
        {
            if (outputLayout.checkElement(name))
            {
                throw std::runtime_error("Output already exists");
            }
            outputLayout.addElement(name, T::TYPE.getPrimitive());
            return T(rg::ShaderOperand::outputAttribute(name));
        }

        void addParameter(const std::string& name, const rg::ShaderPrimitiveType& type)
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

        void addBuffer(const std::string& name, const rg::ShaderBuffer& buffer)
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

        void addTextureArray(const std::string& name, const rg::ShaderTextureArray& textureArray)
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

        void setGeometryInput(rg::Primitive input)
        {
            if (stage != rg::Shader::GEOMETRY) throw std::runtime_error(
                "Attempted to set geometry input in non geometry stage");
            geometryInput = input;
        }

        void setGeometryOutput(rg::Primitive output, size_t maxVertices)
        {
            if (stage != rg::Shader::GEOMETRY) throw std::runtime_error(
                "Attempted to set geometry output in non geometry stage");
            geometryOutput = output;
            geometryMaxVertices = maxVertices;
        }

        rg::Shader build()
        {
            rg::ShaderFunction main;
            std::vector<rg::ShaderFunction> funcs;
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
        rg::Shader::Stage stage;
        rg::Primitive geometryInput{}; // The input rg::Primitive for geometry shaders. Must match the pipeline rg::Primitive.
        rg::Primitive geometryOutput{}; // The output rg::Primitive for geometry shaders
        size_t geometryMaxVertices{}; // The maximum number of output vertices for geometry shaders.

        rg::ShaderAttributeLayout inputLayout;
        rg::ShaderAttributeLayout outputLayout;

        std::unordered_map<std::string, rg::ShaderPrimitiveType> parameters{};
        std::unordered_map<std::string, rg::ShaderBuffer> buffers{};
        std::unordered_map<std::string, rg::ShaderTextureArray> textureArrays{};

        std::vector<rg::ShaderStructDef> typeDefinitions{};

        std::vector<rg::ShaderFunction> functions{};

        XENGINE_EXPORT static ShaderScope*& getCurrent();
    };
}

#endif //XENGINE_SHADERSCOPE_HPP
