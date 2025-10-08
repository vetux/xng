# Shader Script

Shader Script is a custom Shader Domain Specific Language for writing render graph shaders.

It is implemented through headers which define various types, functions and macros to create an easy-to-use DSL that resembles GLSL pretty closely.

### Differences to GLSL
#### Type Definition
  - Literal types are uppercase (e.g. `int` in glsl becomes `Int` in C++)
  - Arrays are instantiated like this `ArrayX<COUNT> b = std::vector{VALUES...}`
  - User Defined types are instantiated like this `Object<TYPENAME> o`

#### Variable Initialization
  - Default constructed variables are undefined just like in glsl e.g. `vec2 v;` produces an uninitialized variable that cannot be used until it is assigned a value.
  - If a variable is assigned at initialization using a prvalue e.g. `vec2 v = vec2(1, 1)` it cannot be assigned again and is inlined. (See ShaderObject::assignValue() for more details)
  - Variables used in subsequent branches or loops must be initialized before the branch / loop body.

#### Type promotion in arithmetic expressions
  - Currently, types are not automatically promoted inside expressions e.g. `vec2 v = ivec2(1, 1) / 1.0f` results in a compile error.

#### Swizzling
  - Vector swizzling is done with functions (e.g. `vec.xyx` in glsl becomes `vec.xyx()` in C++)
  - Vector initialization can use implicit swizzling (e.g. `vec4 v = vec4(vec2(0, 0), vec2(0, 0))`)

#### Subscripting
  - Matrix subscripting has to be done with the `element()` method (e.g. `mat[column][row]` in glsl becomes `mat.element(column, row)` in C++)

#### Resource Access
  - Input attributes are accessed through `ShaderScript::inputAttribute`  or alternatively via the `Input` macro
  - Output attributes are accessed through `ShaderScript::outputAttribute`  or alternatively via the `Output` macro
  - Buffers are accessed through `ShaderScript::buffer` or alternatively via the `Buffer` macro
  - Textures are accessed through `ShaderScript::texture` or alternatively via the `Texture` macro
  - Parameters are accessed through `ShaderScript::parameter`  or alternatively via the `Parameter` macro
  - User-defined function arguments are accessed through `ShaderScript::argument` or alternatively via the `Argument` macro

#### Control Flow
  - Conditionals and Branches are defined through `ShaderScript::If` / `ShaderScript::Else` / `ShaderScript::EndIf` / `ShaderScript::For` / `ShaderScript::EndFor`
  - Functions can be defined through `ShaderScript::Function` / `ShaderScript::EndFunction`
  - User-defined functions can be called through `ShaderScript::Call` (e.g. `test(1)` in glsl becomes `Call("test", {1})` in C++)
  - Functions are returned from through `ShaderScript::Return` (e.g. `return 5;` in glsl becomes `Return(5)` in C++)