/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
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

#ifndef XENGINE_GLFW_HPP
#define XENGINE_GLFW_HPP

#include "xng/display/displayenvironment.hpp"

namespace xng::glfw {
    /**
     * The graphics apis currently implemented for the glfw display adapter implementation.
     */
    enum GraphicsAPI {
        OPENGL_4_6,
        VULKAN_1_1,
    };

    class XENGINE_EXPORT DisplayEnvironment final : public xng::DisplayEnvironment {
    public:
        DisplayEnvironment();

        ~DisplayEnvironment() override;

        /**
         * Later calls to createWindow will create windows for the given api.
         *
         * @param api The api to set
         */
        void setGraphicsAPI(const GraphicsAPI api) { gpuBackend = api; }

        std::unique_ptr<Monitor> getPrimaryMonitor() override;

        std::set<std::unique_ptr<Monitor> > getMonitors() override;

        std::unique_ptr<Window> createWindow() override;

        std::unique_ptr<Window> createWindow(const std::string &title,
                                             Vec2i size,
                                             WindowAttributes attributes) override;

        std::unique_ptr<Window> createWindow(const std::string &title,
                                             Vec2i size,
                                             WindowAttributes attributes,
                                             Monitor &monitor,
                                             VideoMode mode) override;

        std::vector<std::string> getRequiredVulkanExtensions();

    private:
        std::unique_ptr<Window> makeWindow(GraphicsAPI api,
                                          const std::string &title,
                                          Vec2i size,
                                          WindowAttributes attributes,
                                          Monitor *monitor = nullptr,
                                          VideoMode mode = {}) const;

        GraphicsAPI gpuBackend = OPENGL_4_6;
        std::unique_ptr<Window> primaryWindow; // Hack for opengl. Holds one global context shared with all created opengl windows.
    };
}

#endif //XENGINE_GLFW_HPP
