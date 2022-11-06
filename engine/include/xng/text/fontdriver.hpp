/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2022  Julian Zampiccoli
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

#ifndef XENGINE_FONTDRIVER_HPP
#define XENGINE_FONTDRIVER_HPP

#include "xng/driver/driver.hpp"
#include "font.hpp"

namespace xng {
    enum FontDriverBackend {
        FREETYPE
    };

    class XENGINE_EXPORT FontDriver : public Driver {
    public:
        static std::unique_ptr<FontDriver> load(FontDriverBackend backend) {
            switch (backend) {
                case FREETYPE:
                    return std::unique_ptr<FontDriver>(
                            dynamic_cast<FontDriver *>(Driver::load("freetype").release()));
            }
            throw std::runtime_error("Invalid backend");
        }

        virtual std::unique_ptr<Font> createFont(std::istream &data) = 0;

    private:
        std::type_index getBaseType() override {
            return typeid(FontDriver);
        }
    };
}
#endif //XENGINE_FONTDRIVER_HPP
