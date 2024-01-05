cmake_minimum_required(VERSION 3.1)
set(CMAKE_CXX_STANDARD 17)
project(ImFileDialogExample)

# source code
set(SOURCES
	example.cpp
	ImFileDialog.cpp

	extern/imgui/imgui.cpp
	extern/imgui/imgui_widgets.cpp
	extern/imgui/imgui_draw.cpp
	extern/imgui/imgui_tables.cpp
	extern/imgui/backends/imgui_impl_glfw.cpp
	extern/imgui/backends/imgui_impl_opengl3.cpp
	extern/tinyexr/tinyexr.cc
	extern/tinyexr/deps/miniz/miniz.c
)

include_directories(extern)
include_directories(extern/imgui)
include_directories(extern/stb)
include_directories(extern/tinyexr)
include_directories(extern/tinyexr/deps/miniz)
include_directories(extern/glfw/install/include)

link_directories(extern/glfw/install/lib)

# cmake toolchain
if(CMAKE_TOOLCHAIN_FILE)
    include(${CMAKE_TOOLCHAIN_FILE})
endif(CMAKE_TOOLCHAIN_FILE)

# pkg
if (UNIX)
  find_package(PkgConfig REQUIRED)
endif()

# glfw3
if(WIN32 OR APPLE OR NOT ${USE_PKG_CHECK_GLFW3})
  find_package(GLFW3 REQUIRED)
else()
  pkg_check_modules(GLFW3 REQUIRED GLFW3)
endif()

# opengl
set(OpenGL_GL_PREFERENCE GLVND CACHE STRING "Linux only: if GLVND, use the vendor-neutral GL libraries (default). If LEGACY, use the legacy ones (might be necessary to have working optirun/primusrun)")
set_property(CACHE OpenGL_GL_PREFERENCE PROPERTY STRINGS GLVND LEGACY)
find_package(OpenGL REQUIRED)

# create executable
add_executable(ImFileDialogExample ${SOURCES})

# properties
set_target_properties(ImFileDialogExample PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
)

# include directories
target_include_directories(ImFileDialogExample PRIVATE ${GLFW3_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIRS})
#target_include_directories(ImFileDialogExample PRIVATE libs libs/imgui)

# link libraries
target_link_libraries(ImFileDialogExample ${OPENGL_LIBRARIES})
target_link_libraries(ImFileDialogExample GLFW3 ${GLFW3_LIBRARIES} ${CMAKE_DL_LIBS})
