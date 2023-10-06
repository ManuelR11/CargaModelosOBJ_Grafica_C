cmake_minimum_required(VERSION 3.25)
project(CargaModelosOBJ_Grafica_C)



set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SDL2_INCLUDE_DIR "C:/Libs/include")
set(SDL2_LIB_DIR "C:/Libs/lib/x64")

# Agregar la ruta de inclusión de la biblioteca glm
set(GLM_INCLUDE_DIR "C:/Users/rodas/Desktop/CODING11/Repoitorios_GIT/CargaModelosOBJ_Grafica_C/glm")

# Utiliza GLM_INCLUDE_DIR en lugar de CMAKE_INCLUDE_PATH
include_directories(${SDL2_INCLUDE_DIR} ${GLM_INCLUDE_DIR})
link_directories(${SDL2_LIB_DIR})

add_executable(CargaModelosOBJ_Grafica_C main.cpp)

target_link_libraries(CargaModelosOBJ_Grafica_C SDL2main SDL2)