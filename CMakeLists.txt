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
add_executable(HelloSDL-B2D HelloSDL-B2D.cpp)

# =========================================================
# 1. Incluir directorios y librerías de SDL y Box2D
# =========================================================

# Incluir los directorios de cabecera (headers)
# SDL  y Box2D (donde están los archivos .h)
target_include_directories(HelloSDL-B2D PRIVATE ${BOX2D_CPP_ROOT}/include ${SDL_C_ROOT}/include)


# Directorio donde se encuentran los archivos .lib o .a
target_link_directories(HelloSDL-B2D PRIVATE ${BOX2D_CPP_ROOT}/lib ${SDL_C_ROOT}/lib)

# =========================================================
# 2. Definir los ejecutables y sus dependencias
# =========================================================

## Build the example apps
# Enlazar las librerías necesarias con el ejecutable
target_link_libraries(HelloSDL-B2D PRIVATE box2d sdl3)

# =========================================================
# 3. Copiar recursos (Imágenes y Fuentes)
# =========================================================

add_custom_command(TARGET HelloSDL-B2D POST_BUILD
    # Copia la carpeta de imágenes
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/images"
    "$<TARGET_FILE_DIR:HelloSDL-B2D>/images"
    
    # Copia la carpeta de fuentes
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/fonts"
    "$<TARGET_FILE_DIR:HelloSDL-B2D>/fonts"
    
    COMMENT "Actualizando recursos (images/ y fonts/) en la carpeta build..."
)
