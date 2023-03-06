/**
 *  This file is part of xEngine, a C++ game engine library.
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

#include "xng/xng.hpp"

#include "testpass.hpp"

static const char *SPHERE_MATERIAL_PATH = "memory://tests/graph/spherematerial";
static const char *CUBE_MATERIAL_PATH = "memory://tests/graph/cubematerial";

void createMaterialResource(xng::MemoryArchive &archive) {
    // Sphere
    xng::Material material;
    material.shadingModel = xng::SHADE_PHONG;
    material.diffuse = ColorRGBA::blue(0.8);
    material.normal = ResourceHandle<Texture>(Uri("assets/sphere_normals.png"));

    xng::ResourceBundle bundle;
    bundle.add("material", std::make_unique<xng::Material>(material));

    auto msg = xng::JsonParser::createBundle(bundle);

    std::stringstream stream;
    xng::JsonProtocol().serialize(stream, msg);

    std::vector<uint8_t> vec;
    for (auto &c: stream.str()) {
        vec.emplace_back(c);
    }

    archive.addData(SPHERE_MATERIAL_PATH, vec);

    // Cube
    material = {};
    material.shadingModel = xng::SHADE_PHONG;
    material.diffuse = ColorRGBA::white(0.7);

    bundle = {};
    bundle.add("material", std::make_unique<xng::Material>(material));

    msg = xng::JsonParser::createBundle(bundle);

    stream = {};
    xng::JsonProtocol().serialize(stream, msg);

    vec.clear();
    for (auto &c: stream.str()) {
        vec.emplace_back(c);
    }

    archive.addData(CUBE_MATERIAL_PATH, vec);

}

void printUsage() {
    std::cout
            << "Usage: test-framegraph [DIRECTORY]\nExecute the frame graph test using the specified directory path to retrieve assets.";
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printUsage();
    }

    std::string assetDirectory = argv[2];

    auto &archive = dynamic_cast<xng::MemoryArchive &>(xng::ResourceRegistry::getDefaultRegistry().getArchive(
            "memory"));

    createMaterialResource(archive);

    auto displayDriver = xng::DisplayDriver::load(xng::GLFW);
    auto gpuDriver = xng::GpuDriver::load(xng::OPENGL_4_6);

    auto shaderCompiler = xng::ShaderCompiler::load(xng::SHADERC);
    auto shaderDecompiler = xng::ShaderDecompiler::load(xng::SPIRV_CROSS);

    auto device = gpuDriver->createRenderDevice();
    auto window = displayDriver->createWindow("opengl");
    auto &input = window->getInput();

    xng::FrameGraphRenderer renderer(window->getRenderTarget(),
                                     std::make_unique<xng::FrameGraphPoolAllocator>(*device,
                                                                                    *shaderCompiler,
                                                                                    *shaderDecompiler));

    renderer.setPasses({
                               std::make_shared<GBufferPass>(),
                               std::make_shared<TestPass>(),
                               std::make_shared<CompositePass>()
                       });

    xng::Light light;
    light.type = xng::LIGHT_POINT;
    light.transform.setPosition({0, 0, -10});

    xng::Scene::Object sphere;
    sphere.transform.setPosition({5, 0, -10});
    sphere.mesh = xng::ResourceHandle<xng::Mesh>(xng::Uri("meshes/sphere.obj"));
    sphere.material = xng::ResourceHandle<xng::Material>(xng::Uri(SPHERE_MATERIAL_PATH));

    xng::Scene::Object cube;
    cube.transform.setPosition({-5, 0, -10});
    cube.mesh = xng::ResourceHandle<xng::Mesh>(xng::Uri("meshes/cube.obj"));
    cube.material = xng::ResourceHandle<xng::Material>(xng::Uri(CUBE_MATERIAL_PATH));

    xng::Scene scene;
    scene.camera.type = xng::PERSPECTIVE;

    scene.objects.emplace_back(sphere);
    scene.objects.emplace_back(cube);

    scene.lights.emplace_back(light);

    auto &lightRef = scene.lights.at(0);

    bool lightDirection = false;
    auto lightSpeed = 10.0f;

    xng::FrameLimiter limiter;
    limiter.reset();
    while (!window->shouldClose()) {
        auto deltaTime = limiter.newFrame();

        scene.camera.aspectRatio = static_cast<float>(window->getWindowSize().x)
                                   / static_cast<float>(window->getWindowSize().y);

        auto lightPos = lightRef.transform.getPosition();

        if (lightDirection) {
            if (lightPos.y > 10)
                lightDirection = !lightDirection;
            else
                lightRef.transform.setPosition({0, lightPos.y + lightSpeed * deltaTime, -10});
        } else {
            if (lightPos.y < -10)
                lightDirection = !lightDirection;
            else
                lightRef.transform.setPosition({0, lightPos.y - lightSpeed * deltaTime, -10});
        }

        renderer.render(scene);
        window->update();
    }

    return 0;
}