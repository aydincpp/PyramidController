cmake_minimum_required(VERSION 3.16)
project(PyramidController LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(EXTERNAL "${CMAKE_SOURCE_DIR}/external")
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")

# -- GLM ---
add_subdirectory("${EXTERNAL}/glm")

# --- GLFW ---
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
add_subdirectory("${EXTERNAL}/glfw")

# --- GLAD ---
add_library(glad "${EXTERNAL}/glad/src/glad.c")
target_include_directories(glad PUBLIC "${EXTERNAL}/glad/include")

# --- IMGUI ---
set(IMGUI_DIR "${EXTERNAL}/imgui")

set(IMGUI_SOURCES
    "${IMGUI_DIR}/imgui.cpp"
    "${IMGUI_DIR}/imgui_demo.cpp"
    "${IMGUI_DIR}/imgui_draw.cpp"
    "${IMGUI_DIR}/imgui_tables.cpp"
    "${IMGUI_DIR}/imgui_widgets.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_glfw.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp"
)

add_library(imgui ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC
    "${IMGUI_DIR}"
    "${IMGUI_DIR}/backends"
)
target_link_libraries(imgui PUBLIC glfw)

# --- MAIN EXECUTABLE ---
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS src/*.cpp src/*.h)
add_executable(${CMAKE_PROJECT_NAME} ${SRC_FILES})
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${INCLUDE_DIR} ${EXTERNAL}/glm)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE glfw glad imgui)

if(WIN32)
    target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE opengl32)
elseif(APPLE)
    find_library(OpenGL_LIBRARY OpenGL)
    target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE ${OpenGL_LIBRARY})
else() # Linux
    target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE GL)
endif()
