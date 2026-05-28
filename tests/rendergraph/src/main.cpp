/**
 *   xEngine - C++ Game Engine Library
 *   Copyright (C) 2026 Julia Zampiccoli
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the Lesser General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "xng/xng.hpp"

#include "xng/adapters/glfw/glfw.hpp"
#include "xng/adapters/opengl/opengl.hpp"
#include "xng/adapters/freetype/freetype.hpp"
#include "xng/adapters/assimp/assimp.hpp"

using namespace xng;

struct Resources {
    ResourceHandle<Mesh> boxMesh = ResourceHandle<Mesh>(Uri("file://meshes/cornell.fbx:Box"));
    ResourceHandle<Mesh> cubeMesh = ResourceHandle<Mesh>(Uri("file://meshes/cornell.fbx:Cube"));
    ResourceHandle<Mesh> sphereMesh1 = ResourceHandle<Mesh>(Uri("file://meshes/cornell.fbx:Sphere.001"));
    ResourceHandle<Mesh> sphereMesh2 = ResourceHandle<Mesh>(Uri("file://meshes/cornell.fbx:Sphere.002"));

    ResourceHandle<ImageRGBA> boxAlbedoImage = ResourceHandle<ImageRGBA>(Uri("file://images/cornell_boxcolor.png"));

    ResourceHandle<ImageRGBA> brickAlbedo = ResourceHandle<ImageRGBA>(
        Uri("file://images/subway_brick/old-subway-brick_albedo.png"));
    ResourceHandle<ImageRGBA> brickMetallic = ResourceHandle<ImageRGBA>(
        Uri("file://images/subway_brick/old-subway-brick_metallic.png"));
    ResourceHandle<ImageRGBA> brickRoughness = ResourceHandle<ImageRGBA>(
        Uri("file://images/subway_brick/old-subway-brick_roughness.png"));
    ResourceHandle<ImageRGBA> brickAo = ResourceHandle<ImageRGBA>(
        Uri("file://images/subway_brick/old-subway-brick_ao.png"));
    ResourceHandle<ImageRGBA> brickNormal = ResourceHandle<ImageRGBA>(
        Uri("file://images/subway_brick/old-subway-brick_normal-ogl.png"));

    ResourceHandle<ImageRGBA> rustedIronAlbedo = ResourceHandle<ImageRGBA>(
        Uri("file://images/rusted_iron/rustediron2_basecolor.png"));
    ResourceHandle<ImageRGBA> rustedIronMetallic = ResourceHandle<ImageRGBA>(
        Uri("file://images/rusted_iron/rustediron2_metallic.png"));
    ResourceHandle<ImageRGBA> rustedIronRoughness = ResourceHandle<ImageRGBA>(
        Uri("file://images/rusted_iron/rustediron2_roughness.png"));

    ResourceHandle<ImageRGBA> goldAlbedo = ResourceHandle<ImageRGBA>(
        Uri("file://images/lightgold/lightgold_albedo.png"));
    ResourceHandle<ImageRGBA> goldMetallic = ResourceHandle<ImageRGBA>(
        Uri("file://images/lightgold/lightgold_metallic.png"));
    ResourceHandle<ImageRGBA> goldRoughness = ResourceHandle<ImageRGBA>(
        Uri("file://images/lightgold/lightgold_roughness.png"));

    ResourceHandle<ImageRGBA> sphereNormal = ResourceHandle<ImageRGBA>(Uri("file://images/sphere_normals.png"));
};

void createCornellInstance(RenderAllocator &allocator, Resources &res, RenderDrawList &drawList, Vec3f offset) {
    auto boxAlbedo = allocator.createTexture(res.boxAlbedoImage.get().getResolution());
    boxAlbedo->setImage(res.boxAlbedoImage.get());

    auto boxMaterial = allocator.createMaterial({},
                                                1,
                                                1,
                                                1,
                                                Vec4f(1),
                                                boxAlbedo,
                                                {},
                                                {},
                                                {},
                                                {});

    auto boxMesh = allocator.createMesh(res.boxMesh.get(), {});

    auto boxModel = allocator.createModel({boxMesh},
                                          boxMaterial,
                                          RENDER_PATH_DEFERRED,
                                          SHADING_MODEL_PBR,
                                          true,
                                          true);

    boxModel->setTransform(Transform(offset, Vec3f(0), Vec3f(1)));

    drawList.models.emplace_back(boxModel);

    auto brickAlbedo = allocator.createTexture(res.brickAlbedo.get().getResolution());
    auto brickMetallic = allocator.createTexture(res.brickMetallic.get().getResolution());
    auto brickRoughness = allocator.createTexture(res.brickRoughness.get().getResolution());
    auto brickAo = allocator.createTexture(res.brickAo.get().getResolution());
    auto brickNormal = allocator.createTexture(res.brickNormal.get().getResolution());

    brickAlbedo->setImage(res.brickAlbedo.get());
    brickMetallic->setImage(res.brickMetallic.get());
    brickRoughness->setImage(res.brickRoughness.get());
    brickAo->setImage(res.brickAo.get());
    brickNormal->setImage(res.brickNormal.get());

    auto brickMaterial = allocator.createMaterial({},
                                                  1,
                                                  1,
                                                  1,
                                                  Vec4f(1),
                                                  brickAlbedo,
                                                  brickMetallic,
                                                  brickRoughness,
                                                  brickAo,
                                                  brickNormal);

    auto brickMesh = allocator.createMesh(res.cubeMesh.get(), {});

    auto brickModel = allocator.createModel({brickMesh},
                                            brickMaterial,
                                            RENDER_PATH_DEFERRED,
                                            SHADING_MODEL_PBR,
                                            true,
                                            true);

    brickModel->setTransform(Transform(offset, Quaternion(Vec3f(0, 0, 0)), Vec3f(1)));

    drawList.models.emplace_back(brickModel);

    auto sphereMesh = allocator.createMesh(res.sphereMesh1.get(), {});

    auto sphereNormal = allocator.createTexture(res.sphereNormal.get().getResolution());
    sphereNormal->setImage(res.sphereNormal.get());

    auto rustedIronAlbedo = allocator.createTexture(res.rustedIronAlbedo.get().getResolution());
    auto rustedIronMetallic = allocator.createTexture(res.rustedIronMetallic.get().getResolution());
    auto rustedIronRoughness = allocator.createTexture(res.rustedIronRoughness.get().getResolution());
    rustedIronAlbedo->setImage(res.rustedIronAlbedo.get());
    rustedIronMetallic->setImage(res.rustedIronMetallic.get());
    rustedIronRoughness->setImage(res.rustedIronRoughness.get());

    auto rustedIronSphereMaterial = allocator.createMaterial({},
                                                             0,
                                                             0,
                                                             0,
                                                             Vec4f(1),
                                                             rustedIronAlbedo,
                                                             rustedIronMetallic,
                                                             rustedIronRoughness,
                                                             {},
                                                             sphereNormal);

    auto rustedIronSphereModel = allocator.createModel({sphereMesh},
                                                       rustedIronSphereMaterial,
                                                       RENDER_PATH_DEFERRED,
                                                       SHADING_MODEL_PBR,
                                                       true,
                                                       true);

    rustedIronSphereModel->setTransform(Transform(offset, Quaternion(Vec3f(0, 0, 0)), Vec3f(1)));

    drawList.models.emplace_back(rustedIronSphereModel);

    auto goldAlbedo = allocator.createTexture(res.goldAlbedo.get().getResolution());
    auto goldMetallic = allocator.createTexture(res.goldMetallic.get().getResolution());
    auto goldRoughness = allocator.createTexture(res.goldRoughness.get().getResolution());
    goldAlbedo->setImage(res.goldAlbedo.get());
    goldMetallic->setImage(res.goldMetallic.get());
    goldRoughness->setImage(res.goldRoughness.get());

    auto goldSphereMaterial = allocator.createMaterial({},
                                                       0,
                                                       0,
                                                       0,
                                                       Vec4f(1),
                                                       goldAlbedo,
                                                       goldMetallic,
                                                       goldRoughness,
                                                       {},
                                                       sphereNormal);

    auto goldSphereModel = allocator.createModel({sphereMesh},
                                                 goldSphereMaterial,
                                                 RENDER_PATH_DEFERRED,
                                                 SHADING_MODEL_PBR,
                                                 true,
                                                 true);

    goldSphereModel->setTransform(Transform(offset, Vec3f(0), Vec3f(1)));

    drawList.models.emplace_back(goldSphereModel);

    auto pointLight = allocator.createPointLight();
    pointLight->set(offset + Vec3f(0, 0.4, 0),
                    ColorRGB(255),
                    2,
                    true,
                    0.1,
                    1000.0f);

    drawList.pointLights.emplace_back(pointLight);
}

RenderDrawList createDrawList(RenderAllocator &allocator) {
    Resources res;
    RenderDrawList ret;

    // scene.hdri = ResourceHandle<ImageRGBF>(Uri("file://hdri/church_stairway_4k.hdr"));
    ret.camera.setTransform(Transform(Vec3f(0, 0, -5),
                                      Quaternion(Vec3f(0, 0, 0)),
                                      Vec3f(1)));
    ret.camera.setProjection(Camera::getPerspectiveProjection(90, 4 / 3, 0.001f, 1000.0f));
    /*
        SpotLightObject spotLight;
        spotLight.light.castShadows = true;
        spotLight.light.power = 15;
        spotLight.light.outerCutOff = 20;
        spotLight.light.color = ColorRGBA::aqua();
        spotLight.transform.setPosition(Vec3f(0, 0, 0));
        spotLight.transform.setRotation(Quaternion(Vec3f(0, 0, 0)));
        scene.spotLights.emplace_back(spotLight);

        DirectionalLightObject dirLight;
        dirLight.light.power = 0.5f;
        dirLight.light.castShadows = true;
        dirLight.transform.setRotation(Quaternion(Vec3f(45, 45, 0)));
        scene.directionalLights.emplace_back(dirLight);

        SkinnedModelObject mesh;
        mesh.castShadows = true;
        mesh.model = ResourceHandle<SkinnedModel>(Uri("file://meshes/sphere.obj"));
        mesh.transform.setPosition({});
        mesh.transform.setScale(Vec3f(0.05f));
        scene.skinnedModels.push_back(mesh);
    */
    const int rows = 3;
    const int columns = rows;
    const float spacing = 1.5f;
    for (int x = 0; x < columns; x++) {
        for (int y = 0; y < rows; y++) {
            createCornellInstance(allocator, res, ret, Vec3f(x * spacing - (columns * spacing / 2),
                                                             y * spacing - (rows * spacing / 2),
                                                             0));
        }
    }
    return ret;
}

void cameraController(Transform &cameraTransform, Window &window, double deltaTime) {
    constexpr float movementSpeed = 1;

    auto &input = window.getInput();
    auto &mouse = input.getMouse();
    if (mouse.getButton(MOUSE_BUTTON_RIGHT)) {
        constexpr float rotationSpeed = 90;
        auto rot = Vec3d(0, (mouse.positionDelta.x / window.getFramebufferSize().x) * rotationSpeed, 0);
        cameraTransform.applyRotation(Quaternion(rot.convert<float>()), true);

        rot = Vec3d((mouse.positionDelta.y / window.getFramebufferSize().y) * rotationSpeed, 0, 0);
        cameraTransform.applyRotation(Quaternion(rot.convert<float>()), false);
    }

    Vec3f movement{};
    if (input.getKey(KEY_W)) {
        movement.y = 1;
    } else if (input.getKey(KEY_S)) {
        movement.y = -1;
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

void lightController(Transform &lightTransform, Window &window, double deltaTime) {
    constexpr float movementSpeed = 0.5;
    constexpr float rotationSpeed = 35;
    auto &input = window.getInput();

    Vec3f pos;
    Vec3f rot;

    if (input.getKey(KEY_LSHIFT)) {
        if (input.getKey(KEY_UP)) {
            pos.z = 1;
        } else if (input.getKey(KEY_DOWN)) {
            pos.z = -1;
        }
        if (input.getKey(KEY_LEFT)) {
            pos.x = -1;
        } else if (input.getKey(KEY_RIGHT)) {
            pos.x = 1;
        }
    } else {
        if (input.getKey(KEY_UP)) {
            rot.x = -1;
        } else if (input.getKey(KEY_DOWN)) {
            rot.x = 1;
        }
        if (input.getKey(KEY_LEFT)) {
            rot.y = -1;
        } else if (input.getKey(KEY_RIGHT)) {
            rot.y = 1;
        }
    }

    if (input.getKey(KEY_PAGEUP)) {
        pos.y = 1;
    } else if (input.getKey(KEY_PAGEDOWN)) {
        pos.y = -1;
    }

    lightTransform.applyRotation(Quaternion(Vec3f(rot.x, 0, 0) * rotationSpeed * deltaTime), true);
    lightTransform.applyRotation(Quaternion(Vec3f(0, rot.y, 0) * rotationSpeed * deltaTime), false);

    lightTransform.setPosition(lightTransform.getPosition() + pos * movementSpeed * deltaTime);
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
    auto runtime = std::make_shared<opengl::Runtime>(glfw);

    WindowAttributes attributes;
    attributes.visible = true;

    const std::shared_ptr window = std::move(glfw.createWindow("RenderGraph Test",
                                                               {1000, 900},
                                                               attributes));
    window->show();

    auto &input = window->getInput();

    const auto &surface = runtime->createSurface(window, 2);

    const auto &tuxImg = tux.get();
    const auto &smileyImg = smiley.get();

    auto freeType = std::make_unique<freetype::FontEngine>();

    auto ren = Renderer(*runtime);

    ren.setPasses({
        std::make_shared<CompositingPass>(),
        std::make_shared<ConstructionPass>(runtime->getPipelineCache())
    });

    printf("Loading Assets...\n");
    ResourceRegistry::getDefaultRegistry().awaitAll();

    auto drawList = createDrawList(ren.getAllocator());

    printf("Ready\n");

    FrameLimiter frameLimiter(0);
    frameLimiter.reset();

    /*auto textLayoutEngine = TextLayoutEngine(*freeType, font, {0, 30});
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
                                                     });*/
    std::chrono::milliseconds fpsUpdateInterval = std::chrono::milliseconds(50);
    auto now = std::chrono::steady_clock::now();

    RendererStatistics stats;
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
                (stats.bufferVRamUsage + stats.textureVRamUsage) /
                MEGABYTE) + " MB\n";
            txt += "Buffers " + std::to_string(stats.bufferVRamUsage / KILOBYTE) + " KB\n";
            txt += "Textures " + std::to_string(stats.textureVRamUsage / KILOBYTE) + " KB\n\n";
            txt += "VRAM Copy\n";
            txt += "Buffers " + std::to_string(stats.bufferVRamCopy / KILOBYTE) + " KB\n";
            txt += "Textures " + std::to_string(stats.textureVRamCopy / KILOBYTE) + " KB\n\n";
            txt += "VRAM Upload\n";
            txt += "Buffers " + std::to_string(stats.bufferVRamUpload / KILOBYTE) + " KB\n";
            txt += "Texture Buffers " + std::to_string(stats.textureVRamUpload / KILOBYTE) + " KB\n";
            /*    deltaText = deltaTextLayoutEngine.getLayout(txt,
                                                            {
                                                                0,
                                                                0,
                                                                TEXT_ALIGN_RIGHT
                                                            });*/
        }

        auto fbSize = surface->getDimensions();

        drawList.camera.setProjection(Camera::getPerspectiveProjection(90,
                                                                       static_cast<float>(fbSize.x) / static_cast<float>
                                                                       (fbSize.y),
                                                                       0.001f,
                                                                       1000.0f));
        Transform cameraTransform = drawList.camera.getTransform();
        cameraController(cameraTransform, *window, frameLimiter.getDeltaTimeSeconds());
        drawList.camera.setTransform(cameraTransform);
        /*
        lightController(scene.spotLights.at(0).transform, *window, frameLimiter.getDeltaTimeSeconds());
        scene.skinnedModels.at(0).transform.setPosition(scene.spotLights.at(0).transform.getPosition());
        scene.skinnedModels.at(0).transform.setRotation(scene.spotLights.at(0).transform.getRotation());
        config->setScene(scene);*/
        /*
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
        */
        /*       if (input.getKey(KEY_F1)) {
                   config->setGamma(config->getGamma() - 0.75 * frameLimiter.getDeltaTimeSeconds());
               } else if (input.getKey(KEY_F2)) {
                   config->setGamma(config->getGamma() + 0.75 * frameLimiter.getDeltaTimeSeconds());
               }

               if (input.getKey(KEY_F3)) {
                   config->setRenderScale(config->getRenderScale() - 0.1 * frameLimiter.getDeltaTimeSeconds());
               } else if (input.getKey(KEY_F4)) {
                   config->setRenderScale(config->getRenderScale() + 0.1 * frameLimiter.getDeltaTimeSeconds());
               }

               stats = passScheduler->execute(graph3D);*/
        ren.draw(surface, drawList);
    }

    return 0;
}
