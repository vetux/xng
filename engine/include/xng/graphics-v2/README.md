# Graphics v2
Pass shaders are written in ShaderScript.

The ShaderScript usage incurs an additional compilation layer because shaders are parsed or "compiled" to the 
Rendergraph IR at runtime.

However, Users can avoid this overhead by precompiling the pass shaders and load them from disk at runtime.

Additionally the hardware shaders can be cached / saved to disk by using the Rendergraph Pipeline Cache interface.

Currently materials are bindless only.
In the future i might implement material permutations, e.g. compiling separate pipelines for each material combination.

All scene data can be streamed asynchronously.
