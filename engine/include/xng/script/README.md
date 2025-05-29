## Script Abstraction
The idea is that there is a system which synchronizes the native ECS state with some scripting environment (For example C# or Python) and call user scripts in that environment that interact with the state.

# Challenges
- Type definitions of the engine have to be duplicated in the scripting language to allow the scripting environment to interact with the native data.
- Serialization / Deserialization from / to the scripting environment is expensive and hard to scale
