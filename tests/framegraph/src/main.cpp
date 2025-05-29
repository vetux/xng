/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2024  Julian Zampiccoli
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
#include "xng/platform/glfw/glfw.hpp"
#include "xng/platform/opengl/opengl.hpp"
#include "xng/platform/glslang/glslang.hpp"
#include "xng/platform/spirv-cross/spirvcross.hpp"
#include "xng/platform/freetype/freetype.hpp"
#include "xng/platform/assimp/assimp.hpp"

#include "debugpass.hpp"
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


    auto msg = xng::JsonImporter::createBundle(bundle);

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
    std::vector<std::unique_ptr<ResourceImporter>> importers;
    importers.emplace_back(std::make_unique<StbiImporter>());
    importers.emplace_back(std::make_unique<JsonImporter>());
    importers.emplace_back(std::make_unique<FontImporter>());
    importers.emplace_back(std::make_unique<AssImp>());
    xng::ResourceRegistry::getDefaultRegistry().setImporters(std::move(importers));

    xng::ResourceRegistry::getDefaultRegistry().addArchive("file", std::make_shared<DirectoryArchive>("assets/"));

    auto &archive = dynamic_cast<xng::MemoryArchive &>(xng::ResourceRegistry::getDefaultRegistry().getArchive(
            "memory"));

    createMaterialResource(archive);

    auto displayDriver = glfw::GLFW();
    auto gpuDriver = opengl::OpenGL();
    auto shaderCompiler = glslang::GLSLang();
    auto shaderDecompiler = spirv_cross::SpirvCross();
    auto fontDriver = freetype::FreeType();

    auto fontFs = std::ifstream("assets/fonts/Sono/static/Sono/Sono-Regular.ttf", std::ios_base::in | std::ios::binary);
    auto font = fontDriver.createFontRenderer(fontFs);

    auto window = displayDriver.createWindow(gpuDriver.getBackend(),
                                             "XNG FrameGraph Test",
                                             {800, 600},
                                             WindowAttributes{
                                                     .vsync = true,
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

    xng::FrameGraphRenderer renderer(std::make_unique<xng::FrameGraphRuntimeSimple>(*target,
                                                                                    *device,
                                                                                    shaderCompiler,
                                                                                    shaderDecompiler));

    auto testPass = std::make_shared<DebugPass>();

    FrameGraphPipeline pipeline = FrameGraphPipeline()
            .addPass(std::make_shared<ClearPass>())
            .addPass(std::make_shared<ConstructionPass>())
            .addPass(std::make_shared<DeferredLightingPass>())
            .addPass(std::make_shared<ForwardLightingPass>())
            .addPass(std::make_shared<SkyboxPass>())
            .addPass(std::make_shared<CompositePass>())
            .addPass(std::make_shared<PresentationPass>())
            .addPass(testPass);

    renderer.setPipeline(pipeline);

    xng::Scene scene;

    xng::Node node;

    CameraProperty cameraProperty;
    cameraProperty.camera.type = xng::PERSPECTIVE;
    node.addProperty(cameraProperty);

    auto transformProp = TransformProperty();
    node.addProperty(transformProp);

    scene.rootNode.childNodes.emplace_back(node);

    node = {};

    SkyboxProperty skybox;
    skybox.skybox.texture = ResourceHandle<CubeMap>(Uri("textures/skybox_blue.json"));
    node.addProperty(skybox);

    scene.rootNode.childNodes.emplace_back(node);

    node = {};

    node.addProperty(transformProp);

    auto meshProp = SkinnedMeshProperty();
    auto materialProp = MaterialProperty();

    for (int x = 0; x < 10; x++) {
        for (int y = 0; y < 10; y++) {
            xng::Node pbrSphere;

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

    node = {};

    transformProp = {};
    transformProp.transform.setPosition({2.5, 0, -10});
    node.addProperty(transformProp);

    meshProp = {};
    meshProp.mesh = xng::ResourceHandle<xng::SkinnedMesh>(xng::Uri("meshes/cube_faceuv.obj"));
    node.addProperty(meshProp);

    materialProp = {};
    materialProp.materials[0] = xng::ResourceHandle<xng::Material>(xng::Uri(MATERIALS_PATH + std::string("/cubeWall")));
    node.addProperty(materialProp);

    scene.rootNode.childNodes.emplace_back(node);

    node = {};

    transformProp = {};
    transformProp.transform.setPosition({0, 0, -20});
    transformProp.transform.setScale(Vec3f(10, 10, 1));
    node.addProperty(transformProp);

    meshProp = {};
    meshProp.mesh = xng::ResourceHandle<xng::SkinnedMesh>(xng::Uri("meshes/cube.obj"));
    node.addProperty(meshProp);

    materialProp = {};
    materialProp.materials[0] = xng::ResourceHandle<xng::Material>(
            xng::Uri(MATERIALS_PATH + std::string("/cubeAlphaRed")));
    node.addProperty(materialProp);

    scene.rootNode.childNodes.emplace_back(node);

    transformProp = {};
    transformProp.transform.setPosition({-2.5, 0, -10});
    node.addProperty(transformProp);

    meshProp = {};
    meshProp.mesh = xng::ResourceHandle<xng::SkinnedMesh>(xng::Uri("meshes/sphere.obj/Sphere"));
    node.addProperty(meshProp);

    materialProp = MaterialProperty();
    materialProp.materials[0] = xng::ResourceHandle<xng::Material>(xng::Uri(MATERIALS_PATH + std::string("/sphere")));
    node.addProperty(materialProp);

    scene.rootNode.childNodes.emplace_back(node);

    transformProp = {};
    transformProp.transform.setPosition({0, 0, -10});
    node.addProperty(transformProp);

    meshProp = {};
    meshProp.mesh = xng::ResourceHandle<xng::SkinnedMesh>(xng::Uri("meshes/sphere.obj/Sphere"));
    node.addProperty(meshProp);

    materialProp = MaterialProperty();
    materialProp.materials[0] = xng::ResourceHandle<xng::Material>(xng::Uri(MATERIALS_PATH + std::string("/sphere")));
    node.addProperty(materialProp);

    scene.rootNode.childNodes.emplace_back(node);

    transformProp = {};
    transformProp.transform.setPosition({-2.5, 0, -10});
    transformProp.transform.setScale(Vec3f(1, 1, 1));
    node.addProperty(transformProp);

    meshProp = {};
    meshProp.mesh = xng::ResourceHandle<xng::SkinnedMesh>(xng::Uri("meshes/cube_faceuv.obj"));
    node.addProperty(meshProp);

    materialProp = {};
    materialProp.materials[0] = xng::ResourceHandle<xng::Material>(xng::Uri(MATERIALS_PATH + std::string("/cube")));
    node.addProperty(materialProp);

    scene.rootNode.childNodes.emplace_back(node);

    transformProp = {};
    transformProp.transform.setPosition({0, 0, -5});
    node.addProperty(transformProp);

    meshProp = {};
    meshProp.mesh = xng::ResourceHandle<xng::SkinnedMesh>(xng::Uri("meshes/cube.obj"));
    node.addProperty(meshProp);

    materialProp = {};
    materialProp.materials[0] = xng::ResourceHandle<xng::Material>(
            xng::Uri(MATERIALS_PATH + std::string("/cubeAlpha")));
    node.addProperty(materialProp);

    scene.rootNode.childNodes.emplace_back(node);

    transformProp = {};
    transformProp.transform.setPosition({2.5, 2.5, 0});

    transformProp = {};
    transformProp.transform.setPosition({2.5, 2.5, 0});

    PointLightProperty lightProp;
    lightProp.light.power = 50;
    lightProp.light.color = ColorRGBA::white();

    node = {};

    node.addProperty(transformProp);
    node.addProperty(lightProp);
    scene.rootNode.childNodes.emplace_back(node);

    transformProp = {};
    transformProp.transform.setPosition({-2.5, 2.5, 0});

    node = {};
    node.addProperty(transformProp);
    node.addProperty(lightProp);
    scene.rootNode.childNodes.emplace_back(node);

    transformProp = {};
    transformProp.transform.setPosition({2.5, -2.5, 0});

    node = {};
    node.addProperty(transformProp);
    node.addProperty(lightProp);
    scene.rootNode.childNodes.emplace_back(node);

    transformProp = {};
    transformProp.transform.setPosition({-2.5, -2.5, 0});

    node = {};
    node.addProperty(transformProp);
    node.addProperty(lightProp);
    scene.rootNode.childNodes.emplace_back(node);

    auto text = textRenderer.render("GBUFFER POSITION", TextLayout{.lineHeight = 70});
    auto tex = ren2d.createTexture(text.getImage());

    auto &cameraRef = scene.rootNode.find<CameraProperty>().getProperty<CameraProperty>();
    auto &cameraTransformRef = scene.rootNode.find<CameraProperty>().getProperty<TransformProperty>();

    auto lights = scene.rootNode.findAll({typeid(SkinnedMeshProperty)});

    CameraController cameraController(cameraTransformRef.transform, input);

    xng::FrameLimiter limiter;
    limiter.reset();
    while (!window->shouldClose()) {
        auto deltaTime = limiter.newFrame();

        cameraRef.camera.aspectRatio = static_cast<float>(window->getWindowSize().x)
                                       / static_cast<float>(window->getWindowSize().y);

        cameraController.update(deltaTime);

        if (window->getInput().getKeyboard().getKeyDown(xng::KEY_LEFT)) {
            testPass->decrementSlot();
        } else if (window->getInput().getKeyboard().getKeyDown(xng::KEY_RIGHT)) {
            testPass->incrementSlot();
        }

        if (window->getInput().getKey(KEY_R)) {
            for (auto &ln: lights) {
                auto &transform = ln.getProperty<TransformProperty>().transform;
                transform.setPosition(transform.getPosition() + Vec3f(0, 0, 1.0f * deltaTime));
            }
        } else if (window->getInput().getKey(KEY_F)) {
            for (auto &ln: lights) {
                auto &transform = ln.getProperty<TransformProperty>().transform;
                transform.setPosition(transform.getPosition() - Vec3f(0, 0, 1.0f * deltaTime));
            }
        }

        renderer.render(scene);

        auto txt = testPass->getSlotName();
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
        ren2d.draw(Rectf({}, text.getImage().getResolution().convert<float>()),
                   Rectf({static_cast<float>(target->getDescription().size.x) / 2 -
                          static_cast<float>(text.getImage().getResolution().x) / 2, 0},
                         text.getImage().getResolution().convert<float>()),
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