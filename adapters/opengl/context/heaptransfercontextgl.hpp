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

#ifndef XENGINE_HEAPTRANSFERCONTEXTGL_HPP
#define XENGINE_HEAPTRANSFERCONTEXTGL_HPP

#include <chrono>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <variant>
#include <vector>

#include "xng/display/window.hpp"
#include "xng/rendergraph/pass.hpp"
#include "xng/util/downcast.hpp"

#include "glad/glad.h"
#include "ogldebug.hpp"

#include "heapgl.hpp"
#include "heaptransfergl.hpp"
#include "passresources.hpp"

#include "display/windowgl.hpp"
#include "context/transfercontextgl.hpp"

namespace xng::opengl {
    class HeapTransferContextGL final : public rg::TransferContext {
    public:
        struct CopyBufferCmd {
            HeapResource<Buffer> target{};
            HeapResource<Buffer> source{};
            size_t targetOffset{};
            size_t sourceOffset{};
            size_t count{};
        };

        struct CopyTextureCmd {
            HeapResource<Texture> target{};
            HeapResource<Texture> source{};
            Vec3i srcOffset{};
            Vec3i dstOffset{};
            Vec3i size{};
            size_t srcMipMapLevel{};
            size_t dstMipMapLevel{};
        };

        struct CopyBufferToTextureCmd {
            HeapResource<Texture> texture{};
            HeapResource<Buffer> buffer{};
            Texture::SubResource textureSubResource{};
            size_t bufferOffset{};
            Recti textureOffset{};
            ColorFormat bufferFormat{};
        };

        struct CopyTextureToBufferCmd {
            HeapResource<Buffer> buffer{};
            HeapResource<Texture> texture{};
            Texture::SubResource textureSubResource{};
            size_t bufferOffset{};
            Recti textureOffset{};
            ColorFormat bufferFormat{};
        };

        struct ClearTextureCmd {
            HeapResource<Texture> target{};
            Texture::SubResource subResource{};
            Texture::ClearValue clearValue{};
        };

        struct BlitTextureCmd {
            HeapResource<Texture> src;
            HeapResource<Texture> dst;
            Texture::SubResource srcTarget;
            Texture::SubResource dstTarget;
            Recti srcRect;
            Recti dstRect;
            TextureFiltering filtering;
        };

        struct GenerateMipMapsCmd {
            HeapResource<Texture> target{};
        };

        struct SyncCmd {
            std::shared_ptr<HeapTransferSync> sync;
        };

        using Command = std::variant<CopyBufferCmd,
            CopyTextureCmd,
            CopyBufferToTextureCmd,
            CopyTextureToBufferCmd,
            ClearTextureCmd,
            GenerateMipMapsCmd,
            BlitTextureCmd,
            SyncCmd>;

    private:
        struct BufferRegion {
            size_t offset;
            size_t size; // 0 = unknown/rest of buffer
            bool write;
        };

        struct TextureRegion {
            Texture::SubResource sub;
            bool write;
            bool allMips = false; // true for GenerateMipMaps — covers all mip levels
        };

        struct BufferSyncEntry {
            BufferRegion region;
            std::shared_ptr<HeapTransferSync> sync;
        };

        struct TextureSyncEntry {
            TextureRegion region;
            std::shared_ptr<HeapTransferSync> sync;
        };

    public:
        explicit HeapTransferContextGL(std::unique_ptr<Window> heapWindow, Heap &heap)
            : heapWindow(std::move(heapWindow)), heap(heap) {
            thread = std::thread(&HeapTransferContextGL::loop, this);
        }

        ~HeapTransferContextGL() override {
            {
                std::lock_guard lock(mutex);
                running = false;
            }
            cv.notify_one();
            thread.join();
            std::lock_guard lock(syncMutex);
            for (auto &[handle, entries]: bufferSyncs) {
                for (auto &e: entries) {
                    std::lock_guard syncLock(e.sync->mutex);
                    if (e.sync->fence && !e.sync->done) {
                        glDeleteSync(e.sync->fence);
                        e.sync->fence = nullptr;
                        e.sync->done = true;
                    }
                }
            }
            for (auto &[handle, entries]: textureSyncs) {
                for (auto &e: entries) {
                    std::lock_guard syncLock(e.sync->mutex);
                    if (e.sync->fence && !e.sync->done) {
                        glDeleteSync(e.sync->fence);
                        e.sync->fence = nullptr;
                        e.sync->done = true;
                    }
                }
            }
        }

        void copyBuffer(const Resource<Buffer> &target,
                        const Resource<Buffer> &source,
                        const size_t targetOffset,
                        const size_t sourceOffset,
                        const size_t count) override {
            std::lock_guard lock(mutex);
            commandQueue.emplace_back(CopyBufferCmd{
                HeapResource(target.getHandle(), target.getDescription(), heap),
                HeapResource(source.getHandle(), source.getDescription(), heap),
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
                HeapResource(target.getHandle(), target.getDescription(), heap),
                HeapResource(source.getHandle(), source.getDescription(), heap),
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
                                 const Recti &textureOffset,
                                 const ColorFormat bufferFormat) override {
            std::lock_guard lock(mutex);
            commandQueue.emplace_back(CopyBufferToTextureCmd{
                HeapResource(texture.getHandle(), texture.getDescription(), heap),
                HeapResource(buffer.getHandle(), buffer.getDescription(), heap),
                textureSubResource,
                bufferOffset,
                textureOffset,
                bufferFormat
            });
            cv.notify_one();
        }

        void copyTextureToBuffer(const Resource<Buffer> &buffer,
                                 const Resource<Texture> &texture,
                                 const Texture::SubResource textureSubResource,
                                 const size_t bufferOffset,
                                 const Recti &textureOffset,
                                 const ColorFormat bufferFormat) override {
            std::lock_guard lock(mutex);
            commandQueue.emplace_back(CopyTextureToBufferCmd{
                HeapResource(buffer.getHandle(), buffer.getDescription(), heap),
                HeapResource(texture.getHandle(), texture.getDescription(), heap),
                textureSubResource,
                bufferOffset,
                textureOffset,
                bufferFormat
            });
            cv.notify_one();
        }

        void clearTexture(const Resource<Texture> &texture,
                          const Texture::SubResource &target,
                          const Texture::ClearValue &clearValue) override {
            std::lock_guard lock(mutex);
            commandQueue.emplace_back(ClearTextureCmd{
                HeapResource(texture.getHandle(), texture.getDescription(), heap),
                target,
                clearValue
            });
            cv.notify_one();
        }

        void blitTexture(const Resource<Texture> &src,
                         const Resource<Texture> &dst,
                         const Texture::SubResource &srcTarget,
                         const Texture::SubResource &dstTarget,
                         const Recti &srcRect,
                         const Recti &dstRect,
                         const TextureFiltering &filtering) override {
            std::lock_guard lock(mutex);
            commandQueue.emplace_back(BlitTextureCmd{
                HeapResource(src.getHandle(), src.getDescription(), heap),
                HeapResource(dst.getHandle(), dst.getDescription(), heap),
                srcTarget,
                dstTarget,
                srcRect,
                dstRect,
                filtering
            });
            cv.notify_one();
        }

        void generateMipMaps(const Resource<Texture> &texture) override {
            std::lock_guard lock(mutex);
            commandQueue.emplace_back(GenerateMipMapsCmd{
                HeapResource(texture.getHandle(), texture.getDescription(), heap)
            });
            cv.notify_one();
        }

        std::unique_ptr<HeapTransferGL> finishTransfer() {
            auto sync = std::make_shared<HeapTransferSync>();
            {
                std::lock_guard lock(mutex);
                commandQueue.emplace_back(SyncCmd{sync});
                cv.notify_one();
            }
            return std::make_unique<HeapTransferGL>(sync);
        }

        // Wait up to timeoutMs for overlapping buffer transfers to complete.
        // Returns true if overlapping transfers are still pending after the timeout.
        // timeoutMs == 0 is a non-blocking check.
        bool waitForTransfers(const ResourceId::Handle handle,
                              const std::vector<BufferAccess> &accesses,
                              const size_t timeoutMs) {
            const auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeoutMs);
            {
                std::unique_lock lock(mutex);
                const auto pred = [&] {
                    const auto it = inFlightBuffers.find(handle);
                    if (it != inFlightBuffers.end()) {
                        for (const auto &r: it->second) {
                            if (bufferConflicts(r.write, r.offset, r.size, accesses)) return false;
                        }
                    }
                    for (const auto &cmd: commandQueue) {
                        if (commandOverlapsBuffer(cmd, handle, accesses)) return false;
                    }
                    return true;
                };
                if (timeoutMs == 0) {
                    if (!pred()) return true;
                } else {
                    if (!batchCv.wait_until(lock, deadline, pred)) return true;
                }
            }
            return checkPendingBufferSync(handle, accesses, deadline);
        }

        // Wait up to timeoutMs for overlapping texture transfers to complete.
        // Returns true if overlapping transfers are still pending after the timeout.
        // timeoutMs == 0 is a non-blocking check.
        bool waitForTransfers(const ResourceId::Handle handle,
                              const std::vector<TextureAccess> &accesses,
                              const size_t timeoutMs) {
            const auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeoutMs);
            {
                std::unique_lock lock(mutex);
                const auto pred = [&] {
                    const auto it = inFlightTextures.find(handle);
                    if (it != inFlightTextures.end()) {
                        for (const auto &r: it->second) {
                            if (textureRegionConflicts(r, accesses)) return false;
                        }
                    }
                    for (const auto &cmd: commandQueue) {
                        if (commandOverlapsTexture(cmd, handle, accesses)) return false;
                    }
                    return true;
                };
                if (timeoutMs == 0) {
                    if (!pred()) return true;
                } else {
                    if (!batchCv.wait_until(lock, deadline, pred)) return true;
                }
            }
            return checkPendingTextureSync(handle, accesses, deadline);
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
            blockUntilIdle(handle);
            {
                std::lock_guard lock(mutex);
                resources.buffers.erase(handle);
                resources.textures.erase(handle);
                freeHandles.push_back(handle);
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
        // ---- region extraction per command type ----

        static void cmdBufferRegions(const CopyBufferCmd &c,
                                     std::unordered_map<ResourceId::Handle, std::vector<BufferRegion> > &out) {
            out[c.target.getHandle()].push_back({c.targetOffset, c.count, true});
            out[c.source.getHandle()].push_back({c.sourceOffset, c.count, false});
        }

        static void cmdBufferRegions(const CopyBufferToTextureCmd &c,
                                     std::unordered_map<ResourceId::Handle, std::vector<BufferRegion> > &out) {
            out[c.buffer.getHandle()].push_back({c.bufferOffset, 0, false});
        }

        static void cmdBufferRegions(const CopyTextureToBufferCmd &c,
                                     std::unordered_map<ResourceId::Handle, std::vector<BufferRegion> > &out) {
            out[c.buffer.getHandle()].push_back({c.bufferOffset, 0, true});
        }

        static void cmdBufferRegions(const CopyTextureCmd &,
                                     std::unordered_map<ResourceId::Handle, std::vector<BufferRegion> > &) {
        }

        static void cmdBufferRegions(const ClearTextureCmd &,
                                     std::unordered_map<ResourceId::Handle, std::vector<BufferRegion> > &) {
        }

        static void cmdBufferRegions(const BlitTextureCmd &,
                                     std::unordered_map<ResourceId::Handle, std::vector<BufferRegion> > &) {
        }

        static void cmdBufferRegions(const GenerateMipMapsCmd &,
                                     std::unordered_map<ResourceId::Handle, std::vector<BufferRegion> > &) {
        }

        static void cmdBufferRegions(const SyncCmd &,
                                     std::unordered_map<ResourceId::Handle, std::vector<BufferRegion> > &) {
        }

        static void cmdTextureRegions(const CopyTextureCmd &c,
                                      std::unordered_map<ResourceId::Handle, std::vector<TextureRegion> > &out) {
            Texture::SubResource dst;
            dst.mipLevel = c.dstMipMapLevel;
            out[c.target.getHandle()].push_back({dst, true, false});
            Texture::SubResource src;
            src.mipLevel = c.srcMipMapLevel;
            out[c.source.getHandle()].push_back({src, false, false});
        }

        static void cmdTextureRegions(const BlitTextureCmd &c,
                                      std::unordered_map<ResourceId::Handle, std::vector<TextureRegion> > &out) {
            out[c.dst.getHandle()].push_back({c.dstTarget, true, false});
            out[c.src.getHandle()].push_back({c.srcTarget, false, false});
        }

        static void cmdTextureRegions(const CopyBufferToTextureCmd &c,
                                      std::unordered_map<ResourceId::Handle, std::vector<TextureRegion> > &out) {
            out[c.texture.getHandle()].push_back({c.textureSubResource, true, false});
        }

        static void cmdTextureRegions(const CopyTextureToBufferCmd &c,
                                      std::unordered_map<ResourceId::Handle, std::vector<TextureRegion> > &out) {
            out[c.texture.getHandle()].push_back({c.textureSubResource, false, false});
        }

        static void cmdTextureRegions(const ClearTextureCmd &c,
                                      std::unordered_map<ResourceId::Handle, std::vector<TextureRegion> > &out) {
            out[c.target.getHandle()].push_back({c.subResource, true, false});
        }

        static void cmdTextureRegions(const GenerateMipMapsCmd &c,
                                      std::unordered_map<ResourceId::Handle, std::vector<TextureRegion> > &out) {
            out[c.target.getHandle()].push_back({Texture::SubResource{}, true, true}); // allMips
        }

        static void cmdTextureRegions(const CopyBufferCmd &,
                                      std::unordered_map<ResourceId::Handle, std::vector<TextureRegion> > &) {
        }

        static void cmdTextureRegions(const SyncCmd &,
                                      std::unordered_map<ResourceId::Handle, std::vector<TextureRegion> > &) {
        }

        static void collectRegions(const Command &cmd,
                                   std::unordered_map<ResourceId::Handle, std::vector<BufferRegion> > &buffers,
                                   std::unordered_map<ResourceId::Handle, std::vector<TextureRegion> > &textures) {
            std::visit([&](const auto &c) {
                cmdBufferRegions(c, buffers);
                cmdTextureRegions(c, textures);
            }, cmd);
        }

        // ---- visitHandles (for commandTouchesHandle used by blockUntilIdle) ----

        template<typename F>
        static void visitHandles(const Command &cmd, F &&f) {
            std::visit([&](const auto &c) { visitHandlesImpl(c, std::forward<F>(f)); }, cmd);
        }

        template<typename F>
        static void visitHandlesImpl(const CopyBufferCmd &c, F &&f) {
            f(c.target.getHandle());
            f(c.source.getHandle());
        }

        template<typename F>
        static void visitHandlesImpl(const CopyTextureCmd &c, F &&f) {
            f(c.target.getHandle());
            f(c.source.getHandle());
        }

        template<typename F>
        static void visitHandlesImpl(const CopyBufferToTextureCmd &c, F &&f) {
            f(c.texture.getHandle());
            f(c.buffer.getHandle());
        }

        template<typename F>
        static void visitHandlesImpl(const CopyTextureToBufferCmd &c, F &&f) {
            f(c.buffer.getHandle());
            f(c.texture.getHandle());
        }

        template<typename F>
        static void visitHandlesImpl(const ClearTextureCmd &c, F &&f) { f(c.target.getHandle()); }

        template<typename F>
        static void visitHandlesImpl(const BlitTextureCmd &c, F &&f) {
            f(c.src.getHandle());
            f(c.dst.getHandle());
        }

        template<typename F>
        static void visitHandlesImpl(const GenerateMipMapsCmd &c, F &&f) { f(c.target.getHandle()); }

        template<typename F>
        static void visitHandlesImpl(const SyncCmd &, F &&) {
        }

        static bool commandTouchesHandle(const Command &cmd, const ResourceId::Handle handle) {
            bool found = false;
            visitHandles(cmd, [&](ResourceId::Handle h) { if (h == handle) found = true; });
            return found;
        }

        // ---- buffer overlap ----

        static bool bufferRangesOverlap(const size_t off1,
                                        const size_t size1,
                                        const size_t off2,
                                        const size_t size2) {
            if (size1 == 0 || size2 == 0) return true;
            return off1 < off2 + size2 && off2 < off1 + size1;
        }

        static bool bufferConflicts(const bool transferWrite,
                                    const size_t offset,
                                    const size_t size,
                                    const std::vector<BufferAccess> &accesses) {
            for (const auto &acc: accesses) {
                const bool accWrite = acc.type == BufferAccess::TransferDst ||
                                      acc.type == BufferAccess::StorageWrite;
                if (!transferWrite && !accWrite) continue;
                if (bufferRangesOverlap(offset, size, acc.offset, acc.size)) return true;
            }
            return false;
        }

        static bool cmdOverlapsBuffer(const CopyBufferCmd &c,
                                      const ResourceId::Handle h,
                                      const std::vector<BufferAccess> &a) {
            bool r = false;
            if (c.target.getHandle() == h) r |= bufferConflicts(true, c.targetOffset, c.count, a);
            if (c.source.getHandle() == h) r |= bufferConflicts(false, c.sourceOffset, c.count, a);
            return r;
        }

        static bool cmdOverlapsBuffer(const CopyTextureCmd &,
                                      const ResourceId::Handle,
                                      const std::vector<BufferAccess> &) { return false; }

        static bool cmdOverlapsBuffer(const CopyBufferToTextureCmd &c,
                                      const ResourceId::Handle h,
                                      const std::vector<BufferAccess> &a) {
            if (c.buffer.getHandle() != h) return false;
            return bufferConflicts(false, c.bufferOffset, 0, a);
        }

        static bool cmdOverlapsBuffer(const CopyTextureToBufferCmd &c,
                                      const ResourceId::Handle h,
                                      const std::vector<BufferAccess> &a) {
            if (c.buffer.getHandle() != h) return false;
            return bufferConflicts(true, c.bufferOffset, 0, a);
        }

        static bool cmdOverlapsBuffer(const ClearTextureCmd &,
                                      const ResourceId::Handle,
                                      const std::vector<BufferAccess> &) { return false; }

        static bool cmdOverlapsBuffer(const BlitTextureCmd &,
                                      const ResourceId::Handle,
                                      const std::vector<BufferAccess> &) { return false; }

        static bool cmdOverlapsBuffer(const GenerateMipMapsCmd &,
                                      const ResourceId::Handle,
                                      const std::vector<BufferAccess> &) { return false; }

        static bool cmdOverlapsBuffer(const SyncCmd &,
                                      const ResourceId::Handle,
                                      const std::vector<BufferAccess> &) { return false; }

        static bool commandOverlapsBuffer(const Command &cmd,
                                          const ResourceId::Handle h,
                                          const std::vector<BufferAccess> &a) {
            return std::visit([&](const auto &c) { return cmdOverlapsBuffer(c, h, a); }, cmd);
        }

        // ---- texture overlap ----

        static bool subResourceOverlapsRange(const Texture::SubResource &sub,
                                             const TextureBinding::Range &range) {
            if (range.numMipLevels > 0) {
                if (sub.mipLevel < range.baseMipLevel ||
                    sub.mipLevel >= range.baseMipLevel + range.numMipLevels)
                    return false;
            }
            const auto layer = sub.arrayLayer;
            if (layer != static_cast<size_t>(-1) && range.numArrayLayers > 0) {
                if (layer < range.baseArrayLayer ||
                    layer >= range.baseArrayLayer + range.numArrayLayers)
                    return false;
            }
            return true;
        }

        static bool textureConflicts(const bool transferWrite,
                                     const Texture::SubResource &sub,
                                     const std::vector<TextureAccess> &accesses) {
            for (const auto &acc: accesses) {
                const bool accWrite = acc.type == TextureAccess::TextureStorageWrite ||
                                      acc.type == TextureAccess::TextureTransferDst;
                if (!transferWrite && !accWrite) continue;
                if (subResourceOverlapsRange(sub, acc.range)) return true;
            }
            return false;
        }

        static bool textureRegionConflicts(const TextureRegion &r,
                                           const std::vector<TextureAccess> &accesses) {
            for (const auto &acc: accesses) {
                const bool accWrite = acc.type == TextureAccess::TextureStorageWrite ||
                                      acc.type == TextureAccess::TextureTransferDst;
                if (!r.write && !accWrite) continue;
                if (r.allMips) return true; // GenerateMipMaps covers all mips
                if (subResourceOverlapsRange(r.sub, acc.range)) return true;
            }
            return false;
        }

        static bool cmdOverlapsTexture(const CopyBufferCmd &,
                                       const ResourceId::Handle,
                                       const std::vector<TextureAccess> &) { return false; }

        static bool cmdOverlapsTexture(const CopyTextureCmd &c,
                                       const ResourceId::Handle h,
                                       const std::vector<TextureAccess> &a) {
            bool r = false;
            if (c.target.getHandle() == h) {
                Texture::SubResource sub;
                sub.mipLevel = c.dstMipMapLevel;
                r |= textureConflicts(true, sub, a);
            }
            if (c.source.getHandle() == h) {
                Texture::SubResource sub;
                sub.mipLevel = c.srcMipMapLevel;
                r |= textureConflicts(false, sub, a);
            }
            return r;
        }

        static bool cmdOverlapsTexture(const CopyBufferToTextureCmd &c,
                                       const ResourceId::Handle h,
                                       const std::vector<TextureAccess> &a) {
            if (c.texture.getHandle() != h) return false;
            return textureConflicts(true, c.textureSubResource, a);
        }

        static bool cmdOverlapsTexture(const CopyTextureToBufferCmd &c,
                                       const ResourceId::Handle h,
                                       const std::vector<TextureAccess> &a) {
            if (c.texture.getHandle() != h) return false;
            return textureConflicts(false, c.textureSubResource, a);
        }

        static bool cmdOverlapsTexture(const ClearTextureCmd &c,
                                       const ResourceId::Handle h,
                                       const std::vector<TextureAccess> &a) {
            if (c.target.getHandle() != h) return false;
            return textureConflicts(true, c.subResource, a);
        }

        static bool cmdOverlapsTexture(const BlitTextureCmd &c,
                                       const ResourceId::Handle h,
                                       const std::vector<TextureAccess> &a) {
            bool r = false;
            if (c.dst.getHandle() == h) {
                r |= textureConflicts(true, c.dstTarget, a);
            }
            if (c.src.getHandle() == h) {
                r |= textureConflicts(false, c.srcTarget, a);
            }
            return r;
        }

        static bool cmdOverlapsTexture(const GenerateMipMapsCmd &c,
                                       const ResourceId::Handle h,
                                       const std::vector<TextureAccess> &) {
            return c.target.getHandle() == h;
        }

        static bool cmdOverlapsTexture(const SyncCmd &,
                                       const ResourceId::Handle,
                                       const std::vector<TextureAccess> &) { return false; }

        static bool commandOverlapsTexture(const Command &cmd,
                                           const ResourceId::Handle h,
                                           const std::vector<TextureAccess> &a) {
            return std::visit([&](const auto &c) { return cmdOverlapsTexture(c, h, a); }, cmd);
        }

        // ---- GPU sync checking, region-filtered ----

        bool checkPendingBufferSync(const ResourceId::Handle handle,
                                    const std::vector<BufferAccess> &accesses,
                                    const std::chrono::steady_clock::time_point deadline) {
            std::vector<std::shared_ptr<HeapTransferSync> > toWait;
            {
                std::lock_guard lock(syncMutex);
                const auto it = bufferSyncs.find(handle);
                if (it == bufferSyncs.end()) return false;
                for (const auto &e: it->second) {
                    if (bufferConflicts(e.region.write, e.region.offset, e.region.size, accesses))
                        toWait.push_back(e.sync);
                }
            }
            bool pending = false;
            for (const auto &sync: toWait) {
                std::lock_guard lock(sync->mutex);
                if (sync->done) continue;
                if (!sync->fence) {
                    pending = true;
                    continue;
                }
                const auto remainingNs = std::chrono::duration_cast<std::chrono::nanoseconds>(
                    deadline - std::chrono::steady_clock::now()).count();
                const GLuint64 timeoutNs = remainingNs > 0 ? static_cast<GLuint64>(remainingNs) : 0;
                const auto result = glClientWaitSync(sync->fence, GL_SYNC_FLUSH_COMMANDS_BIT, timeoutNs);
                if (result == GL_ALREADY_SIGNALED || result == GL_CONDITION_SATISFIED) {
                    glDeleteSync(sync->fence);
                    sync->fence = nullptr;
                    sync->done = true;
                } else {
                    pending = true;
                }
            }
            pruneBufferSyncs(handle);
            return pending;
        }

        bool checkPendingTextureSync(const ResourceId::Handle handle,
                                     const std::vector<TextureAccess> &accesses,
                                     const std::chrono::steady_clock::time_point deadline) {
            std::vector<std::shared_ptr<HeapTransferSync> > toWait;
            {
                std::lock_guard lock(syncMutex);
                const auto it = textureSyncs.find(handle);
                if (it == textureSyncs.end()) return false;
                for (const auto &e: it->second) {
                    if (textureRegionConflicts(e.region, accesses))
                        toWait.push_back(e.sync);
                }
            }
            bool pending = false;
            for (const auto &sync: toWait) {
                std::lock_guard lock(sync->mutex);
                if (sync->done) continue;
                if (!sync->fence) {
                    pending = true;
                    continue;
                }
                const auto remainingNs = std::chrono::duration_cast<std::chrono::nanoseconds>(
                    deadline - std::chrono::steady_clock::now()).count();
                const GLuint64 timeoutNs = remainingNs > 0 ? static_cast<GLuint64>(remainingNs) : 0;
                const auto result = glClientWaitSync(sync->fence, GL_SYNC_FLUSH_COMMANDS_BIT, timeoutNs);
                if (result == GL_ALREADY_SIGNALED || result == GL_CONDITION_SATISFIED) {
                    glDeleteSync(sync->fence);
                    sync->fence = nullptr;
                    sync->done = true;
                } else {
                    pending = true;
                }
            }
            pruneTextureSyncs(handle);
            return pending;
        }

        void pruneBufferSyncs(const ResourceId::Handle handle) {
            std::lock_guard lock(syncMutex);
            const auto it = bufferSyncs.find(handle);
            if (it == bufferSyncs.end()) return;
            auto &v = it->second;
            v.erase(std::remove_if(v.begin(), v.end(),
                                   [](const BufferSyncEntry &e) { return e.sync->done; }),
                    v.end());
            if (v.empty()) bufferSyncs.erase(it);
        }

        void pruneTextureSyncs(const ResourceId::Handle handle) {
            std::lock_guard lock(syncMutex);
            const auto it = textureSyncs.find(handle);
            if (it == textureSyncs.end()) return;
            auto &v = it->second;
            v.erase(std::remove_if(v.begin(), v.end(),
                                   [](const TextureSyncEntry &e) { return e.sync->done; }),
                    v.end());
            if (v.empty()) textureSyncs.erase(it);
        }

        // Blocking wait for all transfers on a handle — used by free().
        void blockUntilIdle(const ResourceId::Handle handle) {
            {
                std::unique_lock lock(mutex);
                batchCv.wait(lock, [&] {
                    if (inFlightBuffers.count(handle) || inFlightTextures.count(handle)) return false;
                    for (const auto &cmd: commandQueue) {
                        if (commandTouchesHandle(cmd, handle)) return false;
                    }
                    return true;
                });
            }
            std::vector<std::shared_ptr<HeapTransferSync> > allSyncs;
            {
                std::lock_guard lock(syncMutex);
                for (auto &e: bufferSyncs[handle]) allSyncs.push_back(e.sync);
                for (auto &e: textureSyncs[handle]) allSyncs.push_back(e.sync);
                bufferSyncs.erase(handle);
                textureSyncs.erase(handle);
            }
            for (const auto &sync: allSyncs) {
                std::lock_guard lock(sync->mutex);
                if (!sync->done) {
                    if (sync->fence) {
                        glClientWaitSync(sync->fence, GL_SYNC_FLUSH_COMMANDS_BIT, GL_TIMEOUT_IGNORED);
                        glDeleteSync(sync->fence);
                        sync->fence = nullptr;
                    }
                    sync->done = true;
                }
            }
        }

        // ---- background loop ----

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
                    // Populate in-flight regions for the entire batch so waitForTransfers predicates see them.
                    for (const auto &cmd: batch) {
                        collectRegions(cmd, inFlightBuffers, inFlightTextures);
                    }
                }

                TransferContextGL context(PassResources({}, ctxResources));

                // Regions accumulated since the last SyncCmd (or batch start).
                std::unordered_map<ResourceId::Handle, std::vector<BufferRegion> > pendingBuffers;
                std::unordered_map<ResourceId::Handle, std::vector<TextureRegion> > pendingTextures;

                for (auto &cmd: batch) {
                    if (const auto *syncCmd = std::get_if<SyncCmd>(&cmd)) {
                        GLsync fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
                        glFlush();
                        {
                            std::lock_guard lock(syncCmd->sync->mutex);
                            syncCmd->sync->fence = fence;
                        }
                        {
                            std::lock_guard lock(syncMutex);
                            for (auto &[h, regions]: pendingBuffers) {
                                for (auto &r: regions)
                                    bufferSyncs[h].push_back({r, syncCmd->sync});
                            }
                            for (auto &[h, regions]: pendingTextures) {
                                for (auto &r: regions)
                                    textureSyncs[h].push_back({r, syncCmd->sync});
                            }
                        }
                        pendingBuffers.clear();
                        pendingTextures.clear();
                    } else {
                        std::visit([&](auto &c) {
                            using T = std::decay_t<decltype(c)>;
                            if constexpr (!std::is_same_v<T, SyncCmd>) {
                                execute(context, c);
                                cmdBufferRegions(c, pendingBuffers);
                                cmdTextureRegions(c, pendingTextures);
                            }
                        }, cmd);
                    }
                }

                // Any regions after the last SyncCmd (or no SyncCmd) still need a GPU fence for blockUntilIdle.
                if (!pendingBuffers.empty() || !pendingTextures.empty()) {
                    auto fallback = std::make_shared<HeapTransferSync>();
                    fallback->fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
                    glFlush();
                    std::lock_guard lock(syncMutex);
                    for (auto &[h, regions]: pendingBuffers) {
                        for (auto &r: regions)
                            bufferSyncs[h].push_back({r, fallback});
                    }
                    for (auto &[h, regions]: pendingTextures) {
                        for (auto &r: regions)
                            textureSyncs[h].push_back({r, fallback});
                    }
                }

                {
                    std::lock_guard lock(mutex);
                    inFlightBuffers.clear();
                    inFlightTextures.clear();
                }
                batchCv.notify_all();
            }

            windowGl.unbindContext();
        }

        void execute(TransferContextGL &ctx, const CopyBufferCmd &cmd) {
            ctx.copyBuffer(cmd.target, cmd.source, cmd.targetOffset, cmd.sourceOffset, cmd.count);
        }

        void execute(TransferContextGL &ctx, const CopyTextureCmd &cmd) {
            ctx.copyTexture(cmd.target,
                            cmd.source,
                            cmd.srcOffset,
                            cmd.dstOffset,
                            cmd.size,
                            cmd.srcMipMapLevel,
                            cmd.dstMipMapLevel);
        }

        void execute(TransferContextGL &ctx, const CopyBufferToTextureCmd &cmd) {
            ctx.copyBufferToTexture(cmd.texture,
                                    cmd.buffer,
                                    cmd.textureSubResource,
                                    cmd.bufferOffset,
                                    cmd.textureOffset,
                                    cmd.bufferFormat);
        }

        void execute(TransferContextGL &ctx, const CopyTextureToBufferCmd &cmd) {
            ctx.copyTextureToBuffer(cmd.buffer,
                                    cmd.texture,
                                    cmd.textureSubResource,
                                    cmd.bufferOffset,
                                    cmd.textureOffset,
                                    cmd.bufferFormat);
        }

        void execute(TransferContextGL &ctx, const ClearTextureCmd &cmd) {
            ctx.clearTexture(cmd.target, cmd.subResource, cmd.clearValue);
        }

        void execute(TransferContextGL &ctx, const BlitTextureCmd &cmd) {
            ctx.blitTexture(cmd.src, cmd.dst, cmd.srcTarget, cmd.dstTarget, cmd.srcRect, cmd.dstRect, cmd.filtering);
        }

        void execute(TransferContextGL &ctx, const GenerateMipMapsCmd &cmd) {
            ctx.generateMipMaps(cmd.target);
        }

        ResourceId::Handle allocateHandle() {
            if (freeHandles.empty()) return nextHandle++;
            const auto ret = freeHandles.back();
            freeHandles.pop_back();
            return ret;
        }

        ResourceId::Handle nextHandle = 0;
        std::vector<ResourceId::Handle> freeHandles{};

        std::thread thread;
        std::unique_ptr<Window> heapWindow;

        Heap &heap;

        std::mutex mutex;
        std::condition_variable cv;
        std::condition_variable batchCv;
        std::vector<Command> commandQueue;

        // Guarded by mutex — per-region in-flight tracking for the current batch.
        std::unordered_map<ResourceId::Handle, std::vector<BufferRegion> > inFlightBuffers;
        std::unordered_map<ResourceId::Handle, std::vector<TextureRegion> > inFlightTextures;

        // Guarded by syncMutex — per-region GPU sync entries.
        std::mutex syncMutex;
        std::unordered_map<ResourceId::Handle, std::vector<BufferSyncEntry> > bufferSyncs;
        std::unordered_map<ResourceId::Handle, std::vector<TextureSyncEntry> > textureSyncs;

        ResourceScope resources;

        bool running = true;
    };
}

#endif //XENGINE_HEAPTRANSFERCONTEXTGL_HPP
