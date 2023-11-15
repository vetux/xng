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

#include "debugoverlay.hpp"
#include "cameracontroller.hpp"

int main(int argc, char *argv[]) {
    std::vector<std::unique_ptr<ResourceParser>> parsers;
    parsers.emplace_back(std::make_unique<StbiParser>());
    parsers.emplace_back(std::make_unique<AssImpParser>());
    parsers.emplace_back(std::make_unique<JsonParser>());

    xng::ResourceRegistry::getDefaultRegistry().setImporter(
            ResourceImporter(std::move(parsers)));

    xng::ResourceRegistry::getDefaultRegistry().addArchive("file", std::make_shared<DirectoryArchive>("assets/"));

    auto displayDriver = glfw::GLFWDisplayDriver();
    auto gpuDriver = opengl::OGLGpuDriver();
    auto shaderCompiler = glslang::GLSLangCompiler();
    auto shaderDecompiler = spirv_cross::SpirvCrossDecompiler();
    auto fontDriver = freetype::FtFontDriver();

    auto fontFs = std::ifstream("assets/fonts/Sono/static/Sono/Sono-Regular.ttf", std::ios_base::in | std::ios::binary);
    auto font = fontDriver.createFont(fontFs);

    auto window = displayDriver.createWindow(OPENGL_4_6,
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

    xng::FrameGraphRenderer renderer(*target,
                                     *device,
                                     std::make_unique<xng::FrameGraphPoolAllocator>(*device,
                                                                                    shaderCompiler,
                                                                                    shaderDecompiler,
                                                                                    *target),
                                     shaderCompiler,
                                     shaderDecompiler);

    FrameGraphPipeline pipeline = FrameGraphPipeline()
            .addPass(std::make_shared<ClearPass>())
            .addPass(std::make_shared<ConstructionPass>())
            .addPass(std::make_shared<DeferredLightingPass>())
            .addPass(std::make_shared<ForwardLightingPass>())
            .addPass(std::make_shared<CompositePass>())
            .addPass(std::make_shared<PresentationPass>());

    renderer.setPipeline(pipeline);

    xng::Scene scene;

    xng::Node node;

    node.addProperty(TransformProperty());

    SkinnedMeshProperty meshProperty;
    meshProperty.mesh = ResourceHandle<SkinnedMesh>(Uri("meshes/animtest.fbx"));
    node.addProperty(meshProperty);

    node.addProperty(BoneTransformsProperty());

    scene.rootNode.childNodes.emplace_back(node);

    auto &boneTransformsProperty = scene.rootNode.childNodes.at(0).getProperty<BoneTransformsProperty>();

    auto meshHandle = ResourceHandle<SkinnedMesh>(Uri("meshes/animtest.fbx"));
    RigAnimator rigAnimator(meshHandle.get().rig);

    auto animA = ResourceHandle<RigAnimation>(Uri("meshes/animtest.fbx/Armature|LeftAnimation"));
    auto animB = ResourceHandle<RigAnimation>(Uri("meshes/animtest.fbx/Armature|RightAnimation"));
    rigAnimator.start(animA.get());
    rigAnimator.start(animB.get(), {}, true, 1);

    node = {};
    CameraProperty cameraProperty;
    cameraProperty.camera.type = xng::PERSPECTIVE;
    node.addProperty(cameraProperty);

    auto transformProp = TransformProperty();
    transformProp.transform.setPosition({0, 0, 10});
    node.addProperty(transformProp);

    scene.rootNode.childNodes.emplace_back(node);

    Node lightNode;
    TransformProperty lightTransform;
    lightTransform.transform.setPosition({0, 5, 5});
    lightNode.addProperty(lightTransform);
    PointLightProperty light;
    light.light.power = 100;
    lightNode.addProperty(light);
    scene.rootNode.childNodes.emplace_back(lightNode);

    CameraController cameraController(scene.rootNode.find<CameraProperty>().getProperty<TransformProperty>().transform, input);

    auto &prop = scene.rootNode.findAll({typeid(SkinnedMeshProperty)}).at(
            0).getProperty<TransformProperty>();
    xng::FrameLimiter limiter;
    limiter.reset();
    while (!window->shouldClose()) {
        auto deltaTime = limiter.newFrame();

        rigAnimator.update(deltaTime);
        boneTransformsProperty.boneTransforms = rigAnimator.getBoneTransforms();

        scene.rootNode.find<CameraProperty>().getProperty<CameraProperty>().camera.aspectRatio =
                static_cast<float>(window->getWindowSize().x)
                / static_cast<float>(window->getWindowSize().y);

        cameraController.update(deltaTime);

        renderer.render(scene);

        overlay.draw(deltaTime, *target);

        if (input.getKeyboard().getKey(KEY_V)) {
            prop.transform.setRotation(prop.transform.getRotation() * Quaternion(Vec3f(0, 20 * deltaTime, 0)));
        } else if (input.getKeyboard().getKey(KEY_C)) {
            prop.transform.setRotation(prop.transform.getRotation() * Quaternion(Vec3f(0, -20 * deltaTime, 0)));
        }

        window->swapBuffers();
        window->update();
    }

    return 0;
}