cmake_minimum_required(VERSION 3.13)
project(HelloSDL-B2D LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# --- SDL3 ---
FetchContent_Declare(
  SDL3
  GIT_REPOSITORY https://github.com/libsdl-org/SDL
  GIT_TAG        main # Se recomienda main para SDL3 ya que está en desarrollo activo
)
# Desactivamos tests y ejemplos de SDL3 para acelerar el build en GitHub
set(SDL_TEST OFF CACHE BOOL "" FORCE)
set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)
# Desactivamos soporte para screensaver
# set(SDL_X11_XINERAMA OFF CACHE BOOL "" FORCE)
# set(SDL_X11_XSCRNSAVER OFF CACHE BOOL "" FORCE)
message(STATUS "Fetching SDL3 Content...")
FetchContent_MakeAvailable(SDL3)

# --- Box2D ---
FetchContent_Declare(
  box2d
  GIT_REPOSITORY https://github.com/erincatto/box2d
  GIT_TAG        v3.1.1  # O una versión específica como v3.0.0
)
# Desactivar tests de Box2D
set(BOX2D_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(BOX2D_BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
message(STATUS "Fetching Box2D Content...")
FetchContent_MakeAvailable(box2d)

# --- EJECUTABLE ---
# Eliminamos 'WIN32' aquí. Para que no salga la consola en Windows,
# se añade como una propiedad opcional abajo.
add_executable(${PROJECT_NAME} HelloSDL-B2D.cpp)

# Si estás en Windows, ocultamos la consola (equivalente a WIN32)
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3-shared box2d::box2d)

# --- POST-BUILD: DLLs y Recursos ---

# 1. Copiar SDL3.dll solo en Windows
if(WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL3::SDL3-shared>
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMENT "Copiando SDL3.dll al directorio de salida..."
    )
endif()

# 2. Copiar Recursos (images y fonts)
# Usamos un if para verificar que las carpetas existen y evitar errores en el Action
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/images"
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/fonts"

    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/images"
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/images"

    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/fonts"
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/fonts"

    COMMENT "Actualizando recursos (images/ y fonts/) en la carpeta build..."
)
