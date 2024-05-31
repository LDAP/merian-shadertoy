# merian-shadertoy

A limited Vulkan executor for Shadertoys built using [merian](https://github.com/LDAP/merian).

Shaders are automatically hotreloaded. Compilation errors are printed to the GUI below "shadertoy".

## Usage


```bash
# Clone this repo including submodules
git clone --recursive <url>

# Setup the build directory
meson setup build
# Or: Use a debug build that enables debug logging and extra checks
meson setup build --buildtype=debugoptimized

# Compile
meson compile -C build

# Run
# An example shader is provided in res/shaders/shader.comp.glsl
./build/merian-shadertoy <path/to/shader>

```
