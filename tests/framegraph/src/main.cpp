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
    xng::Material material = {};
    material.transparent = false;
    material.albedoTexture = ResourceHandle<Texture>(Uri("textures/wall.json"));
    material.normal = ResourceHandle<Texture>(Uri("textures/sphere_normals.json"));
    material.roughness = 1;
    material.metallic = 0;

    xng::ResourceBundle bundle;
    bundle.add("sphere", std::make_unique<xng::Material>(material));

    // Cube Smiley
    material = {};
    material.albedoTexture = ResourceHandle<Texture>(Uri("textures/awesomeface.json"));
    material.transparent = true;
    material.roughness = 1;
    material.metallic = 0;

    bundle.add("cube", std::make_unique<xng::Material>(material));

    // Cube Transparent
    material = {};
    material.albedo = ColorRGBA(0, 0, 255, 120);
    material.transparent = true;

    bundle.add("cubeAlpha", std::make_unique<xng::Material>(material));

    // Cube Transparent Red
    material = {};
    material.albedo = ColorRGBA::red(1, 200);
    material.transparent = true;

    bundle.add("cubeAlphaRed", std::make_unique<xng::Material>(material));

    // Cube Wall
    material = {};
    material.albedoTexture = ResourceHandle<Texture>(Uri("textures/wall.json"));

    bundle.add("cubeWall", std::make_unique<xng::Material>(material));

    // Pbr Spheres
    for (int x = 0; x < 10; x++) {
        for (int y = 0; y < 10; y++) {
            material = {};
            material.albedo = ColorRGBA::red();
            material.metallic = (((float) x) / 10.0f);
            material.roughness = (((float) y) / 10.0f);
            material.normal = ResourceHandle<Texture>(Uri("textures/sphere_normals.json"));
            bundle.add("PbrSphere-" + std::to_string(x) + "-" + std::to_string(y),
                       std::make_unique<xng::Material>(material));
        }
    }


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

    FrameGraphPipeline pipeline = FrameGraphPipeline()
            .addPass(std::make_shared<ClearPass>())
            .addPass(std::make_shared<ConstructionPass>())
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

    auto transformProp = Scene::TransformProperty();
    transformProp.transform.setPosition({0, 5, -25});
    sphere.addProperty(transformProp);

    auto meshProp = Scene::SkinnedMeshProperty();
    meshProp.mesh = xng::ResourceHandle<xng::SkinnedMesh>(xng::Uri("meshes/sphere.obj/Sphere"));
    sphere.addProperty(meshProp);

    auto materialProp = Scene::MaterialProperty();
    materialProp.materials[0] = xng::ResourceHandle<xng::Material>(xng::Uri(MATERIALS_PATH + std::string("/sphere")));
    auto mat = materialProp.materials[0].get();
    sphere.addProperty(materialProp);

    scene.rootNode.childNodes.emplace_back(sphere);

    for (int x = 0; x < 10; x++) {
        for (int y = 0; y < 10; y++) {
            xng::Scene::Node pbrSphere;

            transformProp = {};
            transformProp.transform.setPosition({(float) (x - 4.5) * 2, (float) (y - 4.5) * 2, -15});
            pbrSphere.addProperty(transformProp);

            meshProp = {};
            meshProp.mesh = xng::ResourceHandle<xng::SkinnedMesh>(xng::Uri("meshes/sphere.obj/Sphere"));
            pbrSphere.addProperty(meshProp);

            materialProp = {};
            materialProp.materials[0] = xng::ResourceHandle<xng::Material>(
                    xng::Uri(
                            MATERIALS_PATH + std::string("/PbrSphere-" + std::to_string(x) + "-" + std::to_string(y))));
            pbrSphere.addProperty(materialProp);

            scene.rootNode.childNodes.emplace_back(pbrSphere);
        }
    }

    xng::Scene::Node cubeWall;

    transformProp = {};
    transformProp.transform.setPosition({2.5, 0, -10});
    cubeWall.addProperty(transformProp);

    meshProp = {};
    meshProp.mesh = xng::ResourceHandle<xng::SkinnedMesh>(xng::Uri("meshes/cube_faceuv.obj"));
    cubeWall.addProperty(meshProp);

    materialProp = {};
    materialProp.materials[0] = xng::ResourceHandle<xng::Material>(xng::Uri(MATERIALS_PATH + std::string("/cubeWall")));
    cubeWall.addProperty(materialProp);

    scene.rootNode.childNodes.emplace_back(cubeWall);

    xng::Scene::Node cube;

    transformProp = {};
    transformProp.transform.setPosition({0, 0, -20});
    transformProp.transform.setScale(Vec3f(10, 10, 1));
    cube.addProperty(transformProp);

    meshProp = {};
    meshProp.mesh = xng::ResourceHandle<xng::SkinnedMesh>(xng::Uri("meshes/cube.obj"));
    cube.addProperty(meshProp);

    materialProp = {};
    materialProp.materials[0] = xng::ResourceHandle<xng::Material>(
            xng::Uri(MATERIALS_PATH + std::string("/cubeAlphaRed")));
    cube.addProperty(materialProp);

    scene.rootNode.childNodes.emplace_back(cube);

    transformProp = {};
    transformProp.transform.setPosition({-2.5, 0, -10});
    sphere.addProperty(transformProp);

    meshProp = {};
    meshProp.mesh = xng::ResourceHandle<xng::SkinnedMesh>(xng::Uri("meshes/sphere.obj/Sphere"));
    sphere.addProperty(meshProp);

    scene.rootNode.childNodes.emplace_back(sphere);

    transformProp = {};
    transformProp.transform.setPosition({0, 0, -10});
    sphere.addProperty(transformProp);

    meshProp = {};
    meshProp.mesh = xng::ResourceHandle<xng::SkinnedMesh>(xng::Uri("meshes/sphere.obj/Sphere"));
    sphere.addProperty(meshProp);

    scene.rootNode.childNodes.emplace_back(sphere);

    transformProp = {};
    transformProp.transform.setPosition({-2.5, 0, -10});
    transformProp.transform.setScale(Vec3f(1, 1, 1));
    cube.addProperty(transformProp);

    meshProp = {};
    meshProp.mesh = xng::ResourceHandle<xng::SkinnedMesh>(xng::Uri("meshes/cube_faceuv.obj"));
    cube.addProperty(meshProp);

    materialProp = {};
    materialProp.materials[0] = xng::ResourceHandle<xng::Material>(xng::Uri(MATERIALS_PATH + std::string("/cube")));
    cube.addProperty(materialProp);

    scene.rootNode.childNodes.emplace_back(cube);

    transformProp = {};
    transformProp.transform.setPosition({0, 0, -5});
    cube.addProperty(transformProp);

    meshProp = {};
    meshProp.mesh = xng::ResourceHandle<xng::SkinnedMesh>(xng::Uri("meshes/cube.obj"));
    cube.addProperty(meshProp);

    materialProp = {};
    materialProp.materials[0] = xng::ResourceHandle<xng::Material>(
            xng::Uri(MATERIALS_PATH + std::string("/cubeAlpha")));
    cube.addProperty(materialProp);

    scene.rootNode.childNodes.emplace_back(cube);

    transformProp = {};
    transformProp.transform.setPosition({2.5, 2.5, 0});

    transformProp = {};
    transformProp.transform.setPosition({2.5, 2.5, 0});

    Scene::PointLightProperty lightProp;
    lightProp.light.power = 10;
    lightProp.light.color = ColorRGBA::white();

    Scene::Node lightNode;
    lightNode.addProperty(transformProp);
    lightNode.addProperty(lightProp);
    scene.rootNode.childNodes.emplace_back(lightNode);

    transformProp = {};
    transformProp.transform.setPosition({-2.5, 2.5, 0});

    lightNode = {};
    lightNode.addProperty(transformProp);
    lightNode.addProperty(lightProp);
    scene.rootNode.childNodes.emplace_back(lightNode);

    transformProp = {};
    transformProp.transform.setPosition({2.5, -2.5, 0});

    lightNode = {};
    lightNode.addProperty(transformProp);
    lightNode.addProperty(lightProp);
    scene.rootNode.childNodes.emplace_back(lightNode);

    transformProp = {};
    transformProp.transform.setPosition({-2.5, -2.5, 0});

    lightNode = {};
    lightNode.addProperty(transformProp);
    lightNode.addProperty(lightProp);
    scene.rootNode.childNodes.emplace_back(lightNode);

    auto text = textRenderer.render("GBUFFER POSITION", TextLayout{.lineHeight = 70});
    auto tex = ren2d.createTexture(text.getImage());

    auto &cameraRef = scene.rootNode.getProperty<Scene::CameraProperty>();

    auto lights = scene.rootNode.findAll({typeid(Scene::SkinnedMeshProperty)});

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

        if (window->getInput().getKeyboard().getKey(KEY_R)) {
            for (auto &node: lights) {
                auto &transform = node.getProperty<Scene::TransformProperty>().transform;
                transform.setPosition(transform.getPosition() + Vec3f(0, 0, 1.0f * deltaTime));
            }
        } else if (window->getInput().getKeyboard().getKey(KEY_F)) {
            for (auto &node: lights) {
                auto &transform = node.getProperty<Scene::TransformProperty>().transform;
                transform.setPosition(transform.getPosition() - Vec3f(0, 0, 1.0f * deltaTime));
            }
        }

        renderer.render(scene);

        auto txt = testPass->getTexName();
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