# Versión mínima de CMake requerida
cmake_minimum_required(VERSION 3.13)

# Nombre del proyecto
project(SDL3-Box2D LANGUAGES CXX)

# Establecer el estándar C++ (usa un estándar moderno)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Directorio base donde se encuentra la librería SDL y Box2D
set(SDL_C_ROOT "c:/_Libz/sdl")
set(BOX2D_CPP_ROOT "c:/_Libz/box2d")

# Añade ejecutables
add_executable(HelloSDL HelloSDL.cpp)
add_executable(HelloBox2D HelloBox2D.cpp)

# =========================================================
# 1. Incluir directorios y librerías de SDL y Box2D
# =========================================================

# Incluir los directorios de cabecera (headers)
# SDL  y Box2D (donde están los archivos .h)
target_include_directories(HelloSDL PRIVATE ${SDL_C_ROOT}/include)
target_include_directories(HelloBox2D PRIVATE ${BOX2D_CPP_ROOT}/include)


# Directorio donde se encuentran los archivos .lib o .a
target_link_directories(HelloSDL PRIVATE ${SDL_C_ROOT}/lib)
target_link_directories(HelloBox2D PRIVATE ${BOX2D_CPP_ROOT}/lib)

# =========================================================
# 2. Definir los ejecutables y sus dependencias
# =========================================================

## Build the example apps
# Enlazar las librerías necesarias con el ejecutable
target_link_libraries(HelloSDL PRIVATE sdl3)
target_link_libraries(HelloBox2D PRIVATE box2d)
