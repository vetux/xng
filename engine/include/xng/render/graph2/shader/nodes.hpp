/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2023  Julian Zampiccoli
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

#ifndef XENGINE_NODES_HPP
#define XENGINE_NODES_HPP

#include "xng/render/graph2/shader/nodes/fgnodeor.hpp"
#include "xng/render/graph2/shader/nodes/fgnodebufferread.hpp"
#include "xng/render/graph2/shader/nodes/fgnodeliteral.hpp"
#include "xng/render/graph2/shader/nodes/fgnodetexturesample.hpp"
#include "xng/render/graph2/shader/nodes/fgnodebufferwrite.hpp"
#include "xng/render/graph2/shader/nodes/fgnodesubscript.hpp"
#include "xng/render/graph2/shader/nodes/fgnodevector.hpp"
#include "xng/render/graph2/shader/nodes/fgnodearraylength.hpp"
#include "xng/render/graph2/shader/nodes/fgnodebranch.hpp"
#include "xng/render/graph2/shader/nodes/fgnodeand.hpp"
#include "xng/render/graph2/shader/nodes/fgnodeparameterread.hpp"
#include "xng/render/graph2/shader/nodes/fgnodeattributeread.hpp"
#include "xng/render/graph2/shader/nodes/fgnodetexturesize.hpp"
#include "xng/render/graph2/shader/nodes/fgnodeloop.hpp"
#include "xng/render/graph2/shader/nodes/fgnodeattributewrite.hpp"
#include "xng/render/graph2/shader/nodes/builtin/fgnodenormalize.hpp"
#include "xng/render/graph2/shader/nodes/comparison/fgnodelessequal.hpp"
#include "xng/render/graph2/shader/nodes/comparison/fgnodeless.hpp"
#include "xng/render/graph2/shader/nodes/comparison/fgnodenequal.hpp"
#include "xng/render/graph2/shader/nodes/comparison/fgnodeequal.hpp"
#include "xng/render/graph2/shader/nodes/comparison/fgnodegreaterequal.hpp"
#include "xng/render/graph2/shader/nodes/comparison/fgnodegreater.hpp"
#include "xng/render/graph2/shader/nodes/arithmetic/fgnodeadd.hpp"
#include "xng/render/graph2/shader/nodes/arithmetic/fgnodemultiply.hpp"
#include "xng/render/graph2/shader/nodes/arithmetic/fgnodesubtract.hpp"
#include "xng/render/graph2/shader/nodes/arithmetic/fgnodedivide.hpp"

#endif //XENGINE_NODES_HPP