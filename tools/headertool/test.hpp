#include "filename.generated.h" // Must be included before headertoolmacros.hpp

XCOMPONENT(Category="MyComponents/DubDub/AnotherSubMenu")
struct UserComponent : public Component {
    XVARIABLE(Minimum=-5,Maximum=15)
    int integerVariable;

    XVARIABLE()
    std::string strVariable = "DefaultValue";

    XVARIABLE()
    ResourceHandle<std::vector<Vector3<int>>, std::string<float>> textureVariable = ResourceHandle<Texture>(asdf, ff);

    XVARIABLE()
    EntityName entityVariable;

    float constantVariable = 5l;

    XGENERATED_OPERATORS()
};

XCOMPONENT(Category="MyComponents/SubMenu/MyOtherSub")
struct OtherComponent : public Component {
    XVARIABLE(Minimum=-5,Maximum=15)
    int sd;

    XVARIABLE()
    std::string de = "DefaultValue";

    XVARIABLE()
    ResourceHandle<std::vector<Vector3<int>>, std::string<float>> da = ResourceHandle<Texture>(asdf, ff);

    XVARIABLE()
    EntityName Blu;

    float constantVariable = 5l;

    XGENERATED_OPERATORS()
};