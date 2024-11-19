# merian-shadertoy

A limited Vulkan executor for Shadertoys built using [merian](https://github.com/LDAP/merian).

Shaders are automatically hotreloaded. Compilation errors are printed to the GUI below "shadertoy".

## Requirements

- A fairly recent C++ compiler
- A GPU with Vulkan support
- shaderc

## Usage


```bash
# Clone this repo including submodules
git clone --recursive <url>
cd merian-shadertoy

# Setup the build directory
meson setup build
# Or: Use a debug build that enables debug logging and extra checks
meson setup build --buildtype=debugoptimized

# Compile
meson compile -C build

# Run
# enable dev env if you do not want to install
meson devenv -C build
./build/merian-shadertoy
```
