/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2023  Julian Zampiccoli
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

#include "xng/shader/shadercompiler.hpp"
#include "xng/shader/spirvbundle.hpp"
#include "xng/shader/shaderdirectoryinclude.hpp"

#include "xng/driver/shaderc/shaderccompiler.hpp"

#include "xng/io/readfile.hpp"
#include "xng/io/writefile.hpp"

#include <fstream>
#include <filesystem>
#include <iostream>

enum OutputMode {
    OUTPUT_BINARY, // Output the shader data as binary
    OUTPUT_HEADER, // Output a c++ header which contains the shader data as a SPIRVBundle
};

struct Arguments {
    std::filesystem::path sourcePath{};
    std::filesystem::path outputPath{};
    std::filesystem::path includeDir{};
    std::string entryPoint{};
    xng::ShaderLanguage language{};
    xng::ShaderStage stage{};
    bool preprocess{};
    OutputMode mode{};
    xng::ShaderCompiler::OptimizationLevel optimization = xng::ShaderCompiler::OPTIMIZATION_NONE;
    bool forceOverwrite = false;
};

xng::ShaderLanguage getLanguage(const std::string &lang) {
    if (lang == "HLSL") {
        return xng::HLSL_SHADER_MODEL_4;
    } else if (lang == "GLSL_460") {
        return xng::GLSL_460;
    } else if (lang == "GLSL_460_VK") {
        return xng::GLSL_460_VK;
    } else if (lang == "GLSL_420") {
        return xng::GLSL_420;
    } else if (lang == "GLSL_420_VK") {
        return xng::GLSL_420_VK;
    } else if (lang == "GLSL_ES_320") {
        return xng::GLSL_ES_320;
    } else {
        throw std::runtime_error("Unrecognized shader language: " + lang);
    }
}

xng::ShaderStage getStage(const std::string &stage) {
    if (stage == "VERTEX") {
        return xng::VERTEX;
    } else if (stage == "TESSELLATION_CONTROL") {
        return xng::TESSELLATION_CONTROL;
    } else if (stage == "TESSELLATION_EVALUATION") {
        return xng::TESSELLATION_EVALUATION;
    } else if (stage == "GEOMETRY") {
        return xng::GEOMETRY;
    } else if (stage == "FRAGMENT") {
        return xng::FRAGMENT;
    } else if (stage == "COMPUTE") {
        return xng::COMPUTE;
    } else if (stage == "RAY_GENERATE") {
        return xng::RAY_GENERATE;
    } else if (stage == "RAY_HIT_ANY") {
        return xng::RAY_HIT_ANY;
    } else if (stage == "RAY_HIT_CLOSEST") {
        return xng::RAY_HIT_CLOSEST;
    } else if (stage == "RAY_MISS") {
        return xng::RAY_MISS;
    } else if (stage == "RAY_INTERSECT") {
        return xng::RAY_INTERSECT;
    } else {
        throw std::runtime_error("Unrecognized shader stage: " + stage);
    }
}

std::string getStage(xng::ShaderStage stage) {
    switch (stage) {
        default:
        case xng::VERTEX:
            return "VERTEX";
        case xng::TESSELLATION_CONTROL:
            return "TESSELLATION_CONTROL";
        case xng::TESSELLATION_EVALUATION:
            return "TESSELLATION_EVALUATION";
        case xng::GEOMETRY:
            return "GEOMETRY";
        case xng::FRAGMENT:
            return "FRAGMENT";
        case xng::COMPUTE:
            return "COMPUTE";
        case xng::RAY_GENERATE:
            return "RAY_GENERATE";
        case xng::RAY_HIT_ANY:
            return "RAY_HIT_ANY";
        case xng::RAY_HIT_CLOSEST:
            return "RAY_HIT_CLOSEST";
        case xng::RAY_MISS:
            return "RAY_MISS";
        case xng::RAY_INTERSECT:
            return "RAY_INTERSECT";
    }
}

xng::ShaderCompiler::OptimizationLevel getOptimizationLevel(const std::string &level) {
    if (level == "OPTIMIZATION_NONE") {
        return xng::ShaderCompiler::OPTIMIZATION_NONE;
    } else if (level == "OPTIMIZATION_PERFORMANCE") {
        return xng::ShaderCompiler::OPTIMIZATION_PERFORMANCE;
    } else if (level == "OPTIMIZATION_SIZE") {
        return xng::ShaderCompiler::OPTIMIZATION_SIZE;
    } else {
        throw std::runtime_error("Unrecognized optimization level: " + level);
    }
}

Arguments parseArgs(int argc, char *argv[]) {
    if (argc < 2) {
        throw std::runtime_error("No arguments specified");
    }

    Arguments ret;
    for (auto i = 1; i < argc; i++) {
        std::string arg(argv[i]);
        if (arg.starts_with('-')) {
            if (arg == "-i") {
                if (i + 1 >= argc) {
                    throw std::runtime_error("-I option must be followed by a valid path");
                } else {
                    ret.includeDir = argv[++i];
                }
            } else if (arg == "-p") {
                ret.preprocess = true;
            } else if (arg == "-l") {
                if (i + 1 >= argc) {
                    throw std::runtime_error("-l option must be followed by a valid shader language name");
                } else {
                    ret.language = getLanguage(std::string(argv[++i]));
                }
            } else if (arg == "-s") {
                if (i + 1 >= argc) {
                    throw std::runtime_error("-s option must be followed by a valid shader stage name");
                } else {
                    ret.stage = getStage(std::string(argv[++i]));
                }
            } else if (arg == "-o") {
                if (i + 1 >= argc) {
                    throw std::runtime_error("-o option must be followed by a valid optimization level name");
                } else {
                    ret.optimization = getOptimizationLevel(std::string(argv[++i]));
                }
            } else if (arg == "-e") {
                if (i + 1 >= argc) {
                    throw std::runtime_error("-e option must be followed by a valid entry point name");
                } else {
                    ret.entryPoint = std::string(argv[++i]);
                }
            } else if (arg == "-f") {
                ret.forceOverwrite = true;
            } else if (arg == "-m") {
                if (i + 1 >= argc) {
                    throw std::runtime_error("-m option must be followed by a valid mode name");
                } else {
                    auto str = std::string(argv[++i]);
                    if (str == "binary") {
                        ret.mode = OUTPUT_BINARY;
                    } else if (str == "header") {
                        ret.mode = OUTPUT_HEADER;
                    } else {
                        throw std::runtime_error("Unrecognized mode name: " + str);
                    }
                }
            } else {
                throw std::runtime_error("Unrecognized option: " + arg);
            }
        } else if (ret.sourcePath.empty()) {
            ret.sourcePath = arg;
        } else if (ret.outputPath.empty()) {
            ret.outputPath = arg;
        } else {
            throw std::runtime_error("Invalid number of paths: " + arg);
        }
    }
    return ret;
}

void printUsage() {
    std::cout << "Usage: shadercompiler [OPTION]... SOURCE DEST\n";
}

xng::SPIRVBlob blob = {1, 2, 3,};

std::string generateHeader(const std::string &filePath, const std::string &filename, const xng::SPIRVShader &shader) {
    std::string ret = "#ifndef " + filePath + "\n#define " + filePath + R"###(
#include "xng/shader/spirvbundle.hpp"
using namespace xng;
const SPIRVBundle )###";

    ret += filename;

    ret += " = SPIRVBundle(std::vector<SPIRVBundle::Entry>{SPIRVBundle::Entry{.stage = ";

    ret += getStage(shader.getStage());

    ret += ", .entryPoint = \"";

    ret += shader.getEntryPoint();

    ret += "\", .blobIndex = 0}}, { SPIRVBlob{";
    for (auto &v: shader.getBlob()) {
        ret += std::to_string(v) + ",";
    }
    ret += R"###(} });
#endif)###";

    return ret;
}

int main(int argc, char *argv[]) {
    Arguments args;
    try {
        args = parseArgs(argc, argv);
    } catch (const std::exception &e) {
        std::cout << "Failed to parse arguments: " << e.what() << "\n";
        printUsage();
        return 0;
    }

    if (!std::filesystem::exists(args.sourcePath)) {
        std::cout << args.sourcePath.string() + ": No such file or directory\n";
        return 0;
    } else if (std::filesystem::is_directory(args.sourcePath)) {
        std::cout << args.sourcePath.string() + ": Is a directory\n";
        return 0;
    }

    if (std::filesystem::exists(args.outputPath) && !args.forceOverwrite) {
        char inValue = 0;
        while (!std::cin.fail() && inValue != 'y' && inValue != 'n') {
            std::cout << args.outputPath.string() +
                         ": File already exists, do you want to overwrite the existing file? y/n: ";
            std::cin >> inValue;
            std::cout << "\n";
        }
        if (inValue == 'n') {
            std::cout << "Aborting";
            return 0;
        }
    }

    auto shader = xng::readFileString(args.sourcePath.string());

    auto compiler = xng::shaderc::ShaderCCompiler();

    if (args.preprocess) {
        if (args.includeDir.empty()) {
            shader = compiler.preprocess(shader, args.stage, args.language, {}, {}, args.optimization);
        } else {
            shader = compiler.preprocess(shader,
                                         args.stage,
                                         args.language,
                                         xng::ShaderDirectoryInclude::getShaderIncludeCallback(args.includeDir),
                                         {},
                                         args.optimization);
        }
    }

    auto bin = compiler.compile(shader, args.entryPoint, args.stage, args.language, args.optimization);

    if (args.outputPath.has_relative_path()
        && args.outputPath.has_parent_path()
        && !std::filesystem::is_directory(args.outputPath.parent_path())) {
        try {
            std::filesystem::remove(args.outputPath.parent_path());
            std::filesystem::create_directories(args.outputPath.parent_path());
        } catch (const std::exception &e) {
            std::cout << "Failed to create directory at " + args.outputPath.string() + "\n";
            std::cout << e.what();
            std::cout << "\n";
            return 1;
        }
    }

    switch (args.mode) {
        default:
        case OUTPUT_BINARY:
            try {
                xng::writeFile(args.outputPath, bin);
            } catch (const std::exception &e) {
                std::cout << "Failed to write file at: " + args.outputPath.string() + "\n";
                std::cout << e.what();
                std::cout << "\n";
                return 1;
            }
            break;
        case OUTPUT_HEADER:
            xng::SPIRVBundle bundle(std::vector<xng::SPIRVBundle::Entry>{xng::SPIRVBundle::Entry{
                                            .stage = args.stage,
                                            .entryPoint = args.entryPoint,
                                            .blobIndex = 0}},
                                    {bin});
            auto guard = args.outputPath.relative_path().string();
            guard = std::string(guard.begin(),
                                guard.begin() + static_cast<std::string::difference_type>(guard.size() -
                                                                                          args.outputPath.relative_path().extension().string().size()));
            std::replace(guard.begin(), guard.end(), '/', '_');

            try {
                xng::writeFile(args.outputPath,
                               generateHeader(guard, args.outputPath.stem().string(), bundle.getShader()));
            } catch (const std::exception &e) {
                std::cout << "Failed to write file at: " + args.outputPath.string() + "\n";
                std::cout << e.what();
                std::cout << "\n";
                return 1;
            }
            break;
    }

    return 0;
}
