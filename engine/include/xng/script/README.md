## Script Abstraction
The idea is that there is a system which synchronizes the c++ ecs state with some scripting environment and call user scripts in that environment that interact with the state.

Because the scripting environment requires much of the data classes to be rewritten for the scripting language and because
serialization / deserialization from / to the scripting environment is expensive and hard to scale I have decided to keep the scripting feature on hold.