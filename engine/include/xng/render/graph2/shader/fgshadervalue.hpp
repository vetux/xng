#ifndef XENGINE_FGSHADERVARIABLE_HPP
#define XENGINE_FGSHADERVARIABLE_HPP

#include <string>

#include "xng/render/graph2/shader/fgshaderliteral.hpp"
#include "xng/render/graph2/shader/fgshadervariable.hpp"

namespace xng {
    typedef std::variant<FGShaderVariable, FGShaderLiteral> FGShaderValue;
}

#endif //XENGINE_FGSHADERVARIABLE_HPP