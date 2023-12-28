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
#include "xng/driver/glfw/glfwdisplaydriver.hpp"
#include "xng/driver/opengl/oglgpudriver.hpp"
#include "xng/driver/glslang/glslangcompiler.hpp"
#include "xng/driver/spirv-cross/spirvcrossdecompiler.hpp"
#include "xng/driver/freetype/ftfontdriver.hpp"
#include "xng/driver/assimp/assimpimporter.hpp"

#include "debugpass.hpp"
#include "cameracontroller.hpp"
#include "debugoverlay.hpp"

static const char *MATERIALS_PATH = "memory://tests/graph/materials.json";

void createMaterialResource(xng::MemoryArchive &archive) {
    xng::ResourceBundle bundle;

    // Cube Wall
    xng::Material material = {};
    material.transparent = false;
    material.albedoTexture = ResourceHandle<Texture>(Uri("textures/wall.json"));

    bundle.add("cubeWall", std::make_unique<xng::Material>(material));

    material = {};
    material.transparent = false;
    material.albedo = ColorRGBA::red();
    material.metallic = 0.7;
    material.roughness = 0.5;
    material.normal = ResourceHandle<Texture>(Uri("textures/sphere_normals.json"));
    bundle.add("sphere", std::make_unique<xng::Material>(material));

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
    importers.emplace_back(std::make_unique<AssImpImporter>());
    xng::ResourceRegistry::getDefaultRegistry().setImporters(std::move(importers));

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
    auto font = fontDriver.createFontRenderer(fontFs);

    auto window = displayDriver.createWindow(gpuDriver.getBackend(),
                                             "XNG Shadows Test",
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
            .addPass(std::make_shared<ShadowMappingPass>())
            .addPass(std::make_shared<DeferredLightingPass>())
            .addPass(std::make_shared<ForwardLightingPass>())
            .addPass(std::make_shared<CompositePass>())
            .addPass(std::make_shared<PresentationPass>())
            .addPass(testPass);

    renderer.setPipeline(pipeline);

    xng::Scene scene;

    Node node;
    CameraProperty cameraProperty;
    cameraProperty.camera.type = xng::PERSPECTIVE;
    node.addProperty(cameraProperty);

    auto transformProp = TransformProperty();
    node.addProperty(transformProp);

    scene.rootNode.childNodes.emplace_back(node);

    ShadowProperty shadowProp;
    shadowProp.castShadows = true;
    shadowProp.receiveShadows = true;

    xng::Node cubeWall;

    transformProp = {};
    transformProp.transform.setPosition({0, 0, -10});
    transformProp.transform.setScale({10, 10, 1});
    cubeWall.addProperty(transformProp);

    SkinnedMeshProperty meshProp = {};
    meshProp.mesh = xng::ResourceHandle<xng::SkinnedMesh>(xng::Uri("meshes/cube_faceuv.obj"));
    cubeWall.addProperty(meshProp);

    MaterialProperty materialProp = {};
    materialProp.materials[0] = xng::ResourceHandle<xng::Material>(xng::Uri(MATERIALS_PATH + std::string("/cubeWall")));
    cubeWall.addProperty(materialProp);

    scene.rootNode.childNodes.emplace_back(cubeWall);

    Node sphere;

    transformProp = {};
    transformProp.transform.setPosition({0, 0, -5});
    sphere.addProperty(transformProp);

    meshProp = {};
    meshProp.mesh = xng::ResourceHandle<xng::SkinnedMesh>(xng::Uri("meshes/sphere.obj"));
    sphere.addProperty(meshProp);

    materialProp = {};
    materialProp.materials[0] = xng::ResourceHandle<xng::Material>(xng::Uri(MATERIALS_PATH + std::string("/sphere")));
    sphere.addProperty(materialProp);

    scene.rootNode.childNodes.emplace_back(sphere);

    transformProp = {};
    transformProp.transform.setPosition({0, 0, 0});

    /*  PointLightProperty dirLight;
      dirLight.light.power = 45;*/

    DirectionalLightProperty dirLight;
    dirLight.light.power = 1;
    dirLight.light.castShadows = true;

    Node light;

    light.addProperty(dirLight);

    transformProp = {};
    light.addProperty(transformProp);

    scene.rootNode.childNodes.emplace_back(light);

    light = {};

    SpotLightProperty spotLight = {};
    spotLight.light.power = 20;
    spotLight.light.direction = Vec3f(0, 0, -1);
    light.addProperty(spotLight);

    transformProp = {};
    transformProp.transform.setPosition({0, 0, 5});
    light.addProperty(transformProp);

    scene.rootNode.childNodes.emplace_back(light);

    auto text = textRenderer.render("GBUFFER POSITION", TextLayout{.lineHeight = 70});
    auto tex = ren2d.createTexture(text.getImage());

    auto &cameraRef = scene.rootNode.find<CameraProperty>().getProperty<CameraProperty>();
    auto &cameraTransformRef = scene.rootNode.find<CameraProperty>().getProperty<TransformProperty>();

    auto lights = scene.rootNode.findAll({typeid(SkinnedMeshProperty)});

    CameraController cameraController(cameraTransformRef.transform, input);

    auto &lightTransform = light.getProperty<TransformProperty>().transform;
    auto &dLight = light.getProperty<SpotLightProperty>().light;

    xng::FrameLimiter limiter;
    limiter.reset();
    while (!window->shouldClose()) {
        auto deltaTime = limiter.newFrame();

        cameraRef.camera.aspectRatio = static_cast<float>(window->getWindowSize().x)
                                       / static_cast<float>(window->getWindowSize().y);

        cameraController.update(deltaTime);

        if (input.getKeyboard().getKeyDown(xng::KEY_LEFT)) {
            testPass->decrementSlot();
        } else if (input.getKeyboard().getKeyDown(xng::KEY_RIGHT)) {
            testPass->incrementSlot();
        }

        if (input.getKeyboard().getKey(KEY_R)) {
            for (auto &ln: lights) {
                auto &transform = ln.getProperty<TransformProperty>().transform;
                transform.setPosition(transform.getPosition() + Vec3f(0, 0, 1.0f * deltaTime));
            }
        } else if (input.getKeyboard().getKey(KEY_F)) {
            for (auto &ln: lights) {
                auto &transform = ln.getProperty<TransformProperty>().transform;
                transform.setPosition(transform.getPosition() - Vec3f(0, 0, 1.0f * deltaTime));
            }
        }

        if (input.getKeyboard().getKey(KEY_KP_8)) {
            auto v = MatrixMath::rotate(Vec3f{20, 0, 0} * deltaTime)
                     * Vec4f(dLight.direction.x, dLight.direction.y, dLight.direction.z, 1);
            dLight.direction = Vec3f(v.x, v.y, v.z);
            // lightTransform.setPosition(lightTransform.getPosition() + Vec3f(0, 0, 1) * deltaTime);
        } else if (input.getKeyboard().getKey(KEY_KP_2)) {
            auto v = MatrixMath::rotate(Vec3f{-20, 0, 0} * deltaTime)
                     * Vec4f(dLight.direction.x, dLight.direction.y, dLight.direction.z, 1);
            dLight.direction = Vec3f(v.x, v.y, v.z);
            // lightTransform.setPosition(lightTransform.getPosition() + Vec3f(0, 0, -1) * deltaTime);
        }

        if (input.getKeyboard().getKey(KEY_KP_4)) {
            auto v = MatrixMath::rotate(Vec3f{0, 20, 0} * deltaTime)
                     * Vec4f(dLight.direction.x, dLight.direction.y, dLight.direction.z, 1);
            dLight.direction = Vec3f(v.x, v.y, v.z);
            //lightTransform.setPosition(lightTransform.getPosition() + Vec3f(1, 0, 0) * deltaTime);
        } else if (input.getKeyboard().getKey(KEY_KP_6)) {
            auto v = MatrixMath::rotate(Vec3f{0, -20, 0} * deltaTime)
                     * Vec4f(dLight.direction.x, dLight.direction.y, dLight.direction.z, 1);
            dLight.direction = Vec3f(v.x, v.y, v.z);
            //   lightTransform.setPosition(lightTransform.getPosition() + Vec3f(-1, 0, 0) * deltaTime);
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