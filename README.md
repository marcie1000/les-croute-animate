# les-croute-animate

2D platformer game in C and SDL2.

## How to build:

### UNIX (Linux):
You need to have [SDL2](https://wiki.libsdl.org/SDL2/Installation), [SDL2_image](https://github.com/libsdl-org/SDL_image/),
[SDL2_ttf](https://github.com/libsdl-org/SDL_ttf), [SDL2_mixer](https://github.com/libsdl-org/SDL_mixer) and CMake installed.
```console
mkdir build
cd build
cmake .. -G "Unix Makefiles"
make
# Creates a symlink to the assets directory because the program needs it to run
ln -s ../assets assets
```

### Windows:
In the MinGW [MSYS2](https://www.msys2.org/) terminal:
Install the following packages (if needed):
```console
pacman -S --needed  mingw-w64-x86_64-SDL2 \
                    mingw-w64-x86_64-SDL2_image \
                    mingw-w64-x86_64-SDL2_mixer \
                    mingw-w64-x86_64-SDL2_ttf \
                    mingw-w64-clang-x86_64-toolchain  
```
Build commands:
```console
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
mingw32-make
# Creates a symlink to the assets directory because the program needs it to run
ln -s ../assets assets
```
You will need to copy `SDL2.dll`, `SDL2_image.dll`, `SDL2_mixer.dll` and `SDL2_ttf.dll` in the executable directory (you can find it in ` <MSYS path>\msys64\mingw64\bin `) to run.

### Web assembly:
You need to have [Emscripten](https://emscripten.org/docs/getting_started/downloads.html) installed.
```console
mkdir page
emcc anim.c audio.c collisions.c fichiers.c game_scripts.c npc.c script_engine.c sdl_init_and_quit.c switch_events.c textures_fx.c mainloop.c main.c -s WASM=1 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='["png"]' -s USE_SDL_TTF=2 -s USE_SDL_MIXER=2 --preload-file assets -o page/animate.html
```
