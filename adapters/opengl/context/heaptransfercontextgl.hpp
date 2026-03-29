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

#ifndef XENGINE_HEAPTRANSFERCONTEXTGL_HPP
#define XENGINE_HEAPTRANSFERCONTEXTGL_HPP

#include <condition_variable>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

#include "glad/glad.h"
#include "ogldebug.hpp"

#include "context/transfercontextgl.hpp"

#include "heapgl.hpp"
#include "passresources.hpp"
#include "display/windowgl.hpp"
#include "xng/display/window.hpp"
#include "xng/rendergraph/statistics.hpp"
#include "xng/util/downcast.hpp"

namespace xng::opengl {
    class HeapTransferContextGL final : public rendergraph::TransferContext {
    public:
        struct UploadBufferCmd {
            ResourceId::Handle target{};
            std::vector<uint8_t> data{};
            size_t targetOffset{};
        };

        struct UploadTextureCmd {
            ResourceId::Handle target{};
            Texture::SubResource subResource{};
            std::vector<uint8_t> data{};
            ColorFormat bufferFormat{};
            Vec2i offset{};
            Vec2i size{};
        };

        struct CopyBufferCmd {
            ResourceId::Handle target{};
            ResourceId::Handle source{};
            size_t targetOffset{};
            size_t sourceOffset{};
            size_t count{};
        };

        struct CopyTextureCmd {
            ResourceId::Handle target{};
            ResourceId::Handle source{};
            Vec3i srcOffset{};
            Vec3i dstOffset{};
            Vec3i size{};
            size_t srcMipMapLevel{};
            size_t dstMipMapLevel{};
        };

        struct CopyBufferToTextureCmd {
            ResourceId::Handle texture{};
            ResourceId::Handle buffer{};
            Texture::SubResource textureSubResource{};
            size_t bufferOffset{};
            Recti textureOffset{};
        };

        struct CopyTextureToBufferCmd {
            ResourceId::Handle buffer{};
            ResourceId::Handle texture{};
            Texture::SubResource textureSubResource{};
            size_t bufferOffset{};
            Recti textureOffset{};
        };

        struct ClearTextureCmd {
            ResourceId::Handle target{};
            Texture::SubResource subResource{};
            Texture::ClearValue clearValue{};
        };

        struct GenerateMipMapsCmd {
            ResourceId::Handle target{};
        };

        using Command = std::variant<UploadBufferCmd,
            UploadTextureCmd,
            CopyBufferCmd,
            CopyTextureCmd,
            CopyBufferToTextureCmd,
            CopyTextureToBufferCmd,
            ClearTextureCmd,
            GenerateMipMapsCmd>;

        explicit HeapTransferContextGL(std::unique_ptr<Window> heapWindow)
            : heapWindow(std::move(heapWindow)) {
            thread = std::thread(&HeapTransferContextGL::loop, this);
        }

        ~HeapTransferContextGL() override {
            {
                std::lock_guard lock(mutex);
                running = false;
            }
            cv.notify_one();
            thread.join();
            for (auto &fence: fences) {
                glDeleteSync(fence.second);
            }
        }

        void uploadBuffer(const Resource<Buffer> &target,
                          const uint8_t *buffer,
                          const size_t bufferSize,
                          const size_t targetOffset) override {
            std::lock_guard lock(mutex);
            commandQueue.emplace_back(UploadBufferCmd{
                target.getHandle(),
                std::vector<uint8_t>(buffer, buffer + bufferSize),
                targetOffset
            });
            cv.notify_one();
        }

        void uploadTexture(const Resource<Texture> &texture,
                           const Texture::SubResource target,
                           const uint8_t *buffer,
                           const size_t bufferSize,
                           const ColorFormat bufferFormat,
                           const Vec2i &offset,
                           const Vec2i &size) override {
            std::lock_guard lock(mutex);
            commandQueue.emplace_back(UploadTextureCmd{
                texture.getHandle(),
                target,
                std::vector<uint8_t>(buffer, buffer + bufferSize),
                bufferFormat,
                offset,
                size
            });
            cv.notify_one();
        }

        void copyBuffer(const Resource<Buffer> &target,
                        const Resource<Buffer> &source,
                        const size_t targetOffset,
                        const size_t sourceOffset,
                        const size_t count) override {
            std::lock_guard lock(mutex);
            commandQueue.emplace_back(CopyBufferCmd{
                target.getHandle(),
                source.getHandle(),
                targetOffset,
                sourceOffset,
                count
            });
            cv.notify_one();
        }

        void copyTexture(const Resource<Texture> &target,
                         const Resource<Texture> &source,
                         const Vec3i &srcOffset,
                         const Vec3i &dstOffset,
                         const Vec3i &size,
                         const size_t srcMipMapLevel,
                         const size_t dstMipMapLevel) override {
            std::lock_guard lock(mutex);
            commandQueue.emplace_back(CopyTextureCmd{
                target.getHandle(),
                source.getHandle(),
                srcOffset,
                dstOffset,
                size,
                srcMipMapLevel,
                dstMipMapLevel
            });
            cv.notify_one();
        }

        void copyBufferToTexture(const Resource<Texture> &texture,
                                 const Resource<Buffer> &buffer,
                                 const Texture::SubResource textureSubResource,
                                 const size_t bufferOffset,
                                 const Recti &textureOffset) override {
            std::lock_guard lock(mutex);
            commandQueue.emplace_back(CopyBufferToTextureCmd{
                texture.getHandle(),
                buffer.getHandle(),
                textureSubResource,
                bufferOffset,
                textureOffset
            });
            cv.notify_one();
        }

        void copyTextureToBuffer(const Resource<Buffer> &buffer,
                                 const Resource<Texture> &texture,
                                 const Texture::SubResource textureSubResource,
                                 const size_t bufferOffset,
                                 const Recti &textureOffset) override {
            std::lock_guard lock(mutex);
            commandQueue.emplace_back(CopyTextureToBufferCmd{
                buffer.getHandle(),
                texture.getHandle(),
                textureSubResource,
                bufferOffset,
                textureOffset
            });
            cv.notify_one();
        }

        void clearTexture(const Resource<Texture> &texture,
                          const Texture::SubResource &target,
                          const Texture::ClearValue &clearValue) override {
            std::lock_guard lock(mutex);
            commandQueue.emplace_back(ClearTextureCmd{
                texture.getHandle(),
                target,
                clearValue
            });
            cv.notify_one();
        }

        void generateMipMaps(const Resource<Texture> &texture) override {
            std::lock_guard lock(mutex);
            commandQueue.emplace_back(GenerateMipMapsCmd{texture.getHandle()});
            cv.notify_one();
        }

        bool hasPendingTransfers(const ResourceId::Handle handle) {
            {
                std::lock_guard lock(mutex);
                if (inFlightHandles.count(handle)) return true;
                for (const auto &cmd: commandQueue) {
                    if (commandTouchesHandle(cmd, handle)) return true;
                }
            }
            std::lock_guard lock(fencesMutex);
            const auto it = fences.find(handle);
            if (it == fences.end()) return false;
            const auto signaled = glClientWaitSync(it->second, GL_SYNC_FLUSH_COMMANDS_BIT, 0) == GL_ALREADY_SIGNALED;
            if (signaled) fences.erase(it);
            return !signaled;
        }

        void wait(const ResourceId::Handle handle) {
            {
                std::unique_lock lock(mutex);
                batchCv.wait(lock, [&] {
                    if (inFlightHandles.count(handle)) return false;
                    for (const auto &cmd: commandQueue) {
                        if (commandTouchesHandle(cmd, handle)) return false;
                    }
                    return true;
                });
            }
            std::lock_guard lock(fencesMutex);
            const auto it = fences.find(handle);
            if (it != fences.end()) {
                glClientWaitSync(it->second, GL_SYNC_FLUSH_COMMANDS_BIT, GL_TIMEOUT_IGNORED);
                glDeleteSync(it->second);
                fences.erase(it);
            }
        }

        ResourceId::Handle allocateBuffer(const Buffer &desc) {
            auto gl = std::make_shared<BufferGL>(desc);
            std::lock_guard lock(mutex);
            auto handle = allocateHandle();
            resources.buffers.emplace(handle, std::move(gl));
            return handle;
        }

        ResourceId::Handle allocateTexture(const Texture &desc) {
            auto gl = std::make_shared<TextureGL>(desc);
            std::lock_guard lock(mutex);
            auto handle = allocateHandle();
            resources.textures.emplace(handle, std::move(gl));
            return handle;
        }

        void free(const ResourceId::Handle handle) {
            wait(handle);
            {
                std::lock_guard lock(mutex);
                resources.buffers.erase(handle);
                resources.textures.erase(handle);
                freeHandles.push_back(handle);
            }
            {
                std::lock_guard lock(fencesMutex);
                const auto it = fences.find(handle);
                if (it != fences.end()) {
                    glDeleteSync(it->second);
                    fences.erase(it);
                }
            }
        }

        BufferGL &getBuffer(const ResourceId::Handle handle) {
            std::lock_guard lock(mutex);
            return *resources.buffers.at(handle);
        }

        ResourceScope getResources() {
            std::lock_guard lock(mutex);
            return resources;
        }

    private:
        template<typename F>
        static void visitHandles(const Command &cmd, F &&f) {
            std::visit([&](const auto &c) { visitHandlesImpl(c, std::forward<F>(f)); }, cmd);
        }

        template<typename F> static void visitHandlesImpl(const UploadBufferCmd &c, F &&f)      { f(c.target); }
        template<typename F> static void visitHandlesImpl(const UploadTextureCmd &c, F &&f)     { f(c.target); }
        template<typename F> static void visitHandlesImpl(const CopyBufferCmd &c, F &&f)        { f(c.target); f(c.source); }
        template<typename F> static void visitHandlesImpl(const CopyTextureCmd &c, F &&f)       { f(c.target); f(c.source); }
        template<typename F> static void visitHandlesImpl(const CopyBufferToTextureCmd &c, F &&f) { f(c.texture); f(c.buffer); }
        template<typename F> static void visitHandlesImpl(const CopyTextureToBufferCmd &c, F &&f) { f(c.buffer); f(c.texture); }
        template<typename F> static void visitHandlesImpl(const ClearTextureCmd &c, F &&f)      { f(c.target); }
        template<typename F> static void visitHandlesImpl(const GenerateMipMapsCmd &c, F &&f)   { f(c.target); }

        static bool commandTouchesHandle(const Command &cmd, const ResourceId::Handle handle) {
            bool found = false;
            visitHandles(cmd, [&](ResourceId::Handle h) { if (h == handle) found = true; });
            return found;
        }

        void createFence(const ResourceId::Handle handle) {
            GLsync fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

            auto it = fences.find(handle);
            if (it != fences.end()) {
                glDeleteSync(it->second);
            }

            fences[handle] = fence;
        }

        void loop() {
            auto &windowGl = down_cast<WindowGl &>(*heapWindow);
            windowGl.bindContext();

            while (true) {
                std::vector<Command> batch;
                ResourceScope ctxResources;
                {
                    std::unique_lock lock(mutex);
                    cv.wait(lock, [this] { return !commandQueue.empty() || !running; });
                    if (!running && commandQueue.empty()) break;
                    std::swap(batch, commandQueue);
                    ctxResources = resources;
                    for (const auto &cmd: batch) {
                        visitHandles(cmd, [&](ResourceId::Handle h) { inFlightHandles.insert(h); });
                    }
                }

                TransferContextGL context(PassResources({}, ctxResources), stats);

                for (auto &cmd: batch) {
                    std::visit([&](auto &c) { execute(context, c); }, cmd);
                }

                {
                    std::lock_guard lock(mutex);
                    inFlightHandles.clear();
                }
                batchCv.notify_all();
            }

            windowGl.unbindContext();
        }

        void execute(TransferContextGL &ctx, const UploadBufferCmd &cmd) {
            ctx.uploadBuffer(Resource<Buffer>(cmd.target, Buffer{}, ResourceId::HEAP),
                             cmd.data.data(), cmd.data.size(), cmd.targetOffset);
            std::lock_guard lock(fencesMutex);
            createFence(cmd.target);
        }

        void execute(TransferContextGL &ctx, const UploadTextureCmd &cmd) {
            ctx.uploadTexture(Resource<Texture>(cmd.target, Texture{}, ResourceId::HEAP),
                              cmd.subResource, cmd.data.data(), cmd.data.size(),
                              cmd.bufferFormat, cmd.offset, cmd.size);
            std::lock_guard lock(fencesMutex);
            createFence(cmd.target);
        }

        void execute(TransferContextGL &ctx, const CopyBufferCmd &cmd) {
            ctx.copyBuffer(Resource<Buffer>(cmd.target, Buffer{}, ResourceId::HEAP),
                           Resource<Buffer>(cmd.source, Buffer{}, ResourceId::HEAP),
                           cmd.targetOffset, cmd.sourceOffset, cmd.count);
            std::lock_guard lock(fencesMutex);
            createFence(cmd.target);
            createFence(cmd.source);
        }

        void execute(TransferContextGL &ctx, const CopyTextureCmd &cmd) {
            ctx.copyTexture(Resource<Texture>(cmd.target, Texture{}, ResourceId::HEAP),
                            Resource<Texture>(cmd.source, Texture{}, ResourceId::HEAP),
                            cmd.srcOffset, cmd.dstOffset, cmd.size,
                            cmd.srcMipMapLevel, cmd.dstMipMapLevel);
            std::lock_guard lock(fencesMutex);
            createFence(cmd.target);
            createFence(cmd.source);
        }

        void execute(TransferContextGL &ctx, const CopyBufferToTextureCmd &cmd) {
            ctx.copyBufferToTexture(Resource<Texture>(cmd.texture, Texture{}, ResourceId::HEAP),
                                    Resource<Buffer>(cmd.buffer, Buffer{}, ResourceId::HEAP),
                                    cmd.textureSubResource, cmd.bufferOffset, cmd.textureOffset);
            std::lock_guard lock(fencesMutex);
            createFence(cmd.texture);
            createFence(cmd.buffer);
        }

        void execute(TransferContextGL &ctx, const CopyTextureToBufferCmd &cmd) {
            ctx.copyTextureToBuffer(Resource<Buffer>(cmd.buffer, Buffer{}, ResourceId::HEAP),
                                    Resource<Texture>(cmd.texture, Texture{}, ResourceId::HEAP),
                                    cmd.textureSubResource, cmd.bufferOffset, cmd.textureOffset);
            std::lock_guard lock(fencesMutex);
            createFence(cmd.buffer);
            createFence(cmd.texture);
        }

        void execute(TransferContextGL &ctx, const ClearTextureCmd &cmd) {
            ctx.clearTexture(Resource<Texture>(cmd.target, Texture{}, ResourceId::HEAP),
                             cmd.subResource, cmd.clearValue);
            std::lock_guard lock(fencesMutex);
            createFence(cmd.target);
        }

        void execute(TransferContextGL &ctx, const GenerateMipMapsCmd &cmd) {
            ctx.generateMipMaps(Resource<Texture>(cmd.target, Texture{}, ResourceId::HEAP));
            std::lock_guard lock(fencesMutex);
            createFence(cmd.target);
        }

        ResourceId::Handle allocateHandle() {
            if (freeHandles.empty()) {
                return nextHandle++;
            }
            const auto ret = freeHandles.back();
            freeHandles.pop_back();
            return ret;
        }

        ResourceId::Handle nextHandle = 0;
        std::vector<ResourceId::Handle> freeHandles{};

        std::thread thread;
        std::unique_ptr<Window> heapWindow;

        Statistics stats;

        std::mutex mutex;
        std::condition_variable cv;
        std::condition_variable batchCv;
        std::vector<Command> commandQueue;
        std::unordered_set<ResourceId::Handle> inFlightHandles;

        std::mutex fencesMutex;
        std::unordered_map<ResourceId::Handle, GLsync> fences;

        ResourceScope resources;

        bool running = true;
    };
}

#endif //XENGINE_HEAPTRANSFERCONTEXTGL_HPP
