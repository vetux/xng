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
#include <fstream>

#include "xng/xng.hpp"

#include "testpass.hpp"

static const char *MATERIALS_PATH = "memory://tests/graph/materials.json";

void createMaterialResource(xng::MemoryArchive &archive) {
    // Sphere
    xng::Material material;
    material.shadingModel = xng::SHADE_PHONG;
    material.diffuseTexture = ResourceHandle<Texture>(Uri("textures/wall.json"));
    material.normal = ResourceHandle<Texture>(Uri("textures/sphere_normals.json"));

    xng::ResourceBundle bundle;
    bundle.add("sphere", std::make_unique<xng::Material>(material));

    // Cube Smiley
    material = {};
    material.shadingModel = xng::SHADE_PHONG;
    material.diffuseTexture = ResourceHandle<Texture>(Uri("textures/awesomeface.json"));
    material.transparent = true;

    bundle.add("cube", std::make_unique<xng::Material>(material));

    // Cube Transparent
    material = {};
    material.shadingModel = xng::SHADE_PHONG;
    material.diffuse = ColorRGBA(0, 0, 255, 120);
    material.transparent = true;

    bundle.add("cubeAlpha", std::make_unique<xng::Material>(material));

    // Cube Transparent Red
    material = {};
    material.shadingModel = xng::SHADE_PHONG;
    material.diffuse = ColorRGBA::red(1, 200);
    material.transparent = true;

    bundle.add("cubeAlphaRed", std::make_unique<xng::Material>(material));

    // Cube Wall
    material = {};
    material.shadingModel = xng::SHADE_PHONG;
    material.diffuseTexture = ResourceHandle<Texture>(Uri("textures/wall.json"));

    bundle.add("cubeWall", std::make_unique<xng::Material>(material));

    auto msg = xng::JsonParser::createBundle(bundle);

    std::stringstream stream;
    xng::JsonProtocol().serialize(stream, msg);

    std::vector<uint8_t> vec;
    for (auto &c: stream.str()) {
        vec.emplace_back(c);
    }

    Uri uri(MATERIALS_PATH);

    archive.addData(uri.toString(false), vec);

}

int main(int argc, char *argv[]) {
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
    auto fontDriver = freetype::FtFontDriver();

    auto fs = std::ifstream("assets/fonts/Sono/static/Sono/Sono-Regular.ttf");
    auto font = fontDriver.createFont(fs);

    auto window = displayDriver.createWindow(OPENGL_4_6);
    auto &input = window->getInput();

    window->bindGraphics();

    auto device = gpuDriver.createRenderDevice();

    auto ren2d = Renderer2D(*device, shaderCompiler, shaderDecompiler);

    auto textRenderer = TextRenderer(*font, ren2d, {0, 70});

    auto target = window->getRenderTarget(*device);

    xng::FrameGraphRenderer renderer(*target,
                                     *device,
                                     std::make_unique<xng::FrameGraphPoolAllocator>(*device,
                                                                                    shaderCompiler,
                                                                                    shaderDecompiler,
                                                                                    *target),
                                     shaderCompiler,
                                     shaderDecompiler);

    auto testPass = std::make_shared<TestPass>();

    FrameGraphPipeline pipeline = FrameGraphPipeline().addPass(std::make_shared<ConstructionPass>())
            .addPass(std::make_shared<DeferredLightingPass>())
            .addPass(std::make_shared<ForwardLightingPass>())
            .addPass(std::make_shared<CompositePass>())
            .addPass(std::make_shared<PresentationPass>())
            .addPass(testPass);

    renderer.setPipeline(pipeline);

    xng::PointLight light;
    light.transform.setPosition({0, 0, -5});

    xng::Scene::Object sphere;
    sphere.transform.setPosition({0, 5, -16});
    sphere.mesh = xng::ResourceHandle<xng::Mesh>(xng::Uri("meshes/sphere.obj/Sphere"));
    sphere.material = xng::ResourceHandle<xng::Material>(xng::Uri(MATERIALS_PATH + std::string("/sphere")));

    xng::Scene::Object cube;
    cube.mesh = xng::ResourceHandle<xng::Mesh>(xng::Uri("meshes/cube_faceuv.obj"));

    xng::Scene::Object cubeWall;
    cubeWall.transform.setPosition({2.5, 0, -10});
    cubeWall.mesh = xng::ResourceHandle<xng::Mesh>(xng::Uri("meshes/cube.obj"));
    cubeWall.material = xng::ResourceHandle<xng::Material>(xng::Uri(MATERIALS_PATH + std::string("/cubeWall")));

    xng::Scene scene;
    scene.camera.type = xng::PERSPECTIVE;

    scene.objects.emplace_back(sphere);

    cube.material = xng::ResourceHandle<xng::Material>(xng::Uri(MATERIALS_PATH + std::string("/cubeAlphaRed")));
    cube.transform.setPosition({0, 0, -15});
    cube.transform.setScale(Vec3f(10, 10, 1));
    scene.objects.emplace_back(cube);

    sphere.transform.setPosition({-2.5, 0, -10});
    scene.objects.emplace_back(sphere);

    sphere.transform.setPosition({0, 0, -10});
    scene.objects.emplace_back(sphere);

    cube.transform.setScale(Vec3f(1, 1, 1));
    cube.transform.setPosition({-2.5, 0, -10});
    cube.material = xng::ResourceHandle<xng::Material>(xng::Uri(MATERIALS_PATH + std::string("/cube")));

    scene.objects.emplace_back(cube);
    scene.objects.emplace_back(cubeWall);

    cube.material = xng::ResourceHandle<xng::Material>(xng::Uri(MATERIALS_PATH + std::string("/cubeAlpha")));
    cube.transform.setScale(Vec3f(1, 1.2, 1));
    cube.transform.setPosition({0, 0, -5});
    scene.objects.emplace_back(cube);

    scene.pointLights.emplace_back(light);

    auto text = textRenderer.render("GBUFFER POSITION", TextLayout{.lineHeight = 70});
    auto tex = ren2d.createTexture(text.getImage());

    testPass->setTex(13);

    xng::FrameLimiter limiter(60);
    limiter.reset();
    while (!window->shouldClose()) {
        window->update();

        auto deltaTime = limiter.newFrame();

        scene.camera.aspectRatio = static_cast<float>(window->getWindowSize().x)
                                   / static_cast<float>(window->getWindowSize().y);

        if (window->getInput().getKeyboard().getKeyDown(xng::KEY_LEFT)) {
            testPass->decrementTex();
        } else if (window->getInput().getKeyboard().getKeyDown(xng::KEY_RIGHT)) {
            testPass->incrementTex();
        }

        renderer.render(scene);

        std::string txt;
        switch (testPass->getTex()) {
            case 0:
                txt = "GBUFFER POSITION";
                break;
            case 1:
                txt = "GBUFFER NORMAL";
                break;
            case 2:
                txt = "GBUFFER TANGENT";
                break;
            case 3:
                txt = "GBUFFER ROUGHNESS_METALLIC_AO";
                break;
            case 4:
                txt = "GBUFFER ALBEDO";
                break;
            case 5:
                txt = "GBUFFER AMBIENT";
                break;
            case 6:
                txt = "GBUFFER SPECULAR";
                break;
            case 7:
                txt = "GBUFFER MODEL_OBJECT";
                break;
            case 8:
                txt = "GBUFFER DEPTH";
                break;
            case 9:
                txt = "DEFERRED COLOR";
                break;
            case 10:
                txt = "DEFERRED DEPTH";
                break;
            case 11:
                txt = "FORWARD COLOR";
                break;
            case 12:
                txt = "FORWARD DEPTH";
                break;
            case 13:
                txt = "SCREEN COLOR";
                break;
            case 14:
                txt = "SCREEN DEPTH";
                break;
        }

        if (text.getText() != txt) {
            text = textRenderer.render(txt, TextLayout{.lineHeight = 70});
            tex = ren2d.createTexture(text.getImage());
        }

        auto fpsText = textRenderer.render(std::to_string(1000 * deltaTime) + "ms", TextLayout{.lineHeight = 70});
        auto fpsTex = ren2d.createTexture(fpsText.getImage());

        ren2d.renderBegin(*target,
                          false,
                          {},
                          {},
                          target->getDescription().size,
                          {});
        ren2d.draw(Rectf({}, fpsText.getImage().getSize().convert<float>()),
                   Rectf({}, fpsText.getImage().getSize().convert<float>()),
                   fpsTex,
                   {},
                   0,
                   NEAREST,
                   ColorRGBA::white());
        ren2d.draw(Rectf({}, text.getImage().getSize().convert<float>()),
                   Rectf({static_cast<float>(target->getDescription().size.x - text.getImage().getSize().x), 0},
                         text.getImage().getSize().convert<float>()),
                   tex,
                   {},
                   0,
                   NEAREST,
                   ColorRGBA::white());
        ren2d.renderPresent();

        ren2d.destroyTexture(fpsTex);

        window->swapBuffers();
    }

    return 0;
}