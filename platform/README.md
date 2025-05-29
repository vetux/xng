# /platform
Platforms implement the interfaces in the engine api using third party api.

Users can extend the engine by creating new platforms without breaking the main interface.

Platforms are optionally compiled with the engine according to the options set in [cmake/platform.cmake](../cmake/platform.cmake).

Users instantiate the platform implementations by including the corresponding headers in [engine/include/platform](../engine/include/platform) 