# Versión mínima de CMake requerida
cmake_minimum_required(VERSION 3.13)

# Nombre del proyecto
project(HelloSDL-B2D LANGUAGES CXX)

# Establecer el estándar C++ (usa un estándar moderno)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Cargar el módulo FetchContent
include(FetchContent)

# SDL3
FetchContent_Declare(
  SDL3
  GIT_REPOSITORY https://github.com/libsdl-org/SDL
  GIT_TAG        release-3.4.0  # Se puede usar un hash específico (main) o una versión como 'release-3.2.0'
)
message(STATUS "Fetching SDL3 Content...")
FetchContent_MakeAvailable(SDL3)

# Box2D
FetchContent_Declare(
  box2d
  GIT_REPOSITORY https://github.com/erincatto/box2d
  GIT_TAG        v3.1.1  # O una versión específica como v3.0.0
)
message(STATUS "Fetching Box2D Content...")
FetchContent_MakeAvailable(box2d)

# Añade ejecutables
add_executable(${PROJECT_NAME} WIN32 HelloSDL-B2D.cpp)

# FetchContent exporta el target como 'SDL3::SDL3-shared' o 'SDL3::SDL3-static'
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3-shared box2d::box2d)

# Copia la DLL de SDL3 al directorio del ejecutable automáticamente
set(DLLS_TO_COPY
    SDL3::SDL3-shared
)
foreach(LIB ${DLLS_TO_COPY})
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:${LIB}>
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
endforeach()

# Copia recursos (Imágenes y Fuentes)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    # Copia la carpeta de imágenes
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/images"
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/images"

    # Copia la carpeta de fuentes
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/fonts"
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/fonts"

    COMMENT "Actualizando recursos (images/ y fonts/) en la carpeta build..."
)
