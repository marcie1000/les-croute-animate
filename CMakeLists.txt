cmake_minimum_required (VERSION 3.10.0)

project (animate)

# Add cmake directory to the cmake module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

# Add project files
set (HEADERS
    anim.h
    audio.h
    collisions.h
    enumerations.h
    fichiers.h
    game_scripts.h
    npc.h
    script_engine.h
    sdl_init_and_quit.h
    switch_events.h
    textures_fx.h
    types_struct_defs.h
    mainloop.h
)

set (SOURCES
    anim.c
    audio.c
    collisions.c
    fichiers.c
    game_scripts.c
    npc.c
    script_engine.c
    sdl_init_and_quit.c
    switch_events.c
    textures_fx.c
    mainloop.c
    main.c
)

find_package (SDL2 REQUIRED)
find_package (SDL2_image REQUIRED)
find_package (SDL2_ttf REQUIRED)
find_package (SDL2_mixer REQUIRED)

# ============= SDL2 ============
if (NOT SDL2_FOUND)
    message (FATAL_ERROR "Error: SDL2 not found.")
else ()
    message (STATUS "SDL2 include directory : ${SDL2_INCLUDE_DIRS}")
    message (STATUS "SDL2 library directory : ${SDL2_LIBRARIES}")
    include_directories(${SDL2_INCLUDE_DIRS})
    link_directories(${SDL2_LIBRARIES})
endif (NOT SDL2_FOUND)

# =========== SDL2_image ===========
if (NOT SDL2_image_FOUND)
    message (FATAL_ERROR "Error: SDL2_image not found.")
else ()
    message (STATUS "SDL2_image include directory : ${SDL2_IMAGE_INCLUDE_DIRS}")
    message (STATUS "SDL2_image library directory : ${SDL2_IMAGE_LIBRARIES}")
    include_directories(${SDL2_IMAGE_INCLUDE_DIRS})
    link_directories(${SDL2_IMAGE_LIBRARIES})
endif (NOT SDL2_image_FOUND)

# =========== SDL2_ttf =============
if (NOT SDL2_ttf_FOUND)
    message (FATAL_ERROR "Error: SDL2_ttf not found.")
else ()
    message (STATUS "SDL2_ttf include directory : ${SDL2_TTF_INCLUDE_DIRS}")
    message (STATUS "SDL2_ttf library directory : ${SDL2_TTF_LIBRARIES}")
    include_directories(${SDL2_TTF_INCLUDE_DIRS})
    link_directories(${SDL2_TTF_LIBRARIES})
endif (NOT SDL2_ttf_FOUND)

# =========== SDL2_mixer ============
if (NOT SDL2_mixer_FOUND)
    message (FATAL_ERROR "Error: SDL2_mixer not found.")
else ()
    message (STATUS "SDL2_mixer include directory : ${SDL2_MIXER_INCLUDE_DIRS}")
    message (STATUS "SDL2_mixer library directory : ${SDL2_MIXER_LIBRARIES}")
    include_directories(${SDL2_MIXER_INCLUDE_DIRS})
    link_directories(${SDL2_MIXER_LIBRARIES})
endif (NOT SDL2_mixer_FOUND)

set (CMAKE_C_STANDARD 11)
add_executable (animate ${SOURCES} ${HEADERS})
target_link_libraries (animate ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} ${SDL2_TTF_LIBRARIES} ${SDL2_MIXER_LIBRARIES})




