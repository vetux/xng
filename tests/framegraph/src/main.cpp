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

#include <memory>

#include "xng/xng.hpp"

#include "testpass.hpp"

static const char *SPHERE_MATERIAL_PATH = "memory://tests/graph/spherematerial.json";
static const char *CUBE_MATERIAL_PATH = "memory://tests/graph/cubematerial.json";

void createMaterialResource(xng::MemoryArchive &archive) {
    // Sphere
    xng::Material material;
    material.shadingModel = xng::SHADE_PHONG;
    material.diffuse = ColorRGBA::blue(0.8);
    material.normal = ResourceHandle<Texture>(Uri("textures/sphere_normals.json"));

    xng::ResourceBundle bundle;
    bundle.add("material", std::make_unique<xng::Material>(material));

    auto msg = xng::JsonParser::createBundle(bundle);

    std::stringstream stream;
    xng::JsonProtocol().serialize(stream, msg);

    std::vector<uint8_t> vec;
    for (auto &c: stream.str()) {
        vec.emplace_back(c);
    }

    Uri uri(SPHERE_MATERIAL_PATH);

    archive.addData(uri.toString(false), vec);

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

    uri = Uri(CUBE_MATERIAL_PATH);
    archive.addData(uri.toString(false), vec);

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

    std::vector<std::unique_ptr<ResourceParser>> parsers;
    parsers.emplace_back(std::make_unique<StbiParser>());
    parsers.emplace_back(std::make_unique<AssImpParser>());
    parsers.emplace_back(std::make_unique<JsonParser>());

    xng::ResourceRegistry::getDefaultRegistry().setImporter(
            ResourceImporter(std::move(parsers)));

    xng::ResourceRegistry::getDefaultRegistry().addArchive("file", std::make_shared<DirectoryArchive>("assets/"));

    auto &archive = dynamic_cast<xng::MemoryArchive &>(xng::ResourceRegistry::getDefaultRegistry().getArchive(
            "memory"));

    createMaterialResource(archive);

    auto displayDriver = glfw::GLFWDisplayDriver();
    auto gpuDriver = opengl::OGLGpuDriver();
    auto shaderCompiler = shaderc::ShaderCCompiler();
    auto shaderDecompiler = spirv_cross::SpirvCrossDecompiler();

    auto window = displayDriver.createWindow(OPENGL_4_6);
    auto &input = window->getInput();

    window->bindGraphics();

    auto device = gpuDriver.createRenderDevice();
    xng::FrameGraphRenderer renderer(window->getRenderTarget(),
                                     std::make_unique<xng::FrameGraphPoolAllocator>(*device,
                                                                                    shaderCompiler,
                                                                                    shaderDecompiler,
                                                                                    window->getRenderTarget()),
                                     shaderCompiler,
                                     shaderDecompiler);

    auto testPass = std::make_shared<TestPass>();
    renderer.setPasses({
                               std::make_shared<GBufferPass>(),
                               testPass,
                       });

    xng::Light light;
    light.type = xng::LIGHT_POINT;
    light.transform.setPosition({0, 0, -10});

    xng::Scene::Object sphere;
    sphere.transform.setPosition({0, 0, 10});
    sphere.mesh = xng::ResourceHandle<xng::Mesh>(xng::Uri("meshes/sphere.obj/Sphere"));
    sphere.material = xng::ResourceHandle<xng::Material>(xng::Uri(SPHERE_MATERIAL_PATH));

    xng::Scene::Object cube;
    cube.transform.setPosition({-0, 0, -10});
    cube.mesh = xng::ResourceHandle<xng::Mesh>(xng::Uri("meshes/cube.obj/Cube"));
    cube.material = xng::ResourceHandle<xng::Material>(xng::Uri(CUBE_MATERIAL_PATH));

    xng::Scene scene;
    scene.camera.type = xng::PERSPECTIVE;

    scene.objects.emplace_back(sphere);
    scene.objects.emplace_back(cube);

    scene.lights.emplace_back(light);

    auto &lightRef = scene.lights.at(0);

    bool lightDirection = false;
    auto lightSpeed = 10.0f;

    testPass->setTex(1);

    xng::FrameLimiter limiter(60);
    limiter.reset();
    while (!window->shouldClose()) {
        window->update();

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

        if (window->getInput().getKeyboard().getKeyDown(xng::KEY_LEFT)){
            testPass->decrementTex();
        } else if (window->getInput().getKeyboard().getKeyDown(xng::KEY_RIGHT)) {
            testPass->incrementTex();
        }

        renderer.render(scene);
        window->swapBuffers();
    }

    return 0;
}