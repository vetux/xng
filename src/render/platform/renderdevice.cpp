#include "render/platform/renderdevice.hpp"

#include "render/platform/opengl/oglrenderdevice.hpp"

namespace xengine {
    std::unique_ptr<RenderDevice> RenderDevice::create(RenderPlatform backend) {
        switch (backend) {
            case OPENGL_4_1:
                return std::make_unique<opengl::OGLRenderDevice>();
            case OPENGL_4_1_QT:
            case DIRECTX_11:
            case VULKAN:
            default:
                throw std::runtime_error("Graphics backend not supported");
        }
    }
}