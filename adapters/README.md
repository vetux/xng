# /adapters
Adapters implement target interfaces in the engine api using third party api.

Users can extend the engine by creating new adapters without breaking the main interface.

Adapters are optionally compiled with the engine according to the options set in [cmake/adapters.cmake](../cmake/adapters.cmake).

Users instantiate the adapter implementations by including the corresponding headers in [engine/include/adapters](../engine/include/platform) 