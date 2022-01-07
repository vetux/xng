/**
 *  Mana - 3D Game Engine
 *  Copyright (C) 2021  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef XENGINE_COMPOSITOR_HPP
#define XENGINE_COMPOSITOR_HPP

#include "geometrybuffer.hpp"

#include "platform/graphics/rendercommand.hpp"

namespace xengine {
    /**
     * The compositor creates the final image on the screen by combining textures from the geometry buffer.
     */
    class XENGINE_EXPORT Compositor {
    public:
        struct XENGINE_EXPORT Layer {
            Layer() = default;

            Layer(std::string name,
                  std::string color,
                  std::string depth,
                  DepthTestMode depthTestMode = DEPTH_TEST_LESS,
                  BlendMode colorBlendModeSource = SRC_ALPHA,
                  BlendMode colorBlendModeDest = ONE_MINUS_SRC_ALPHA)
                    : name(std::move(name)),
                      color(std::move(color)),
                      depth(std::move(depth)),
                      depthTestMode(depthTestMode),
                      colorBlendModeSource(colorBlendModeSource),
                      colorBlendModeDest(colorBlendModeDest) {}

            Layer(const Layer &other) = default;

            std::string name;

            // The names of the color texture in the geometry buffer
            std::string color;

            //The optional name of the depth texture in the geometry buffer
            std::string depth;

            //The depth test mode to use when rendering this layer
            DepthTestMode depthTestMode = DEPTH_TEST_LESS;

            //The blend modes to use when rendering this layer
            BlendMode colorBlendModeSource = SRC_ALPHA;
            BlendMode colorBlendModeDest = ONE_MINUS_SRC_ALPHA;
        };

        Compositor(RenderDevice &device, std::vector<Layer> layers);

        std::vector<Layer> &getLayers();

        void setLayers(const std::vector<Layer> &layers);

        void setClearColor(ColorRGB color);

        void presentLayers(RenderTarget &screen, GeometryBuffer &buffer);

        void presentLayers(RenderTarget &screen, GeometryBuffer &buffer, const std::vector<Layer> &pLayers);

    private:
        void drawLayer(RenderTarget &screen,
                       GeometryBuffer &buffer,
                       const Layer &layer);

        ColorRGB clearColor{0, 0, 0};
        RenderDevice &device;
        std::vector<Layer> layers;
        std::unique_ptr<ShaderProgram> shader;
    };
}

#endif //XENGINE_COMPOSITOR_HPP
