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

#include "xng/rendergraph/shader/nodes/nodeor.hpp"
#include "xng/rendergraph/shader/nodes/nodebufferread.hpp"
#include "xng/rendergraph/shader/nodes/nodeliteral.hpp"
#include "xng/rendergraph/shader/nodes/nodetexturesample.hpp"
#include "xng/rendergraph/shader/nodes/nodebufferwrite.hpp"
#include "xng/rendergraph/shader/nodes/nodeassign.hpp"
#include "xng/rendergraph/shader/nodes/nodebuffersize.hpp"
#include "xng/rendergraph/shader/nodes/nodesubscriptarray.hpp"
#include "xng/rendergraph/shader/nodes/nodesubscriptvector.hpp"
#include "xng/rendergraph/shader/nodes/nodesubscriptmatrix.hpp"
#include "xng/rendergraph/shader/nodes/nodeargument.hpp"
#include "xng/rendergraph/shader/nodes/nodevector.hpp"
#include "xng/rendergraph/shader/nodes/nodearray.hpp"
#include "xng/rendergraph/shader/nodes/nodevariablecreate.hpp"
#include "xng/rendergraph/shader/nodes/nodebranch.hpp"
#include "xng/rendergraph/shader/nodes/nodeand.hpp"
#include "xng/rendergraph/shader/nodes/nodeparameter.hpp"
#include "xng/rendergraph/shader/nodes/nodeattributeinput.hpp"
#include "xng/rendergraph/shader/nodes/nodevariable.hpp"
#include "xng/rendergraph/shader/nodes/nodetexturesize.hpp"
#include "xng/rendergraph/shader/nodes/nodeloop.hpp"
#include "xng/rendergraph/shader/nodes/nodeattributeoutput.hpp"
#include "xng/rendergraph/shader/nodes/nodecall.hpp"
#include "xng/rendergraph/shader/nodes/nodereturn.hpp"
#include "xng/rendergraph/shader/nodes/nodebuiltin.hpp"
#include "xng/rendergraph/shader/nodes/comparison/nodelessequal.hpp"
#include "xng/rendergraph/shader/nodes/comparison/nodeless.hpp"
#include "xng/rendergraph/shader/nodes/comparison/nodenequal.hpp"
#include "xng/rendergraph/shader/nodes/comparison/nodeequal.hpp"
#include "xng/rendergraph/shader/nodes/comparison/nodegreaterequal.hpp"
#include "xng/rendergraph/shader/nodes/comparison/nodegreater.hpp"
#include "xng/rendergraph/shader/nodes/arithmetic/nodeadd.hpp"
#include "xng/rendergraph/shader/nodes/arithmetic/nodemultiply.hpp"
#include "xng/rendergraph/shader/nodes/arithmetic/nodesubtract.hpp"
#include "xng/rendergraph/shader/nodes/arithmetic/nodedivide.hpp"

#endif //XENGINE_NODES_HPP