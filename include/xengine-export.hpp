#ifndef MANA_ENGINE_EXPORT_HPP
#define MANA_ENGINE_EXPORT_HPP

// Include this header in user code before including any other engine headers,
// This header defines the export variable to be none which is what the user wants when linking to the engine.
// Alternatively (More practical) the symbol can be defined in the build files.
#define MANA_EXPORT

#endif //MANA_ENGINE_EXPORT_HPP
