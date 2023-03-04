/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2023  Julian Zampiccoli
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

#ifndef XENGINE_HEADERPARSER_HPP
#define XENGINE_HEADERPARSER_HPP

#include <vector>
#include <functional>

#include "xng/headertool/componentmetadata.hpp"
#include "xng/headertool/token.hpp"

#include "xng/headertool/syntaxexception.hpp"

namespace xng {
    class XENGINE_EXPORT HeaderParser {
    public:
        /**
         * Parse the list of tokens and return all available component metadata declarations.
         *
         * @param fileName The string describing the source of the tokens, Used when building SyntaxExceptions
         * @param tokens The list of tokens to parse
         * @return
         */
        std::vector<ComponentMetadata> parseTokens(const std::string &fileName,
                                                   const std::vector<Token> &tokens);
    };
}
#endif //XENGINE_HEADERPARSER_HPP
