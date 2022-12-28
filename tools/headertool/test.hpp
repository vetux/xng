#include "filename.generated.h" // Must be included before headertoolmacros.hpp

int functionCall() {
    return 33;
}

XCOMPONENT(Category="MyComponents/DubDub/AnotherSubMenu")
struct UserComponent : public Component {
    XVARIABLE(Minimum=-5,Maximum=15)
    int integerVariable = functionCall();

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

    XVARIABLE(Description="Lorem Ipsum Lopsum")
    std::string de = std::string("DefaultValue"); // Make sure to initialize strings with std::string and not string literals.

    XVARIABLE(Name="TheVariable")
    ResourceHandle<Texture> da = ResourceHandle<Texture>();

    XVARIABLE(Name="ImportantValue", Description="Very important")
    EntityName Blu;

    float constantVariable = 5l;

    XGENERATED_OPERATORS()
};