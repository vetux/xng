/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2023  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 3 of the License, or (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.

 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "shadercompilerglsl.hpp"

#include "xng/render/graph2/shader/fgshadernode.hpp"
#include "xng/util/downcast.hpp"

using namespace xng;

static std::string debugNodeTypeName(FGShaderNode::NodeType type) {
    switch (type) {
        case FGShaderNode::LITERAL:
            return "LITERAL";
        case FGShaderNode::VECTOR:
            return "VECTOR";
        case FGShaderNode::ATTRIBUTE_READ:
            return "ATTRIBUTE_READ";
        case FGShaderNode::ATTRIBUTE_WRITE:
            return "ATTRIBUTE_WRITE";
        case FGShaderNode::PARAMETER_READ:
            return "PARAMETER_READ";
        case FGShaderNode::TEXTURE_SAMPLE:
            return "TEXTURE_SAMPLE";
        case FGShaderNode::TEXTURE_SIZE:
            return "TEXTURE_SIZE";
        case FGShaderNode::BUFFER_READ:
            return "BUFFER_READ";
        case FGShaderNode::BUFFER_WRITE:
            return "BUFFER_WRITE";
        case FGShaderNode::ADD:
            return "ADD";
        case FGShaderNode::SUBTRACT:
            return "SUBTRACT";
        case FGShaderNode::MULTIPLY:
            return "MULTIPLY";
        case FGShaderNode::DIVIDE:
            return "DIVIDE";
        case FGShaderNode::EQUAL:
            return "EQUAL";
        case FGShaderNode::NEQUAL:
            return "NEQUAL";
        case FGShaderNode::GREATER:
            return "GREATER";
        case FGShaderNode::LESS:
            return "LESS";
        case FGShaderNode::GREATER_EQUAL:
            return "GREATER_EQUAL";
        case FGShaderNode::LESS_EQUAL:
            return "LESS_EQUAL";
        case FGShaderNode::AND:
            return "AND";
        case FGShaderNode::OR:
            return "OR";
        case FGShaderNode::NORMALIZE:
            return "NORMALIZE";
        case FGShaderNode::ARRAY_LENGTH:
            return "ARRAY_LENGTH";
        case FGShaderNode::SUBSCRIPT:
            return "SUBSCRIPT";
        case FGShaderNode::BRANCH:
            return "BRANCH";
        case FGShaderNode::LOOP:
            return "LOOP";
        default:
            return "UNKNOWN";
    }
}

static std::string debugTraverseTreePrefix(size_t depth = 0) {
    std::string ret;
    if (depth > 0) {
        for (size_t i = 0; i <= depth; i++) {
            ret += " ";
        }
        ret += "|-";
    }
    return ret;
}

static std::string debugTraverseTree(FGShaderNode &node, size_t depth = 0) {
    std::string ret = debugTraverseTreePrefix(depth) + debugNodeTypeName(node.getType()) + "\n";

    for (auto input: node.getInputs()) {
        ret += debugTraverseTreePrefix(depth + 2) + input.get().name + "\n";
        if (input.get().source != nullptr) {
            ret += debugTraverseTree(*input.get().source, depth + 4);
        }
    }
    return ret;
}

static std::string debugNodeTree(const std::vector<std::shared_ptr<FGShaderNode> > nodes) {
    std::vector<std::shared_ptr<FGShaderNode> > baseNodes;
    for (auto node: nodes) {
        if (node->getOutputs().empty()) {
            baseNodes.emplace_back(node);
        }
    }

    std::string ret;
    for (auto node: baseNodes) {
        ret = ret + debugTraverseTree(*node);
        ret += "\n";
    }
    return ret;
}

static std::string getTypeName(const FGShaderValue &value) {
    std::string ret;
    switch (value.type) {
        case FGShaderValue::SCALAR:
            switch (value.component) {
                case FGShaderValue::BOOLEAN:
                    return "bool";
                case FGShaderValue::UNSIGNED_INT:
                    return "uint";
                case FGShaderValue::SIGNED_INT:
                    return "int";
                case FGShaderValue::FLOAT:
                    return "float";
                case FGShaderValue::DOUBLE:
                    return "double";
                default:
                    throw std::runtime_error("Invalid component");
            }
        case FGShaderValue::VECTOR2:
            switch (value.component) {
                case FGShaderValue::BOOLEAN:
                    return "bvec2";
                case FGShaderValue::UNSIGNED_INT:
                    return "uvec2";
                case FGShaderValue::SIGNED_INT:
                    return "ivec2";
                case FGShaderValue::FLOAT:
                    return "vec2";
                case FGShaderValue::DOUBLE:
                    return "dvec2";
                default:
                    throw std::runtime_error("Invalid component");
            }
        case FGShaderValue::VECTOR3:
            switch (value.component) {
                case FGShaderValue::BOOLEAN:
                    return "bvec3";
                case FGShaderValue::UNSIGNED_INT:
                    return "uvec3";
                case FGShaderValue::SIGNED_INT:
                    return "ivec3";
                case FGShaderValue::FLOAT:
                    return "vec3";
                case FGShaderValue::DOUBLE:
                    return "dvec3";
                default:
                    throw std::runtime_error("Invalid component");
            }
        case FGShaderValue::VECTOR4:
            switch (value.component) {
                case FGShaderValue::BOOLEAN:
                    return "bvec4";
                case FGShaderValue::UNSIGNED_INT:
                    return "uvec4";
                case FGShaderValue::SIGNED_INT:
                    return "ivec4";
                case FGShaderValue::FLOAT:
                    return "vec4";
                case FGShaderValue::DOUBLE:
                    return "dvec4";
                default:
                    throw std::runtime_error("Invalid component");
            }
        case FGShaderValue::MAT2:
            switch (value.component) {
                case FGShaderValue::FLOAT:
                    return "mat2";
                case FGShaderValue::DOUBLE:
                    return "dmat2";
                default:
                    throw std::runtime_error("Invalid component");
            }
        case FGShaderValue::MAT3:
            switch (value.component) {
                case FGShaderValue::FLOAT:
                    return "mat3";
                case FGShaderValue::DOUBLE:
                    return "dmat3";
                default:
                    throw std::runtime_error("Invalid component");
            }
        case FGShaderValue::MAT4:
            switch (value.component) {
                case FGShaderValue::FLOAT:
                    return "mat4";
                case FGShaderValue::DOUBLE:
                    return "dmat4";
                default:
                    throw std::runtime_error("Invalid component");
            }
        default:
            throw std::runtime_error("Invalid type");
    }
}

static std::string getVariablePrefix(const FGShaderSource &source, FGShaderNode &base, size_t &varCount) {
    return getTypeName(base.getOutputType(source))
           + " tmp"
           + std::to_string(varCount++);
}

static std::string literalToString(const FGShaderLiteral &value) {
    auto type = getLiteralType(value);

    switch (type.type) {
        case FGShaderValue::SCALAR:
            switch (type.component) {
                case FGShaderValue::BOOLEAN:
                    return std::to_string(std::get<bool>(value));
                case FGShaderValue::UNSIGNED_INT:
                    return std::to_string(std::get<unsigned int>(value));
                case FGShaderValue::SIGNED_INT:
                    return std::to_string(std::get<int>(value));
                case FGShaderValue::FLOAT:
                    return std::to_string(std::get<float>(value));
                case FGShaderValue::DOUBLE:
                    return std::to_string(std::get<double>(value));
            }
            break;
        case FGShaderValue::VECTOR2:
            switch (type.component) {
                case FGShaderValue::BOOLEAN: {
                    auto vec = std::get<Vector2<bool> >(value);
                    return "bvec2(" + std::to_string(vec.x) + ", " + std::to_string(vec.y) + ")";
                }
                case FGShaderValue::UNSIGNED_INT: {
                    auto vecu = std::get<Vector2<unsigned int> >(value);
                    return "uvec2(" + std::to_string(vecu.x) + ", " + std::to_string(vecu.y) + ")";
                }
                case FGShaderValue::SIGNED_INT: {
                    auto veci = std::get<Vector2<int> >(value);
                    return "ivec2(" + std::to_string(veci.x) + ", " + std::to_string(veci.y) + ")";
                }
                case FGShaderValue::FLOAT: {
                    auto vecf = std::get<Vector2<float> >(value);
                    return "vec2(" + std::to_string(vecf.x) + ", " + std::to_string(vecf.y) + ")";
                }
                case FGShaderValue::DOUBLE: {
                    auto vecd = std::get<Vector2<double> >(value);
                    return "dvec2(" + std::to_string(vecd.x) + ", " + std::to_string(vecd.y) + ")";
                }
            }
        case FGShaderValue::VECTOR3:
            switch (type.component) {
                case FGShaderValue::BOOLEAN: {
                    auto vec = std::get<Vector3<bool> >(value);
                    return "bvec3(" + std::to_string(vec.x) + ", " + std::to_string(vec.y) + ", " +
                           std::to_string(vec.z) + ")";
                }
                case FGShaderValue::UNSIGNED_INT: {
                    auto vecu = std::get<Vector3<unsigned int> >(value);
                    return "uvec3(" + std::to_string(vecu.x) + ", " + std::to_string(vecu.y) + ", " +
                           std::to_string(vecu.z) + ")";
                }
                case FGShaderValue::SIGNED_INT: {
                    auto veci = std::get<Vector3<int> >(value);
                    return "ivec3(" + std::to_string(veci.x) + ", " + std::to_string(veci.y) + ", " +
                           std::to_string(veci.z) + ")";
                }
                case FGShaderValue::FLOAT: {
                    auto vecf = std::get<Vector3<float> >(value);
                    return "vec3(" + std::to_string(vecf.x) + ", " + std::to_string(vecf.y) + ", " +
                           std::to_string(vecf.z) + ")";
                }
                case FGShaderValue::DOUBLE: {
                    auto vecd = std::get<Vector3<double> >(value);
                    return "dvec3(" + std::to_string(vecd.x) + ", " + std::to_string(vecd.y) + ", " +
                           std::to_string(vecd.z) + ")";
                }
            }
        case FGShaderValue::VECTOR4:
            switch (type.component) {
                case FGShaderValue::BOOLEAN: {
                    auto vec = std::get<Vector4<bool> >(value);
                    return "bvec4(" + std::to_string(vec.x) + ", " + std::to_string(vec.y) + ", " +
                           std::to_string(vec.z) + ", " + std::to_string(vec.w) + ")";
                }
                case FGShaderValue::UNSIGNED_INT: {
                    auto vecu = std::get<Vector4<unsigned int> >(value);
                    return "uvec4(" + std::to_string(vecu.x) + ", " + std::to_string(vecu.y) + ", " +
                           std::to_string(vecu.z) + ", " + std::to_string(vecu.w) + ")";
                }
                case FGShaderValue::SIGNED_INT: {
                    auto veci = std::get<Vector4<int> >(value);
                    return "ivec4(" + std::to_string(veci.x) + ", " + std::to_string(veci.y) + ", " +
                           std::to_string(veci.z) + ", " + std::to_string(veci.w) + ")";
                }
                case FGShaderValue::FLOAT: {
                    auto vecf = std::get<Vector4<float> >(value);
                    return "vec4(" + std::to_string(vecf.x) + ", " + std::to_string(vecf.y) + ", " +
                           std::to_string(vecf.z) + ", " + std::to_string(vecf.w) + ")";
                }
                case FGShaderValue::DOUBLE: {
                    auto vecd = std::get<Vector4<double> >(value);
                    return "dvec4(" + std::to_string(vecd.x) + ", " + std::to_string(vecd.y) + ", " +
                           std::to_string(vecd.z) + ", " + std::to_string(vecd.w) + ")";
                }
            }
        case FGShaderValue::MAT2:
            switch (type.component) {
                case FGShaderValue::FLOAT: {
                    auto mat2 = std::get<Matrix<float, 2, 2> >(value);
                    return "mat2(" + std::to_string(mat2.get(0, 0)) + ", " + std::to_string(mat2.get(0, 1)) + ", " +
                           std::to_string(mat2.get(1, 0)) + ", " + std::to_string(mat2.get(1, 1)) + ")";
                }
                case FGShaderValue::DOUBLE: {
                    auto mat2d = std::get<Matrix<double, 2, 2> >(value);
                    return "dmat2(" + std::to_string(mat2d.get(0, 0)) + ", " + std::to_string(mat2d.get(0, 1)) + ", " +
                           std::to_string(mat2d.get(1, 0)) + ", " + std::to_string(mat2d.get(1, 1)) + ")";
                }
                default:
                    throw std::runtime_error("Invalid matrix component type");
            }
        case FGShaderValue::MAT3:
            switch (type.component) {
                case FGShaderValue::FLOAT: {
                    auto mat3 = std::get<Matrix<float, 3, 3> >(value);
                    return "mat3(" + std::to_string(mat3.get(0, 0)) + ", " + std::to_string(mat3.get(0, 1)) + ", " +
                           std::to_string(mat3.get(0, 2)) + ", " + std::to_string(mat3.get(1, 0)) + ", " +
                           std::to_string(mat3.get(1, 1)) + ", " + std::to_string(mat3.get(1, 2)) + ", " +
                           std::to_string(mat3.get(2, 0)) + ", " + std::to_string(mat3.get(2, 1)) + ", " +
                           std::to_string(mat3.get(2, 2)) + ")";
                }
                case FGShaderValue::DOUBLE: {
                    auto mat3d = std::get<Matrix<double, 3, 3> >(value);
                    return "dmat3(" + std::to_string(mat3d.get(0, 0)) + ", " + std::to_string(mat3d.get(0, 1)) + ", " +
                           std::to_string(mat3d.get(0, 2)) + ", " + std::to_string(mat3d.get(1, 0)) + ", " +
                           std::to_string(mat3d.get(1, 1)) + ", " + std::to_string(mat3d.get(1, 2)) + ", " +
                           std::to_string(mat3d.get(2, 0)) + ", " + std::to_string(mat3d.get(2, 1)) + ", " +
                           std::to_string(mat3d.get(2, 2)) + ")";
                }
                default:
                    throw std::runtime_error("Invalid matrix component type");
            }
        case FGShaderValue::MAT4:
            switch (type.component) {
                case FGShaderValue::FLOAT: {
                    auto mat4 = std::get<Matrix<float, 4, 4> >(value);
                    return "mat4(" + std::to_string(mat4.get(0, 0)) + ", " + std::to_string(mat4.get(0, 1)) + ", " +
                           std::to_string(mat4.get(0, 2)) + ", " + std::to_string(mat4.get(0, 3)) + ", " +
                           std::to_string(mat4.get(1, 0)) + ", " + std::to_string(mat4.get(1, 1)) + ", " +
                           std::to_string(mat4.get(1, 2)) + ", " + std::to_string(mat4.get(1, 3)) + ", " +
                           std::to_string(mat4.get(2, 0)) + ", " + std::to_string(mat4.get(2, 1)) + ", " +
                           std::to_string(mat4.get(2, 2)) + ", " + std::to_string(mat4.get(2, 3)) + ", " +
                           std::to_string(mat4.get(3, 0)) + ", " + std::to_string(mat4.get(3, 1)) + ", " +
                           std::to_string(mat4.get(3, 2)) + ", " + std::to_string(mat4.get(3, 3)) + ")";
                }
                case FGShaderValue::DOUBLE: {
                    auto mat4d = std::get<Matrix<double, 4, 4> >(value);
                    return "dmat4(" + std::to_string(mat4d.get(0, 0)) + ", " + std::to_string(mat4d.get(0, 1)) + ", " +
                           std::to_string(mat4d.get(0, 2)) + ", " + std::to_string(mat4d.get(0, 3)) + ", " +
                           std::to_string(mat4d.get(1, 0)) + ", " + std::to_string(mat4d.get(1, 1)) + ", " +
                           std::to_string(mat4d.get(1, 2)) + ", " + std::to_string(mat4d.get(1, 3)) + ", " +
                           std::to_string(mat4d.get(2, 0)) + ", " + std::to_string(mat4d.get(2, 1)) + ", " +
                           std::to_string(mat4d.get(2, 2)) + ", " + std::to_string(mat4d.get(2, 3)) + ", " +
                           std::to_string(mat4d.get(3, 0)) + ", " + std::to_string(mat4d.get(3, 1)) + ", " +
                           std::to_string(mat4d.get(3, 2)) + ", " + std::to_string(mat4d.get(3, 3)) + ")";
                }
                default:
                    throw std::runtime_error("Invalid matrix component type");
            }
    }
}

static std::string compileNode(FGNodeAttributeRead &base) {
    return "inputAttribute" + std::to_string(base.attributeIndex);
}

static std::string compileNode(FGNodeAttributeWrite &base, const std::string &inputName) {
    return "outputAttribute" + std::to_string(base.attributeIndex) + " = " + inputName;
}

static std::string compileNode(FGNodeBufferRead &base) {
    return "buffer_" + base.bufferName + "." + base.elementName;
}

static std::string compileNode(FGNodeAdd &base,
                               const std::vector<std::string> &inputNames) {
    return inputNames.at(0) + " + " + inputNames.at(1);
}

static std::string compileNode(FGNodeMultiply &base,
                               const std::vector<std::string> &inputNames) {
    return inputNames.at(0) + " * " + inputNames.at(1);
}

static std::string compileNode(FGNodeSubscript &base,
                               const std::vector<std::string> &inputNames,
                               const FGShaderSource &source) {
    std::string assignment;
    auto valueType = base.value.source->getOutputType(source);
    if (valueType.count > 1) {
        // Array subscripting
        assignment = inputNames.at(0) + "[" + inputNames.at(1) + "]";
    } else if (valueType.type == FGShaderValue::MAT2
               || valueType.type == FGShaderValue::MAT3
               || valueType.type == FGShaderValue::MAT4) {
        // Matrix subscripting
        assignment = inputNames.at(0) + "[" + inputNames.at(1) + "][" + inputNames.at(2) + "]";
    } else {
        // Vector subscripting
        auto indexSource = base.row.source;
        if (indexSource->getType() != FGShaderNode::LITERAL) {
            throw std::runtime_error("Subscripting vectors with non-literal index is not supported");
        }
        auto index = std::get<int>(down_cast<FGNodeLiteral &>(*indexSource).value);
        switch (index) {
            case 0:
                assignment = inputNames.at(0) + ".x";
                break;
            case 1:
                assignment = inputNames.at(0) + ".y";
                break;
            case 2:
                assignment = inputNames.at(0) + ".z";
                break;
            case 3:
                assignment = inputNames.at(0) + ".w";
                break;
            default:
                throw std::runtime_error("Invalid vector subscript index");
        }
    }
    return assignment;
}

static std::string compileNode(FGNodeLiteral &base) {
    return literalToString(base.value);
}

static std::string compileNode(FGNodeVector &base,
                               const std::vector<std::string> &inputNames,
                               const FGShaderSource &source) {
    std::string ret;
    if (inputNames.size() == 1) {
        ret = inputNames.at(0);
    } else if (inputNames.size() == 2) {
        ret = "(" + inputNames.at(0) + ", " + inputNames.at(1) + ")";
    } else if (inputNames.size() == 3) {
        ret = "(" + inputNames.at(0) + ", " + inputNames.at(1) + ", " + inputNames.at(2) + ")";
    } else if (inputNames.size() == 4) {
        ret = "(" + inputNames.at(0) + ", " + inputNames.at(1) + ", " + inputNames.at(2) + ", " + inputNames.at(3) +
              ")";
    } else {
        throw std::runtime_error("Invalid number of inputs");
    }
    return getTypeName(base.getOutputType(source)) + ret;
}

std::string ShaderCompilerGLSL::compile(const FGShaderSource &source) {
    std::vector<std::shared_ptr<FGShaderNode> > baseNodes;
    for (const auto &node: source.nodes) {
        if (node->getOutputs().empty()) {
            baseNodes.emplace_back(node);
        }
    }
    std::string ret;
    size_t varCount = 0;
    for (auto &base: baseNodes) {
        ret += compile(source, *base, varCount);
    }
    return ret;
}

std::string ShaderCompilerGLSL::compile(const FGShaderSource &source, FGShaderNode &base, size_t &varCount) {
    std::string ret;
    std::vector<std::string> inputNames;
    for (auto &input: base.getInputs()) {
        if (input.get().source != nullptr) {
            ret += compile(source, *input.get().source, varCount);
            inputNames.emplace_back("tmp" + std::to_string(varCount - 1));
        }
    }
    switch (base.getType()) {
        case FGShaderNode::LITERAL:
            ret += getVariablePrefix(source, base, varCount)
                    + " = "
                    + compileNode(down_cast<FGNodeLiteral &>(base))
                    + "\n";
            break;
        case FGShaderNode::VECTOR:
            ret += getVariablePrefix(source, base, varCount)
                    + " = "
                    + compileNode(down_cast<FGNodeVector &>(base), inputNames, source)
                    + "\n";
            break;
        case FGShaderNode::ATTRIBUTE_READ:
            ret += getVariablePrefix(source, base, varCount)
                    + " = "
                    + compileNode(down_cast<FGNodeAttributeRead &>(base))
                    + "\n";
            break;
        case FGShaderNode::ATTRIBUTE_WRITE:
            ret += compileNode(down_cast<FGNodeAttributeWrite &>(base), inputNames.at(0))
                    + "\n";
            break;
        case FGShaderNode::PARAMETER_READ:
            break;
        case FGShaderNode::TEXTURE_SAMPLE:
            break;
        case FGShaderNode::TEXTURE_SIZE:
            break;
        case FGShaderNode::BUFFER_READ:
            ret += getVariablePrefix(source, base, varCount)
                    + " = "
                    + compileNode(down_cast<FGNodeBufferRead &>(base))
                    + "\n";
            break;
        case FGShaderNode::BUFFER_WRITE:
            break;
        case FGShaderNode::ADD:
            ret += getVariablePrefix(source, base, varCount)
                    + " = "
                    + compileNode(down_cast<FGNodeAdd &>(base), inputNames)
                    + "\n";
            break;
        case FGShaderNode::SUBTRACT:
            break;
        case FGShaderNode::MULTIPLY:
            ret += getVariablePrefix(source, base, varCount)
                    + " = "
                    + compileNode(down_cast<FGNodeMultiply &>(base), inputNames)
                    + "\n";
            break;
        case FGShaderNode::DIVIDE:
            break;
        case FGShaderNode::EQUAL:
            break;
        case FGShaderNode::NEQUAL:
            break;
        case FGShaderNode::GREATER:
            break;
        case FGShaderNode::LESS:
            break;
        case FGShaderNode::GREATER_EQUAL:
            break;
        case FGShaderNode::LESS_EQUAL:
            break;
        case FGShaderNode::AND:
            break;
        case FGShaderNode::OR:
            break;
        case FGShaderNode::NORMALIZE:
            break;
        case FGShaderNode::ARRAY_LENGTH:
            break;
        case FGShaderNode::SUBSCRIPT:
            ret += getVariablePrefix(source, base, varCount)
                    + " = "
                    + compileNode(down_cast<FGNodeSubscript &>(base), inputNames, source)
                    + "\n";
            break;
        case FGShaderNode::BRANCH:
            break;
        case FGShaderNode::LOOP:
            break;
    }
    return ret;
}
