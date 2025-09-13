# Shader Script

Shader Script is a custom Shader Domain Specific Language for writing render graph shaders.

It is implemented through headers which define various types and functions to create an easy-to-use DSL that resembles GLSL pretty closely.

### Differences to GLSL
#### Type Definition
  - Literal types are uppercase (e.g. `int` in glsl becomes `Int` in C++)
  - Arrays are defined as <code>ArrayX<COUNT> b = ArrayX<COUNT>{VALUES...}</code>
  - If a variable is assigned at initialization eg `vec2 v = vec2(1, 1)` it might be inlined in the resulting shader. (See `ShaderNodeWrapper::promoteToVariable` for more details)
    - Default initalization can be used to avoid inlining eg `vec2 v; v = vec2(1, 1)`  

#### Subscripting
  - Vector subscripting are functions (e.g. <code>vec.x</code> in glsl becomes `vec.x()` in C++)
  - To assign to a subscripted value, users must use `setX` or `setElement` (e.g. `vec.x = 1` in glsl becomes `vec.setX(1)` in C++ 
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
  - Conditionals and Branches are defined through `ShaderBuilder::If` / `ShaderBuilder::Else` / `ShaderBuilder::EndIf` / `ShaderBuilder::For` / `ShaderBuilder::EndFor`
  - Functions can be defined through `ShaderBuilder::Function` / `ShaderBuilder::EndFunction`
  - User-defined functions can be called through `ShaderScript::Call` (e.g. `test(1)` in glsl becomes `Call("test", {1})` in C++)
  - Functions are returned from through `ShaderScript::Return` (e.g. `return 5;` in glsl becomes `Return(5)` in C++)