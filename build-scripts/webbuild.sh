cd ..
mkdir page
emcc anim.c audio.c collisions.c fichiers.c game_scripts.c npc.c script_engine.c sdl_init_and_quit.c switch_events.c textures_fx.c mainloop.c main.c -s WASM=1 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='["png"]' -s USE_SDL_TTF=2 -s USE_SDL_MIXER=2 --preload-file assets -o page/animate.js
