# Build Configuration

Create a file called <code>user-config-toolchain.cmake</code> in this directory. Available variables are:

MINGW_ENV_BASE_DIR - Set by default to /usr/ which is compatible with common linux distributions package manager installation of mingw.

For each ABI you can create a file called <code>user-config-project.cmake</code> inside the corresponding ABI folder
to run cmake commands for the abi project to specify driver dependency headers and link directories per ABI.