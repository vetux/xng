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

#include "xng/rendergraph/shader/nodes/fgnodeor.hpp"
#include "xng/rendergraph/shader/nodes/fgnodebufferread.hpp"
#include "xng/rendergraph/shader/nodes/fgnodeliteral.hpp"
#include "xng/rendergraph/shader/nodes/fgnodetexturesample.hpp"
#include "xng/rendergraph/shader/nodes/fgnodebufferwrite.hpp"
#include "xng/rendergraph/shader/nodes/fgnodeassign.hpp"
#include "xng/rendergraph/shader/nodes/fgnodebuffersize.hpp"
#include "xng/rendergraph/shader/nodes/fgnodesubscriptarray.hpp"
#include "xng/rendergraph/shader/nodes/fgnodesubscriptvector.hpp"
#include "xng/rendergraph/shader/nodes/fgnodesubscriptmatrix.hpp"
#include "xng/rendergraph/shader/nodes/fgnodeargument.hpp"
#include "xng/rendergraph/shader/nodes/fgnodevector.hpp"
#include "xng/rendergraph/shader/nodes/fgnodearray.hpp"
#include "xng/rendergraph/shader/nodes/fgnodevariablecreate.hpp"
#include "xng/rendergraph/shader/nodes/fgnodebranch.hpp"
#include "xng/rendergraph/shader/nodes/fgnodeand.hpp"
#include "xng/rendergraph/shader/nodes/fgnodeparameter.hpp"
#include "xng/rendergraph/shader/nodes/fgnodeattributeinput.hpp"
#include "xng/rendergraph/shader/nodes/fgnodevariable.hpp"
#include "xng/rendergraph/shader/nodes/fgnodetexturesize.hpp"
#include "xng/rendergraph/shader/nodes/fgnodeloop.hpp"
#include "xng/rendergraph/shader/nodes/fgnodeattributeoutput.hpp"
#include "xng/rendergraph/shader/nodes/fgnodecall.hpp"
#include "xng/rendergraph/shader/nodes/fgnodereturn.hpp"
#include "xng/rendergraph/shader/nodes/fgnodebuiltin.hpp"
#include "xng/rendergraph/shader/nodes/comparison/fgnodelessequal.hpp"
#include "xng/rendergraph/shader/nodes/comparison/fgnodeless.hpp"
#include "xng/rendergraph/shader/nodes/comparison/fgnodenequal.hpp"
#include "xng/rendergraph/shader/nodes/comparison/fgnodeequal.hpp"
#include "xng/rendergraph/shader/nodes/comparison/fgnodegreaterequal.hpp"
#include "xng/rendergraph/shader/nodes/comparison/fgnodegreater.hpp"
#include "xng/rendergraph/shader/nodes/arithmetic/fgnodeadd.hpp"
#include "xng/rendergraph/shader/nodes/arithmetic/fgnodemultiply.hpp"
#include "xng/rendergraph/shader/nodes/arithmetic/fgnodesubtract.hpp"
#include "xng/rendergraph/shader/nodes/arithmetic/fgnodedivide.hpp"

#endif //XENGINE_NODES_HPP