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

#include "debugoverlay.hpp"
#include "cameracontroller.hpp"

int main(int argc, char *argv[]) {
    std::vector<std::unique_ptr<ResourceImporter>> importers;
    importers.emplace_back(std::make_unique<StbiImporter>());
    importers.emplace_back(std::make_unique<AssImp>());
    importers.emplace_back(std::make_unique<JsonImporter>());

    xng::ResourceRegistry::getDefaultRegistry().setImporters(std::move(importers));

    xng::ResourceRegistry::getDefaultRegistry().addArchive("file", std::make_shared<DirectoryArchive>("assets/"));

    auto displayDriver = glfw::GLFW();
    auto gpuDriver = opengl::OpenGL();
    auto shaderCompiler = glslang::GLSLang();
    auto shaderDecompiler = spirv_cross::SpirvCross();
    auto fontDriver = freetype::FreeType();

    auto fontFs = std::ifstream("assets/fonts/Sono/static/Sono/Sono-Regular.ttf", std::ios_base::in | std::ios::binary);
    auto font = fontDriver.createFontRenderer(fontFs);

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

    xng::FrameGraphRenderer renderer(std::make_unique<xng::FrameGraphRuntimeSimple>(*target,
                                                                                    *device,
                                                                                    shaderCompiler,
                                                                                    shaderDecompiler));

    FrameGraphPipeline pipeline = FrameGraphPipeline()
            .addPass(std::make_shared<ClearPass>())
            .addPass(std::make_shared<WireframePass>())
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

        if (input.getKey(KEY_V)) {
            prop.transform.setRotation(prop.transform.getRotation() * Quaternion(Vec3f(0, 20 * deltaTime, 0)));
        } else if (input.getKey(KEY_C)) {
            prop.transform.setRotation(prop.transform.getRotation() * Quaternion(Vec3f(0, -20 * deltaTime, 0)));
        }

        window->swapBuffers();
        window->update();
    }

    return 0;
}