#ifndef XENGINE_ENGINE_EXPORT_HPP
#define XENGINE_ENGINE_EXPORT_HPP

// Include this header in user code before including any other engine headers,
// This header defines the export variable to be none which is what the user wants when linking to the engine.
// Alternatively (More practical) the XENGINE_EXPORT symbol can be defined by the build system.
#define XENGINE_EXPORT

#endif //XENGINE_ENGINE_EXPORT_HPP
