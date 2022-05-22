#include "render/platform/renderdevice.hpp"

#ifdef DRIVER_OPENGL

#include "render/platform/opengl/oglrenderdevice.hpp"

#endif

namespace xengine {
    std::unique_ptr<RenderDevice> RenderDevice::create(RenderPlatform backend) {
        switch (backend) {
#ifdef DRIVER_OPENGL
            case OPENGL_4_1:
                return std::make_unique<opengl::OGLRenderDevice>();
#endif
            case OPENGL_4_1_QT:
            case DIRECTX_11:
            case VULKAN:
            default:
                throw std::runtime_error("Graphics backend not supported");
        }
    }
}