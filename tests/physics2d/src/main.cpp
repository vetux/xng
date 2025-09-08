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
#include "xng/adapters/opengl/opengl.hpp"
#include "xng/adapters/glslang/glslang.hpp"
#include "xng/adapters/spirv-cross/spirvcross.hpp"
#include "xng/adapters/freetype/freetype.hpp"
#include "xng/adapters/assimp/assimp.hpp"
#include "xng/adapters/box2d/box2d.hpp"

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
    auto gpuDriver = opengl::OpenGL();
    auto shaderCompiler = glslang::GLSLang();
    auto shaderDecompiler = spirv_cross::SpirvCross();
    auto fontDriver = freetype::FreeType();
    auto physicsDriver = box2d::Box2D();

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

    xng::Renderer2D ren(*device, shaderCompiler, shaderDecompiler);

    auto testPass = std::make_shared<DebugPass>();

    world->setGravity({0, -10, 0});

    auto ifs = std::ifstream("assets/images/tux.png");
    auto bundle = StbiImporter().read(ifs, ".png", "assets/images/tux.png",  {});
    auto tex = ren.createTexture(bundle.get<ImageRGBA>());

    Rectf rect;
    rect.dimensions = {50, 50};

    auto floor = world->createBody();

    ColliderDesc desc;
    desc.shape.type = COLLIDER_2D;
    desc.shape.vertices.emplace_back(-1000, -5, 0);
    desc.shape.vertices.emplace_back(1000, -5, 0);
    desc.shape.vertices.emplace_back(1000, 5, 0);
    desc.shape.vertices.emplace_back(-1000, 5, 0);

    auto col = floor->createCollider(desc);

    floor->setRigidBodyType(xng::RigidBody::STATIC);

    auto rb = world->createBody();

    rb->setRigidBodyType(xng::RigidBody::DYNAMIC);
    rb->setMass(30,  Vec3f{0, 0, 0});

    rb->setPosition({0, 100, 0});

    desc = {};
    desc.properties.friction = 0.1;
    desc.shape.type = COLLIDER_2D;
    desc.shape.vertices.emplace_back(0, 0, 0);
    desc.shape.vertices.emplace_back(0, 50, 0);
    desc.shape.vertices.emplace_back(50, 50, 0);
    desc.shape.vertices.emplace_back(50, 0, 0);

    auto rbCol = rb->createCollider(desc);

    xng::FrameLimiter limiter;
    limiter.reset();
    while (!window->shouldClose()) {
        auto deltaTime = limiter.newFrame();

        if (input.getKey(KEY_UP)){
            rb->applyLinearImpulse({0, 1000, 0}, {25, 25, 0});
        } else if (input.getKey(KEY_LEFT)){
            rb->applyLinearImpulse({1000, 0, 0}, {25, 25, 0});
        } else if (input.getKey(KEY_RIGHT)){
            rb->applyLinearImpulse({-1000, 0, 0}, {25, 25, 0});
        } else if (input.getKey(KEY_DOWN)){
            rb->applyLinearImpulse({0, -1000, 0}, {25, 25, 0});
        }

        world->step(deltaTime);

        auto pos = rb->getPosition();
        rect.position = {pos.x, pos.y};

        ren.renderBegin(*target);

        auto renRect = rect;
        renRect.position.y *= -1;
        renRect.position += target->getDescription().size.convert<float>() / 2;

        ren.draw(Rectf{{}, tex.getSize().convert<float>()}, renRect, tex, {25, 25}, rb->getRotation().z, LINEAR);

        ren.renderPresent();

        overlay.draw(deltaTime, *target);

        window->swapBuffers();
        window->update();
    }

    return 0;
}