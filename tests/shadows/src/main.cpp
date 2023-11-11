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

#include "debugpass.hpp"
#include "cameracontroller.hpp"
#include "debugoverlay.hpp"

static const char *MATERIALS_PATH = "memory://tests/graph/materials.json";

void createMaterialResource(xng::MemoryArchive &archive) {
    xng::ResourceBundle bundle;

    // Cube Wall
    xng::Material material = {};
    material.transparent = true;
    material.albedoTexture = ResourceHandle<Texture>(Uri("textures/wall.json"));

    bundle.add("cubeWall", std::make_unique<xng::Material>(material));

    // Pbr Spheres
    for (int x = 0; x < 4; x++) {
        for (int y = 0; y < 4; y++) {
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
                                             "XNG Shadows Test",
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
    shadowProp.castShadows = false;
    shadowProp.receiveShadows = false;

    for (int x = 0; x < 4; x++) {
        for (int y = 0; y < 4; y++) {
            xng::Node pbrSphere;

            if (y == 1) {
                pbrSphere.addProperty(shadowProp);
            }

            TransformProperty transformProp = {};
            transformProp.transform.setPosition({(float) (x - 1.5) * 2, (float) (y - 1.5) * 2, -15});
            pbrSphere.addProperty(transformProp);

            SkinnedMeshProperty meshProp = {};
            meshProp.mesh = xng::ResourceHandle<xng::SkinnedMesh>(xng::Uri("meshes/sphere.obj/Sphere"));
            pbrSphere.addProperty(meshProp);

            MaterialProperty materialProp = {};
            materialProp.materials[0] = xng::ResourceHandle<xng::Material>(
                    xng::Uri(
                            MATERIALS_PATH + std::string("/PbrSphere-" + std::to_string(x) + "-" + std::to_string(y))));
            pbrSphere.addProperty(materialProp);

            scene.rootNode.childNodes.emplace_back(pbrSphere);
        }
    }

    xng::Node cubeWall;

    transformProp = {};
    transformProp.transform.setPosition({0, 0, -20});
    transformProp.transform.setScale({10, 10, 1});
    cubeWall.addProperty(transformProp);

    SkinnedMeshProperty meshProp = {};
    meshProp.mesh = xng::ResourceHandle<xng::SkinnedMesh>(xng::Uri("meshes/cube_faceuv.obj"));
    cubeWall.addProperty(meshProp);

    MaterialProperty materialProp = {};
    materialProp.materials[0] = xng::ResourceHandle<xng::Material>(xng::Uri(MATERIALS_PATH + std::string("/cubeWall")));
    cubeWall.addProperty(materialProp);

    scene.rootNode.childNodes.emplace_back(cubeWall);

    transformProp = {};
    transformProp.transform.setPosition({5.5, 5.5, 0});

    DirectionalLightProperty dirLight;
    dirLight.light.power = 0.1;

    Node lightNode;
    lightNode.addProperty(dirLight);
    scene.rootNode.childNodes.emplace_back(lightNode);

    PointLightProperty lightProp;
    lightProp.light.power = 10;
    lightProp.light.color = ColorRGBA::white();

    lightNode = {};
    lightNode.addProperty(transformProp);
    lightNode.addProperty(lightProp);
    scene.rootNode.childNodes.emplace_back(lightNode);

    lightProp.light.castShadows = false;

    transformProp = {};
    transformProp.transform.setPosition({-5.5, 5.5, 0});

    lightNode = {};
    lightNode.addProperty(transformProp);
    lightNode.addProperty(lightProp);
    scene.rootNode.childNodes.emplace_back(lightNode);

    transformProp = {};
    transformProp.transform.setPosition({5.5, -5.5, 0});

    SpotLightProperty spotProp;
    lightNode = {};
    lightNode.addProperty(transformProp);
    lightNode.addProperty(spotProp);
    scene.rootNode.childNodes.emplace_back(lightNode);

    transformProp = {};
    transformProp.transform.setPosition({-5.5, -5.5, 0});

    lightNode = {};
    lightNode.addProperty(transformProp);
    lightNode.addProperty(spotProp);
    scene.rootNode.childNodes.emplace_back(lightNode);

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

        if (window->getInput().getKeyboard().getKey(KEY_R)) {
            for (auto &ln: lights) {
                auto &transform = ln.getProperty<TransformProperty>().transform;
                transform.setPosition(transform.getPosition() + Vec3f(0, 0, 1.0f * deltaTime));
            }
        } else if (window->getInput().getKeyboard().getKey(KEY_F)) {
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