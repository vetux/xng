/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2024  Julian Zampiccoli
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

#include <memory>
#include <fstream>

#include "xng/xng.hpp"
#include "xng/adapters/glfw/glfw.hpp"
#include "../../../adapters/opengl/ogl/oglgpu.hpp"
#include "../../../adapters/shared/shader/glslang.hpp"
#include "../../../adapters/shared/shader/spirvcross.hpp"
#include "xng/adapters/freetype/freetype.hpp"
#include "xng/adapters/assimp/assimp.hpp"
#include "xng/adapters/bullet3/bullet3.hpp"

#include "debugpass.hpp"
#include "cameracontroller.hpp"
#include "debugoverlay.hpp"
#include "spherecontrollersystem.hpp"

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
    material.albedo = ColorRGBA::aqua();
    material.transparent = false;
    material.roughness = 1;
    material.metallic = 0;

    bundle.add("cube", std::make_unique<xng::Material>(material));

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
    auto gpuDriver = opengl::OGLGpu();
    auto shaderCompiler = glslang::GLSLang();
    auto shaderDecompiler = spirv_cross::SpirvCross();
    auto fontDriver = freetype::FreeType();
    auto physicsDriver = bullet3::Bullet3();

    auto world = physicsDriver.createWorld();

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

    world->setGravity({0, -10, 0});

    auto p = SystemPipeline{SystemPipeline::TICK_FRAME, std::vector<std::shared_ptr<System>>{
            std::make_shared<PhysicsSystem>(*world, 1, 10),
            std::make_shared<MeshRenderSystem>(renderer),
            std::make_shared<SphereControllerSystem>(input)
    }};
    SystemRuntime runtime(std::vector<SystemPipeline>{p});

    auto &scene = *runtime.getScene();

    auto cameraEnt = scene.createEntity();
    cameraEnt.createComponent(CameraComponent());

    TransformComponent transformComponent;

    transformComponent.transform.setPosition({0, 10, 0});
    transformComponent.transform.setRotation(Quaternion({90, 0, 0}));
    cameraEnt.createComponent(transformComponent);

    auto floorEnt = scene.createEntity();

    RigidBodyComponent rigidBodyComponent;
    rigidBodyComponent.type = xng::RigidBody::STATIC;

    floorEnt.createComponent(rigidBodyComponent);

    Collider3DComponent colliderComponent;
    colliderComponent.shape.type = COLLIDER_BOX;
    colliderComponent.shape.halfExtent = Vec3f(10, 1, 10);

    floorEnt.createComponent(colliderComponent);

    transformComponent = {};
    transformComponent.transform.setScale({10, 1, 10});

    floorEnt.createComponent(transformComponent);

    SkinnedMeshComponent meshComponent;
    meshComponent.mesh = ResourceHandle<SkinnedMesh>(Uri("meshes/cube.obj"));

    floorEnt.createComponent(meshComponent);

    MaterialComponent materialComponent;
    materialComponent.materials[0] = ResourceHandle<Material>(Uri(MATERIALS_PATH + std::string("/cube")));

    floorEnt.createComponent(materialComponent);

    floorEnt = scene.createEntity();

    rigidBodyComponent = {};
    rigidBodyComponent.type = xng::RigidBody::STATIC;

    floorEnt.createComponent(rigidBodyComponent);

    colliderComponent = {};
    colliderComponent.shape.type = COLLIDER_BOX;
    colliderComponent.shape.halfExtent = Vec3f(10, 10, 1);

    floorEnt.createComponent(colliderComponent);

    transformComponent = {};
    transformComponent.transform.setPosition({0, 10, -10});
    transformComponent.transform.setScale({10, 10, 1});

    floorEnt.createComponent(transformComponent);

    meshComponent = {};
    meshComponent.mesh = ResourceHandle<SkinnedMesh>(Uri("meshes/cube.obj"));

    floorEnt.createComponent(meshComponent);

    materialComponent = {};
    materialComponent.materials[0] = ResourceHandle<Material>(Uri(MATERIALS_PATH + std::string("/cube")));

    floorEnt.createComponent(materialComponent);

    floorEnt = scene.createEntity();

    rigidBodyComponent = {};
    rigidBodyComponent.type = xng::RigidBody::STATIC;

    floorEnt.createComponent(rigidBodyComponent);

    colliderComponent = {};
    colliderComponent.shape.type = COLLIDER_BOX;
    colliderComponent.shape.halfExtent = Vec3f(10, 10, 1);

    floorEnt.createComponent(colliderComponent);

    transformComponent = {};
    transformComponent.transform.setPosition({0, 10, 10});
    transformComponent.transform.setScale({10, 10, 1});

    floorEnt.createComponent(transformComponent);

    meshComponent = {};
    meshComponent.mesh = ResourceHandle<SkinnedMesh>(Uri("meshes/cube.obj"));

    floorEnt.createComponent(meshComponent);

    materialComponent = {};
    materialComponent.materials[0] = ResourceHandle<Material>(Uri(MATERIALS_PATH + std::string("/cube")));

    floorEnt.createComponent(materialComponent);

    floorEnt = scene.createEntity();

    rigidBodyComponent = {};
    rigidBodyComponent.type = xng::RigidBody::STATIC;

    floorEnt.createComponent(rigidBodyComponent);

    colliderComponent = {};
    colliderComponent.shape.type = COLLIDER_BOX;
    colliderComponent.shape.halfExtent = Vec3f(1, 10, 10);

    floorEnt.createComponent(colliderComponent);

    transformComponent = {};
    transformComponent.transform.setPosition({10, 10, 0});
    transformComponent.transform.setScale({1, 10, 10});

    floorEnt.createComponent(transformComponent);

    meshComponent = {};
    meshComponent.mesh = ResourceHandle<SkinnedMesh>(Uri("meshes/cube.obj"));

    floorEnt.createComponent(meshComponent);

    materialComponent = {};
    materialComponent.materials[0] = ResourceHandle<Material>(Uri(MATERIALS_PATH + std::string("/cube")));

    floorEnt.createComponent(materialComponent);

    floorEnt = scene.createEntity();

    rigidBodyComponent = {};
    rigidBodyComponent.type = xng::RigidBody::STATIC;

    floorEnt.createComponent(rigidBodyComponent);

    colliderComponent = {};
    colliderComponent.shape.type = COLLIDER_BOX;
    colliderComponent.shape.halfExtent = Vec3f(1, 10, 10);

    floorEnt.createComponent(colliderComponent);

    transformComponent = {};
    transformComponent.transform.setPosition({-10, 10, 0});
    transformComponent.transform.setScale({1, 10, 10});

    floorEnt.createComponent(transformComponent);

    meshComponent = {};
    meshComponent.mesh = ResourceHandle<SkinnedMesh>(Uri("meshes/cube.obj"));

    floorEnt.createComponent(meshComponent);

    materialComponent = {};
    materialComponent.materials[0] = ResourceHandle<Material>(Uri(MATERIALS_PATH + std::string("/cube")));

    floorEnt.createComponent(materialComponent);

    auto sphereEnt = scene.createEntity();

    rigidBodyComponent = {};
    rigidBodyComponent.type = xng::RigidBody::DYNAMIC;
    rigidBodyComponent.mass = 100;

    colliderComponent = {};
    colliderComponent.shape.type = COLLIDER_SPHERE;
    colliderComponent.shape.radius = 1;

    sphereEnt.createComponent(rigidBodyComponent);
    sphereEnt.createComponent(colliderComponent);

    sphereEnt.createComponent(SphereControllerComponent());

    transformComponent = {};
    transformComponent.transform.setPosition({0, 5, 0});

    sphereEnt.createComponent(transformComponent);

    meshComponent = {};
    meshComponent.mesh = ResourceHandle<SkinnedMesh>(Uri("meshes/sphere.obj"));

    sphereEnt.createComponent(meshComponent);

    materialComponent = {};
    materialComponent.materials[0] = ResourceHandle<Material>(Uri(MATERIALS_PATH + std::string("/sphere")));

    sphereEnt.createComponent(materialComponent);

    auto skyboxEnt = scene.createEntity();
    SkyboxComponent skyboxComponent;
    skyboxComponent.skybox.texture = ResourceHandle<CubeMap>(Uri("textures/skybox_blue.json"));

    skyboxEnt.createComponent(skyboxComponent);

    auto lightEnt = scene.createEntity();

    DirectionalLight light;
    light.power = 0.8;

    LightComponent lightComponent;
    lightComponent.light = light;

    lightEnt.createComponent(TransformComponent());
    lightEnt.createComponent(lightComponent);

    Transform cameraTransform;
    CameraController cameraController(cameraTransform, input);

    xng::FrameLimiter limiter;
    limiter.reset();
    while (!window->shouldClose()) {
        auto deltaTime = limiter.newFrame();

        auto cameraRef = cameraEnt.getComponent<CameraComponent>();
        cameraRef.camera.aspectRatio = static_cast<float>(window->getWindowSize().x)
                                       / static_cast<float>(window->getWindowSize().y);
        cameraEnt.updateComponent(cameraRef);

        cameraTransform = cameraEnt.getComponent<TransformComponent>().transform;
        cameraController.update(deltaTime);

        TransformComponent tcomp;
        tcomp.transform = cameraTransform;
        cameraEnt.updateComponent(tcomp);

        if (window->getInput().getKeyboard().getKeyDown(xng::KEY_LEFT)) {
            testPass->decrementSlot();
        } else if (window->getInput().getKeyboard().getKeyDown(xng::KEY_RIGHT)) {
            testPass->incrementSlot();
        }

        runtime.update(deltaTime);

        overlay.draw(deltaTime, *target);

        window->swapBuffers();
        window->update();
    }

    return 0;
}