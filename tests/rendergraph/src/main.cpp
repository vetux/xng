/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
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

#include "xng/xng.hpp"

#include "xng/adapters/glfw/glfw.hpp"
#include "xng/adapters/opengl/opengl.hpp"
#include "xng/adapters/freetype/freetype.hpp"
#include "xng/adapters/assimp/assimp.hpp"

using namespace xng;

RenderScene createScene() {
    RenderScene scene;

    Material material;
    SkinnedMeshObject mesh;
    mesh.castShadows = true;

    material = {};
    material.roughness = 1;
    material.albedoTexture = ResourceHandle<Texture>(Uri("file://images/cornell_boxcolor.png"));
    mesh.mesh = ResourceHandle<SkinnedMesh>(Uri("file://meshes/cornell.fbx:Box"));
    mesh.materials[0] = material;
    scene.skinnedMeshes.push_back(mesh);

    material = {};
    material.ambientOcclusionTexture = ResourceHandle<Texture>(
        Uri("file://images/subway_brick/old-subway-brick_ao.png"));
    material.metallicTexture = ResourceHandle<Texture>(Uri("file://images/subway_brick/old-subway-brick_metallic.png"));
    material.roughnessTexture = ResourceHandle<Texture>(
        Uri("file://images/subway_brick/old-subway-brick_roughness.png"));
    material.albedoTexture = ResourceHandle<Texture>(Uri("file://images/subway_brick/old-subway-brick_albedo.png"));
    material.normal = ResourceHandle<Texture>(Uri("file://images/subway_brick/old-subway-brick_normal-ogl.png"));
    mesh.mesh = ResourceHandle<SkinnedMesh>(Uri("file://meshes/cornell.fbx:Cube"));
    mesh.materials[0] = material;
    scene.skinnedMeshes.push_back(mesh);

    material = {};
    material.normal = ResourceHandle<Texture>(Uri("file://images/sphere_normals.png"));
    material.metallicTexture = ResourceHandle<Texture>(Uri("file://images/rusted_iron/rustediron2_metallic.png"));
    material.roughnessTexture = ResourceHandle<Texture>(Uri("file://images/rusted_iron/rustediron2_roughness.png"));
    material.albedoTexture = ResourceHandle<Texture>(Uri("file://images/rusted_iron/rustediron2_basecolor.png"));
    mesh.mesh = ResourceHandle<SkinnedMesh>(Uri("file://meshes/cornell.fbx:Sphere.001"));
    mesh.materials[0] = material;
    scene.skinnedMeshes.push_back(mesh);

    material = {};
    material.normal = ResourceHandle<Texture>(Uri("file://images/sphere_normals.png"));
    material.metallicTexture = ResourceHandle<Texture>(Uri("file://images/rusted_iron/rustediron2_metallic.png"));
    material.roughnessTexture = ResourceHandle<Texture>(Uri("file://images/rusted_iron/rustediron2_roughness.png"));
    material.albedoTexture = ResourceHandle<Texture>(Uri("file://images/rusted_iron/rustediron2_basecolor.png"));
    material.metallic = 0;
    material.roughness = 0.5;
    material.transparent = false;
    material.albedo = ColorRGBA::white(1, 255);
    mesh.mesh = ResourceHandle<SkinnedMesh>(Uri("file://meshes/cornell.fbx:Sphere.002"));
    mesh.materials[0] = material;
    scene.skinnedMeshes.push_back(mesh);

    PointLightObject light;
    light.light.power = 2;
    light.light.castShadows = true;
    light.transform.setPosition(Vec3f(0, 0.45, 0));
    scene.pointLights.emplace_back(light);

    DirectionalLightObject dirLight;
    dirLight.transform.setRotation(Quaternion(Vec3f(45, -45, 0)));
    //scene.directionalLights.emplace_back(dirLight);

    SpotLightObject spotLight;
    spotLight.transform.setPosition(Vec3f(1, 0, -2));
    spotLight.transform.setRotation(Quaternion(Vec3f(0, 25, 0)));
    //scene.spotLights.emplace_back(spotLight);

    scene.cameraTransform.setRotation(Quaternion(Vec3f(0, 180, 0)));
    scene.cameraTransform.setPosition(Vec3f(0, 0, -2));

    return scene;
}

void cameraController(Transform &cameraTransform, Window &window, double deltaTime) {
    constexpr float movementSpeed = 1;

    auto &input = window.getInput();
    auto &mouse = input.getMouse();
    if (mouse.getButton(MOUSE_BUTTON_RIGHT)) {
        constexpr float rotationSpeed = 90;
        auto rot = Vec3d(0, -(mouse.positionDelta.x / window.getFramebufferSize().x) * rotationSpeed, 0);
        cameraTransform.applyRotation(Quaternion(rot.convert<float>()), true);

        rot = Vec3d(-(mouse.positionDelta.y / window.getFramebufferSize().y) * rotationSpeed, 0, 0);
        cameraTransform.applyRotation(Quaternion(rot.convert<float>()), false);
    }

    Vec3f movement{};
    if (input.getKey(KEY_W)) {
        movement.y = -1;
    } else if (input.getKey(KEY_S)) {
        movement.y = 1;
    }
    if (input.getKey(KEY_A)) {
        movement.x = 1;
    } else if (input.getKey(KEY_D)) {
        movement.x = -1;
    }
    if (input.getKey(KEY_SPACE)) {
        movement.z = 1;
    } else if (input.getKey(KEY_LCTRL)) {
        movement.z = -1;
    }

    cameraTransform.setPosition(cameraTransform.getPosition()
                                + cameraTransform.forward() * (movement.y * movementSpeed * deltaTime));
    cameraTransform.setPosition(cameraTransform.getPosition()
                                + cameraTransform.left() * (movement.x * movementSpeed * deltaTime));
    cameraTransform.setPosition(cameraTransform.getPosition()
                                + Vec3f(0, 1, 0) * (movement.z * movementSpeed * deltaTime));
}

int main(int argc, char *argv[]) {
    std::vector<std::unique_ptr<ResourceImporter> > importers;
    importers.emplace_back(std::make_unique<FontImporter>());
    importers.emplace_back(std::make_unique<StbiImporter>());
    importers.emplace_back(std::make_unique<assimp::ResourceImporter>());
    ResourceRegistry::getDefaultRegistry().setImporters(std::move(importers));

    ResourceRegistry::getDefaultRegistry().addArchive("file", std::make_shared<DirectoryArchive>("assets/"));

    auto tux = ResourceHandle<ImageRGBA>(Uri("file://images/tux.png"));
    auto smiley = ResourceHandle<ImageRGBA>(Uri("file://images/awesomeface.png"));
    auto font = ResourceHandle<Font>(Uri("file://fonts/Sono/static/Sono/Sono-Bold.ttf"));

    auto glfw = glfw::DisplayEnvironment();
    auto runtime = std::make_shared<opengl::RenderGraphRuntime>();

    const std::shared_ptr window = std::move(glfw.createWindow(runtime->getGraphicsAPI()));
    auto &input = window->getInput();

    window->setWindowSize({1000, 900});

    runtime->setWindow(window);

    const auto &tuxImg = tux.get();
    const auto &smileyImg = smiley.get();

    auto freeType = std::make_unique<freetype::FontEngine>();

    auto config = std::make_shared<RenderConfiguration>();

    auto scene = createScene();
    config->setScene(scene);

    auto registry = std::make_shared<SharedResourceRegistry>();

    auto passScheduler = std::make_shared<RenderPassScheduler>(runtime);

    auto graph3D = passScheduler->addGraph({
        std::make_shared<ConstructionPass>(config, registry),
        std::make_shared<ShadowMappingPass>(config, registry),
        std::make_shared<DeferredLightingPass>(config, registry),
        std::make_shared<ForwardLightingPass>(config, registry),
        std::make_shared<CanvasRenderPass>(config, registry),
        std::make_shared<CompositingPass>(config, registry),
    });

    FrameLimiter frameLimiter(0);
    frameLimiter.reset();

    auto textLayoutEngine = TextLayoutEngine(*freeType, font, {0, 30});
    auto text = textLayoutEngine.getLayout(
        "Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet.",
        {
            500,
            0,
            TEXT_ALIGN_LEFT,
        });

    auto deltaTextLayoutEngine = TextLayoutEngine(*freeType, font, {0, 20});

    auto deltaText = deltaTextLayoutEngine.getLayout(std::to_string(0) + " FPS",
                                                     {
                                                         0,
                                                         0,
                                                         TEXT_ALIGN_LEFT
                                                     });
    std::chrono::milliseconds fpsUpdateInterval = std::chrono::milliseconds(50);
    auto now = std::chrono::steady_clock::now();

    RenderGraphStatistics stats;
    while (!window->shouldClose()) {
        frameLimiter.newFrame();
        window->update();

        if (std::chrono::steady_clock::now() - now > fpsUpdateInterval) {
            now = std::chrono::steady_clock::now();

            auto txt = std::to_string(frameLimiter.getFramerate()) + " FPS\n\n";
            txt += std::to_string(stats.drawCalls) + " draw calls\n";
            txt += std::to_string(stats.polygons) + " polygons\n\n";
            txt += "VRAM Usage\n";
            txt += "Total " + std::to_string(
                (stats.vertexVRamUsage + stats.indexVRamUsage + stats.shaderBufferVRamUsage + stats.textureVRamUsage) /
                MEGABYTE) + " MB\n";
            txt += "Vertex Buffers " + std::to_string(stats.vertexVRamUsage / KILOBYTE) + " KB\n";
            txt += "Index Buffers " + std::to_string(stats.indexVRamUsage / KILOBYTE) + " KB\n";
            txt += "Shader Storage Buffers " + std::to_string(stats.shaderBufferVRamUsage / KILOBYTE) + " KB\n";
            txt += "Texture Buffers " + std::to_string(stats.textureVRamUsage / KILOBYTE) + " KB\n\n";
            txt += "VRAM Copy\n";
            txt += "Vertex Buffers " + std::to_string(stats.vertexVRamCopy / KILOBYTE) + " KB\n";
            txt += "Index Buffers " + std::to_string(stats.indexVRamCopy / KILOBYTE) + " KB\n";
            txt += "Shader Storage Buffers " + std::to_string(stats.shaderBufferVRamCopy / KILOBYTE) + " KB\n";
            txt += "Texture Buffers " + std::to_string(stats.textureVRamCopy / KILOBYTE) + " KB\n\n";
            txt += "VRAM Upload\n";
            txt += "Vertex Buffers " + std::to_string(stats.vertexVRamUpload / KILOBYTE) + " KB\n";
            txt += "Index Buffers " + std::to_string(stats.indexVRamUpload / KILOBYTE) + " KB\n";
            txt += "Shader Storage Buffers " + std::to_string(stats.shaderBufferVRamUpload / KILOBYTE) + " KB\n";
            txt += "Texture Buffers " + std::to_string(stats.textureVRamUpload / KILOBYTE) + " KB\n";
            deltaText = deltaTextLayoutEngine.getLayout(txt,
                                                        {
                                                            0,
                                                            0,
                                                            TEXT_ALIGN_RIGHT
                                                        });
        }

        auto fbSize = passScheduler->updateBackBuffer();

        scene.camera.aspectRatio = static_cast<float>(fbSize.x)
                                   / static_cast<float>(fbSize.y);
        cameraController(scene.cameraTransform, *window, frameLimiter.getDeltaTimeSeconds());
        config->setScene(scene);

        auto fbSizeF = fbSize.convert<float>();

        Canvas canvas(fbSize);
        canvas.setBackgroundColor(ColorRGBA::black(1, 0));
        canvas.paint(PaintImage(Rectf({}, smileyImg.getResolution().convert<float>()),
                                Rectf({}, smileyImg.getResolution().convert<float>()),
                                smiley,
                                true));
        canvas.paint(PaintImage(Rectf({}, tuxImg.getResolution().convert<float>()),
                                Rectf({}, tuxImg.getResolution().convert<float>()),
                                tux,
                                true));
        canvas.paint(PaintText({15, 10}, text, ColorRGBA::purple()));
        canvas.paint(PaintText(Vec2f(fbSizeF.x - static_cast<float>(deltaText.size.x) - 3, 0), deltaText,
                               ColorRGBA::black()));

        config->setCanvases({canvas});

        if (input.getKey(KEY_F1)) {
            config->setGamma(config->getGamma() - 0.75 * frameLimiter.getDeltaTimeSeconds());
        } else if (input.getKey(KEY_F2)) {
            config->setGamma(config->getGamma() + 0.75 * frameLimiter.getDeltaTimeSeconds());
        }

        if (input.getKey(KEY_F3)) {
            config->setRenderScale(config->getRenderScale() - 0.1 * frameLimiter.getDeltaTimeSeconds());
        } else if (input.getKey(KEY_F4)) {
            config->setRenderScale(config->getRenderScale() + 0.1 * frameLimiter.getDeltaTimeSeconds());
        }

        stats = passScheduler->execute(graph3D);
    }

    return 0;
}
