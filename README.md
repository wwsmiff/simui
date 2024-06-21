# SIMUI

**S**DL **I**mmediate **M**ode **UI**

This is an immediate mode UI library for SDL2. It can be used within SDL2 applications or as a standalone UI library.
Creating and destroying the `SDL_Window` and `SDL_Renderer` and events specific to the user-created `SDL_Window`
must be handled by the user. The UI rendering and UI events are handled by `simui` itself.

# Requirements
- C compiler
- SDL2

# Build and run
```
git clone https://github.com/wwsmiff/simui
cd simui
mkdir build
./build.sh
./build/main
```
