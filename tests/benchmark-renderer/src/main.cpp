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
#include "xng/renderer/renderfont.hpp"
#include "xng/renderer/rendertext.hpp"
#include "xng/renderer/passes/canvaspass.hpp"

using namespace xng;

struct Resources {
    ResourceHandle<Mesh> boxMesh = ResourceHandle<Mesh>(Uri("file://meshes/cornell.fbx:Box"));
    Transform boxTransform{};
    ResourceHandle<Mesh> cubeMesh = ResourceHandle<Mesh>(Uri("file://meshes/cornell.fbx:Cube"));
    Transform cubeTransform{};
    ResourceHandle<Mesh> sphereMesh1 = ResourceHandle<Mesh>(Uri("file://meshes/cornell.fbx:Sphere.001"));
    Transform sphereTransform1{};
    ResourceHandle<Mesh> sphereMesh2 = ResourceHandle<Mesh>(Uri("file://meshes/cornell.fbx:Sphere.002"));
    Transform sphereTransform2{};

    std::shared_ptr<ImageTileLoader> boxAlbedoLoader;

    std::shared_ptr<ImageTileLoader> brickAlbedoLoader;
    std::shared_ptr<ImageTileLoader> brickMetallicLoader;
    std::shared_ptr<ImageTileLoader> brickRoughnessLoader;
    std::shared_ptr<ImageTileLoader> brickAoLoader;
    std::shared_ptr<ImageTileLoader> brickNormalLoader;

    std::shared_ptr<ImageTileLoader> rustedIronAlbedoLoader;
    std::shared_ptr<ImageTileLoader> rustedIronMetallicLoader;
    std::shared_ptr<ImageTileLoader> rustedIronRoughnessLoader;

    std::shared_ptr<ImageTileLoader> goldAlbedoLoader;
    std::shared_ptr<ImageTileLoader> goldMetallicLoader;
    std::shared_ptr<ImageTileLoader> goldRoughnessLoader;

    std::shared_ptr<ImageTileLoader> sphereNormalLoader;

    Transform getTransform(const std::string &name, const AssetScene::Node &node, bool &found) {
        if (node.name == name) {
            found = true;
            return node.transform;
        }
        for (auto &child: node.children) {
            auto childTransform = getTransform(name, child, found);
            if (found) {
                return childTransform.getWorldTransform(node.transform);
            }
        }
        return node.transform;
    }

    Resources(rg::Runtime &runtime) {
        const auto scene = ResourceHandle<AssetScene>(Uri("file://meshes/cornell.fbx:RootNode")).get();
        bool found = false;
        boxTransform = getTransform("Box", scene.root, found);
        found = false;
        cubeTransform = getTransform("Cube", scene.root, found);
        found = false;
        sphereTransform1 = getTransform("Sphere.001", scene.root, found);
        found = false;
        sphereTransform2 = getTransform("Sphere.002", scene.root, found);

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

        constexpr auto tileSize = 256;
        constexpr auto tileBorder = 9;

        boxAlbedoLoader = getLoader(boxAlbedoImage, runtime, tileSize, tileBorder);

        brickAlbedoLoader = getLoader(brickAlbedo, runtime, tileSize, tileBorder);
        brickMetallicLoader = getLoader(brickMetallic, runtime, tileSize, tileBorder);
        brickRoughnessLoader = getLoader(brickRoughness, runtime, tileSize, tileBorder);
        brickAoLoader = getLoader(brickAo, runtime, tileSize, tileBorder);
        brickNormalLoader = getLoader(brickNormal, runtime, tileSize, tileBorder);

        rustedIronAlbedoLoader = getLoader(rustedIronAlbedo, runtime, tileSize, tileBorder);
        rustedIronMetallicLoader = getLoader(rustedIronMetallic, runtime, tileSize, tileBorder);
        rustedIronRoughnessLoader = getLoader(rustedIronRoughness, runtime, tileSize, tileBorder);

        goldAlbedoLoader = getLoader(goldAlbedo, runtime, tileSize, tileBorder);
        goldMetallicLoader = getLoader(goldMetallic, runtime, tileSize, tileBorder);
        goldRoughnessLoader = getLoader(goldRoughness, runtime, tileSize, tileBorder);

        sphereNormalLoader = getLoader(sphereNormal, runtime, tileSize, tileBorder);
    }

private:
    std::shared_ptr<ImageTileLoader> getLoader(const ResourceHandle<ImageRGBA> &image,
                                               rg::Runtime &runtime,
                                               const unsigned int tileSize,
                                               const unsigned int tileBorder) {
        return std::make_shared<ImageTileLoader>(image.get(),
                                                 rg::Texture::calculateMipLevels(
                                                     image.get().getResolution()),
                                                 tileSize,
                                                 tileBorder,
                                                 WRAP_CLAMP_TO_EDGE,
                                                 runtime);
    }
};

void createCornellInstance(RenderScene &scene,
                           Resources &res,
                           Vec3f offset,
                           std::vector<RenderObjectHandle<RenderModel> > &models,
                           std::vector<RenderObjectHandle<RenderPointLight> > &lights) {
    auto boxAlbedo = scene.createTexture(res.boxAlbedoLoader);

    PBRMaterial boxMaterialDef;
    boxMaterialDef.setAlbedo(boxAlbedo, {});

    auto boxMaterial = scene.createMaterial(PBRMaterial({}), RENDER_PATH_DEFERRED);
    auto boxMesh = scene.createMesh(res.boxMesh.get());
    auto boxModel = scene.createModel(boxMaterial,
                                      {boxMesh},
                                      true,
                                      0);

    boxModel->setTransform(res.boxTransform.getWorldTransform(Transform(offset, Vec3f(0), Vec3f(1))));

    models.emplace_back(boxModel);

    auto brickAlbedo = scene.createTexture(res.brickAlbedoLoader);
    auto brickMetallic = scene.createTexture(res.brickMetallicLoader);
    auto brickRoughness = scene.createTexture(res.brickRoughnessLoader);
    auto brickAo = scene.createTexture(res.brickAoLoader);
    auto brickNormal = scene.createTexture(res.brickNormalLoader);

    SamplingProperties brickProps(FILTER_BICUBIC, FILTER_BICUBIC, rg::LINEAR, WRAP_REPEAT);

    PBRMaterial brickMaterialDef;
    brickMaterialDef.setAlbedo(brickAlbedo, brickProps);
    brickMaterialDef.setMetallic(brickMetallic, brickProps);
    brickMaterialDef.setRoughness(brickRoughness, brickProps);
    brickMaterialDef.setAmbientOcclusion(brickAo, brickProps);
    brickMaterialDef.setNormal(brickNormal, brickProps);

    auto brickMaterial = scene.createMaterial(brickMaterialDef, RENDER_PATH_DEFERRED);
    auto brickMesh = scene.createMesh(res.cubeMesh.get());
    auto brickModel = scene.createModel(brickMaterial,
                                        {brickMesh},
                                        true,
                                        0);

    brickModel->setTransform(res.cubeTransform.getWorldTransform(
            Transform(offset, Quaternion(Vec3f(0, 0, 0)), Vec3f(1)))
    );

    models.emplace_back(brickModel);

    auto sphereMesh = scene.createMesh(Mesh::computeSmoothNormals(res.sphereMesh1.get()));
    auto sphereMesh2 = scene.createMesh(res.sphereMesh2.get());

    auto sphereNormal = scene.createTexture(res.sphereNormalLoader);

    auto rustedIronAlbedo = scene.createTexture(res.rustedIronAlbedoLoader);
    auto rustedIronMetallic = scene.createTexture(res.rustedIronMetallicLoader);
    auto rustedIronRoughness = scene.createTexture(res.rustedIronRoughnessLoader);

    PBRMaterial rustedIronMaterialDef;
    rustedIronMaterialDef.setAlbedo(rustedIronAlbedo, {});
    rustedIronMaterialDef.setMetallic(rustedIronMetallic, {});
    rustedIronMaterialDef.setRoughness(rustedIronRoughness, {});

    auto rustedIronSphereMaterial = scene.createMaterial(rustedIronMaterialDef, RENDER_PATH_DEFERRED);
    auto rustedIronSphereModel = scene.createModel(rustedIronSphereMaterial,
                                                   {sphereMesh},
                                                   true,
                                                   0);

    rustedIronSphereModel->setTransform(res.sphereTransform1.getWorldTransform(
        Transform(offset, Quaternion(Vec3f(0, 0, 0)), Vec3f(1))
    ));

    models.emplace_back(rustedIronSphereModel);

    auto goldAlbedo = scene.createTexture(res.goldAlbedoLoader);
    auto goldMetallic = scene.createTexture(res.goldMetallicLoader);
    auto goldRoughness = scene.createTexture(res.goldRoughnessLoader);

    PBRMaterial goldMaterialDef;
    goldMaterialDef.setAlbedo(goldAlbedo, {});
    goldMaterialDef.setMetallic(goldMetallic, {});
    goldMaterialDef.setRoughness(goldRoughness, {});
    goldMaterialDef.setNormal(sphereNormal, {});

    auto goldSphereMaterial = scene.createMaterial(goldMaterialDef, RENDER_PATH_DEFERRED);
    auto goldSphereModel = scene.createModel(goldSphereMaterial,
                                             {sphereMesh2},
                                             true,
                                             0);

    goldSphereModel->setTransform(res.sphereTransform2.getWorldTransform(Transform(offset, Vec3f(0), Vec3f(1))));

    models.emplace_back(goldSphereModel);

    auto pointLight = scene.createPointLight();
    pointLight->set(offset + Vec3f(0, 0.4, 0),
                    ColorRGB(255),
                    2,
                    true,
                    0.1,
                    1000.0f);

    lights.emplace_back(pointLight);
}

void createDrawList(RenderScene &scene,
                    rg::Runtime &runtime,
                    std::vector<RenderObjectHandle<RenderModel> > &models,
                    std::vector<RenderObjectHandle<RenderPointLight> > &lights) {
    std::cout << "Generating Mips..." << std::endl;
    Resources res(runtime);

    std::cout << "Generating Draw List..." << std::endl;
    // scene.hdri = ResourceHandle<ImageRGBF>(Uri("file://hdri/church_stairway_4k.hdr"));

    scene.setCamera(Camera(Transform(Vec3f(0, 0, -2),
                                     Quaternion(Vec3f(0, 0, 0)),
                                     Vec3f(1)),
                           Camera::getPerspectiveProjection(90, 4 / 3, 0.001f, 1000.0f)));
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
    const int rows = 10;
    const int columns = rows;
    const float spacing = 1.5f;
    for (int x = 0; x < columns; x++) {
        for (int y = 0; y < rows; y++) {
            createCornellInstance(scene, res, Vec3f(x * spacing - (columns * spacing / 2),
                                                    y * spacing - (rows * spacing / 2),
                                                    0),
                                  models,
                                  lights);
        }
    }
}

void cameraController(Transform &cameraTransform, Window &window, double deltaTime) {
    constexpr float movementSpeed = 1;

    auto &input = window.getInput();
    auto &mouse = input.getMouse();
    if (mouse.getButton(MOUSE_BUTTON_RIGHT)) {
        constexpr float rotationSpeed = 90;
        auto rot = Vec3d(0, (mouse.positionDelta.x / window.getFramebufferSize().x) * rotationSpeed, 0);
        cameraTransform.rotateLocal(Quaternion(rot.convert<float>()));

        rot = Vec3d((mouse.positionDelta.y / window.getFramebufferSize().y) * rotationSpeed, 0, 0);
        cameraTransform.rotateWorld(Quaternion(rot.convert<float>()));
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

    lightTransform.rotateLocal(Quaternion(Vec3f(rot.x, 0, 0) * rotationSpeed * deltaTime));
    lightTransform.rotateWorld(Quaternion(Vec3f(0, rot.y, 0) * rotationSpeed * deltaTime));

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
    attributes.doubleBuffer = false;

    const std::shared_ptr window = std::move(glfw.createWindow("Renderer Benchmark",
                                                               {1000, 900},
                                                               attributes));
    window->show();

    auto &input = window->getInput();

    const auto &surface = runtime->createSurface(window, 2);

    const auto &tuxImg = tux.get();
    const auto &smileyImg = smiley.get();

    auto freeType = std::make_unique<freetype::FontEngine>();

    auto ren = Renderer(*runtime);

    auto scene = ren.createScene();

    ren.setPasses({
        std::make_shared<ConstructionPass>(scene->getPbrDeferredPipeline().getCompiler()),
        std::make_shared<DeferredPBRPass>(runtime->getResourceHeap(), runtime->getPipelineCache()),
        std::make_shared<CanvasPass>(runtime->getResourceHeap(), runtime->getPipelineCache()),
        std::make_shared<CompositingPass>(runtime->getPipelineCache())
    });

    std::cout << "Loading Assets..." << std::endl;
    ResourceRegistry::getDefaultRegistry().awaitAll();

    std::cout << "Allocating Objects..." << std::endl;

    std::vector<RenderObjectHandle<RenderModel> > models;
    std::vector<RenderObjectHandle<RenderPointLight> > lights;

    createDrawList(*scene, *runtime, models, lights);

    std::cout << "Ready" << std::endl;

    FrameLimiter frameLimiter(0);
    frameLimiter.reset();

    std::chrono::milliseconds fpsUpdateInterval = std::chrono::milliseconds(50);
    auto now = std::chrono::steady_clock::now();

    std::vector<std::unique_ptr<FontRenderer> > fonts;
    fonts.emplace_back(freeType->createFontRenderer(font.get().data));

    const auto fontObject = std::make_shared<RenderFont>(scene,
                                                         std::move(fonts),
                                                         Vec2i(0, 16));

    auto canvas = scene->createCanvas();

   // RenderText textObject(*scene, canvas, fontObject, {}, {}, {}, {});

    RendererStatistics stats;
    while (!window->shouldClose()) {
        frameLimiter.newFrame();
        window->update();

        canvas->setLocalProjection(RenderCanvas::getLocalProjection(0,
                                                                    window->getFramebufferSize().x,
                                                                    0,
                                                                    window->getFramebufferSize().y));

        if (std::chrono::steady_clock::now() - now > fpsUpdateInterval) {
            now = std::chrono::steady_clock::now();

            std::wstring txt = std::to_wstring(frameLimiter.getFramerate())
                               + L" FPS";
            /* if (ren.getStatistics().streamingTiles > 0)
                 txt += L"\nStreaming "
                         + std::to_wstring(ren.getStatistics().streamingTiles)
                         + L" tiles";

             SamplingProperties props;
             props.minFilter = FILTER_NEAREST;
             props.magFilter = FILTER_NEAREST;
             textObject = ren.getAllocator().createPaintText(fontObject,
                                                             std::u32string(txt.begin(), txt.end()),
                                                             {},
                                                             ColorRGBA::lime(),
                                                             props);*/

            // canvas->setPaints({textObject});
        }

        auto fbSize = surface->getDimensions();

        Transform cameraTransform = scene->getCamera().getTransform();
        cameraController(cameraTransform, *window, frameLimiter.getDeltaTimeSeconds());
        scene->setCamera(Camera(cameraTransform,
                                Camera::getPerspectiveProjection(90,
                                                                 static_cast<float>(fbSize.x) / static_cast<float>
                                                                 (fbSize.y),
                                                                 0.001f,
                                                                 1000.0f)));

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
        ren.draw(surface, *scene);
    }

    return 0;
}
