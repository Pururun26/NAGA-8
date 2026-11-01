cmake_minimum_required(VERSION 3.20)
project(naga8 C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# --- пути к SDK ---
set(RAYLIB_DIR   "${CMAKE_SOURCE_DIR}/raylib/raylib-6.0_win64_mingw-w64")
set(POCKETPY_DIR "${CMAKE_SOURCE_DIR}/pocketpy")

# --- pocketpy как статическая библиотека ---
add_library(pocketpy STATIC ${POCKETPY_DIR}/pocketpy.c)
target_include_directories(pocketpy PUBLIC ${POCKETPY_DIR})

# отключаем доступ к ОС (файлы, сеть) — нужно для sandbox и убирает ws2_32
target_compile_definitions(pocketpy PRIVATE PK_ENABLE_OS=0)

# --- исполняемый файл ---
add_executable(naga8
    src/naga8.c
    src/naga8_api.c
    src/naga8_api_py.c
)

target_include_directories(naga8 PRIVATE
    ${RAYLIB_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
    ${POCKETPY_DIR}
)

target_link_directories(naga8 PRIVATE ${RAYLIB_DIR}/lib)

target_link_libraries(naga8 PRIVATE
    raylib
    pocketpy
    opengl32
    gdi32
    winmm
)