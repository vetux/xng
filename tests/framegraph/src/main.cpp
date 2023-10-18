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

#include <memory>
#include <fstream>

#include "xng/xng.hpp"

#include "testpass.hpp"
#include "cameracontroller.hpp"
#include "debugoverlay.hpp"

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


// TODO: Fix OUT_OF_MEMORY thrown after running the framegraph test for some time on windows (The task manager is not showing any kind of memory leak.)
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
    auto shaderCompiler = glslang::GLSLangCompiler();
    auto shaderDecompiler = spirv_cross::SpirvCrossDecompiler();
    auto fontDriver = freetype::FtFontDriver();

    auto fontFs = std::ifstream("assets/fonts/Sono/static/Sono/Sono-Regular.ttf", std::ios_base::in | std::ios::binary);
    auto font = fontDriver.createFont(fontFs);

    auto window = displayDriver.createWindow(gpuDriver.getBackend(),
                                             "XNG FrameGraph Test",
                                             {800, 600},
                                             WindowAttributes{
                                                     .swapInterval = 1,
                                                     .debug = false
                                             });
    auto &input = window->getInput();

    auto device = gpuDriver.createRenderDevice();

    device->setDebugCallback([](const std::string &msg) {
        std::cout << msg << "\n";
    });

    auto ren2d = Renderer2D(*device, shaderCompiler, shaderDecompiler);

    DebugOverlay overlay(*font, ren2d);

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

    xng::Scene scene;
    Scene::CameraProperty cameraProperty;
    cameraProperty.camera.type = xng::PERSPECTIVE;
    scene.rootNode.addProperty(cameraProperty);

    xng::Scene::Node sphere;

    auto meshProp = Scene::SkinnedMeshProperty();
    meshProp.transform.setPosition({0, 5, -16});
    meshProp.mesh = xng::ResourceHandle<xng::SkinnedMesh>(xng::Uri("meshes/sphere.obj/Sphere"));
    sphere.addProperty(meshProp);

    auto materialProp = Scene::MaterialProperty();
    materialProp.materials[0] = xng::ResourceHandle<xng::Material>(xng::Uri(MATERIALS_PATH + std::string("/sphere")));
    sphere.addProperty(materialProp);

    scene.rootNode.childNodes.emplace_back(sphere);

    xng::Scene::Node cubeWall;

    meshProp = {};
    meshProp.transform.setPosition({2.5, 0, -10});
    meshProp.mesh = xng::ResourceHandle<xng::SkinnedMesh>(xng::Uri("meshes/cube_faceuv.obj"));
    cubeWall.addProperty(meshProp);

    materialProp = {};
    materialProp.materials[0] = xng::ResourceHandle<xng::Material>(xng::Uri(MATERIALS_PATH + std::string("/cubeWall")));
    cubeWall.addProperty(materialProp);

    scene.rootNode.childNodes.emplace_back(cubeWall);

    xng::Scene::Node cube;

    meshProp = {};
    meshProp.transform.setPosition({0, 0, -15});
    meshProp.transform.setScale(Vec3f(10, 10, 1));
    meshProp.mesh = xng::ResourceHandle<xng::SkinnedMesh>(xng::Uri("meshes/cube.obj"));
    cube.addProperty(meshProp);

    materialProp = {};
    materialProp.materials[0] = xng::ResourceHandle<xng::Material>(
            xng::Uri(MATERIALS_PATH + std::string("/cubeAlphaRed")));
    cube.addProperty(materialProp);

    scene.rootNode.childNodes.emplace_back(cube);

    meshProp = {};
    meshProp.transform.setPosition({-2.5, 0, -10});
    meshProp.mesh = xng::ResourceHandle<xng::SkinnedMesh>(xng::Uri("meshes/sphere.obj/Sphere"));
    sphere.addProperty(meshProp);

    scene.rootNode.childNodes.emplace_back(sphere);

    meshProp = {};
    meshProp.transform.setPosition({0, 0, -10});
    meshProp.mesh = xng::ResourceHandle<xng::SkinnedMesh>(xng::Uri("meshes/sphere.obj/Sphere"));
    sphere.addProperty(meshProp);

    scene.rootNode.childNodes.emplace_back(sphere);

    meshProp = {};
    meshProp.transform.setPosition({-2.5, 0, -10});
    meshProp.transform.setScale(Vec3f(1, 1, 1));
    meshProp.mesh = xng::ResourceHandle<xng::SkinnedMesh>(xng::Uri("meshes/cube_faceuv.obj"));
    cube.addProperty(meshProp);

    materialProp = {};
    materialProp.materials[0] = xng::ResourceHandle<xng::Material>(xng::Uri(MATERIALS_PATH + std::string("/cube")));
    cube.addProperty(materialProp);

    scene.rootNode.childNodes.emplace_back(cube);

    meshProp = {};
    meshProp.transform.setPosition({0, 0, -5});
    meshProp.transform.setScale(Vec3f(1, 1.2, 1));
    meshProp.mesh = xng::ResourceHandle<xng::SkinnedMesh>(xng::Uri("meshes/cube.obj"));
    cube.addProperty(meshProp);

    materialProp = {};
    materialProp.materials[0] = xng::ResourceHandle<xng::Material>(
            xng::Uri(MATERIALS_PATH + std::string("/cubeAlpha")));
    cube.addProperty(materialProp);

    scene.rootNode.childNodes.emplace_back(cube);

    Scene::LightingProperty lightingProperty;

    xng::PointLight light;
    light.transform.setPosition({0, 0, -5});

    lightingProperty.pointLights.emplace_back(light);

    scene.rootNode.addProperty(lightingProperty);

    auto text = textRenderer.render("GBUFFER POSITION", TextLayout{.lineHeight = 70});
    auto tex = ren2d.createTexture(text.getImage());

    testPass->setTex(13);

    auto &cameraRef = scene.rootNode.getProperty<Scene::CameraProperty>();

    CameraController cameraController(cameraRef.cameraTransform, input);

    xng::FrameLimiter limiter;
    limiter.reset();
    while (!window->shouldClose()) {
        auto deltaTime = limiter.newFrame();

        cameraRef.camera.aspectRatio = static_cast<float>(window->getWindowSize().x)
                                   / static_cast<float>(window->getWindowSize().y);

        cameraController.update(deltaTime);

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

        ren2d.renderBegin(*target,
                          false,
                          {},
                          {},
                          target->getDescription().size,
                          {});
        ren2d.draw(Rectf({}, text.getImage().getSize().convert<float>()),
                   Rectf({static_cast<float>(target->getDescription().size.x) / 2 -
                          static_cast<float>(text.getImage().getSize().x) / 2, 0},
                         text.getImage().getSize().convert<float>()),
                   tex,
                   {},
                   0,
                   NEAREST,
                   ColorRGBA::white());
        ren2d.renderPresent();

        overlay.draw(deltaTime, *target);

        window->swapBuffers();
        window->update();
    }

    return 0;
}