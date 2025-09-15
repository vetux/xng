# Shader Script

Shader Script is a custom Shader Domain Specific Language for writing render graph shaders.

It is implemented through headers which define various types and functions to create an easy-to-use DSL that resembles GLSL pretty closely.

### Differences to GLSL
#### Type Definition
  - Literal types are uppercase (e.g. `int` in glsl becomes `Int` in C++)
  - Arrays are defined as `ArrayX<COUNT> b = ArrayX<COUNT>{VALUES...}`
  - If a variable is assigned at initialization e.g. `vec2 v = vec2(1, 1)` it might be inlined in the resulting shader. (See `ShaderNodeWrapper::promoteToVariable` for more details)
    - Default initialization can be used to avoid inlining e.g. `vec2 v; v = vec2(1, 1)`. This is currently required if the variable is assigned to in a subsequent conditional or branch.  

#### Type promotion in arithmetic expressions
  - Currently, types are not automatically promoted inside expressions e.g. `vec2 v = ivec2(1, 1) / 1.0f` results in a compile error.

#### Swizzling
  - Vector swizzling is done with functions (e.g. `vec.xyx` in glsl becomes `vec.xyx()` in C++)

#### Subscripting
  - Matrix subscripting has to be done with the `elements()` method (e.g. `mat[column][row]` in glsl becomes `mat.element(column, row)` in C++)

#### Input Data 
  - Input attributes are accessed through `ShaderScript::attribute` 
  - Buffers are accessed through `ShaderScript::buffer` and `ShaderScript::dynamicBuffer` 
  - Parameters are accessed through `ShaderScript::parameter` 
  - User-defined function arguments are accessed through `ShaderScript::argument`

#### Output Data
  - Writing attributes is done through `ShaderScript::writeAttribute`
  - Writing buffers is done through `ShaderScript::writeBuffer` / `ShaderScript::writeDynamicBuffer`

#### Control Flow
  - Conditionals and Branches are defined through `ShaderScript::If` / `ShaderScript::Else` / `ShaderScript::EndIf` / `ShaderScript::For` / `ShaderScript::EndFor`
  - Functions can be defined through `ShaderScript::Function` / `ShaderScript::EndFunction`
  - User-defined functions can be called through `ShaderScript::Call` (e.g. `test(1)` in glsl becomes `Call("test", {1})` in C++)
  - Functions are returned from through `ShaderScript::Return` (e.g. `return 5;` in glsl becomes `Return(5)` in C++)