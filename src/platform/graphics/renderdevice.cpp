#include "platform/graphics/renderdevice.hpp"

#include "platform/graphics/opengl/oglrenderdevice.hpp"
#include "platform/graphics/opengl-qt/qtoglrenderdevice.hpp"

namespace engine {
    std::unique_ptr<RenderDevice> RenderDevice::create(GraphicsBackend backend) {
        switch (backend) {
            case OPENGL_4_6:
                return std::make_unique<opengl::OGLRenderDevice>();
            case OPENGL_4_6_QT:
                return std::make_unique<opengl::QtOGLRenderDevice>();
            case DIRECTX_11:
            case VULKAN:
            default:
                throw std::runtime_error("Graphics backend not supported");
        }
    }
}