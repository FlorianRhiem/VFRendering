cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(VFRendering VERSION 0.0.0)

include(ExternalProject)
ExternalProject_add(qhullstatic_r
	GIT_REPOSITORY https://github.com/qhull/qhull.git
	CMAKE_ARGS "-DCMAKE_INSTALL_PREFIX=${PROJECT_BINARY_DIR}/thirdparty-install;-Dqhull_TARGETS_INSTALL=qhullstatic_r"
	PREFIX qhull-prefix
)
ExternalProject_add(qhullcpp
	GIT_REPOSITORY https://github.com/qhull/qhull.git
	CMAKE_ARGS "-DCMAKE_INSTALL_PREFIX=${PROJECT_BINARY_DIR}/thirdparty-install;-Dqhull_TARGETS_INSTALL=qhullcpp"
	PREFIX qhull-prefix
	DEPENDS qhullstatic_r
)

add_library(libqhullstatic_r STATIC IMPORTED)
set_property(TARGET libqhullstatic_r PROPERTY IMPORTED_LOCATION ${PROJECT_BINARY_DIR}/thirdparty-install/lib/${CMAKE_STATIC_LIBRARY_PREFIX}qhullstatic_r${CMAKE_STATIC_LIBRARY_SUFFIX})
add_dependencies(libqhullstatic_r qhullstatic_r)

add_library(libqhullcpp STATIC IMPORTED)
set_property(TARGET libqhullcpp PROPERTY IMPORTED_LOCATION ${PROJECT_BINARY_DIR}/thirdparty-install/lib/${CMAKE_STATIC_LIBRARY_PREFIX}qhullcpp${CMAKE_STATIC_LIBRARY_SUFFIX})
set_property(TARGET libqhullcpp PROPERTY INTERFACE_LINK_LIBRARIES qhullstatic_r)
add_dependencies(libqhullcpp qhullcpp)

set(SOURCE_FILES
    src/ArrowRenderer.cxx
    src/BoundingBoxRenderer.cxx
    src/Camera.cxx
    src/CombinedRenderer.cxx
    src/CoordinateSystemRenderer.cxx
    src/FPSCounter.cxx
    src/Geometry.cxx
    src/IsosurfaceRenderer.cxx
    src/RendererBase.cxx
    src/shaders
    src/shaders/arrows.frag.glsl
    src/shaders/arrows.vert.glsl
    src/shaders/boundingbox.frag.glsl
    src/shaders/boundingbox.vert.glsl
    src/shaders/colormap.bluegreenred.glsl
    src/shaders/colormap.bluered.glsl
    src/shaders/colormap.bluewhitered.glsl
    src/shaders/colormap.hsv.glsl
    src/shaders/coordinatesystem.frag.glsl
    src/shaders/coordinatesystem.vert.glsl
    src/shaders/sphere_background.frag.glsl
    src/shaders/sphere_background.vert.glsl
    src/shaders/sphere_points.frag.glsl
    src/shaders/sphere_points.vert.glsl
    src/shaders/surface.frag.glsl
    src/shaders/surface.vert.glsl
    src/SurfaceRenderer.cxx
    src/Utilities.cxx
    src/VectorfieldIsosurface.cxx
    src/VectorSphereRenderer.cxx
    src/View.cxx
    ${CMAKE_SOURCE_DIR}/thirdparty/glad/src/glad.c
)

set(HEADER_FILES
    include/VectorfieldIsosurface.hxx
    include/VFRendering/ArrowRenderer.hxx
    include/VFRendering/BoundingBoxRenderer.hxx
    include/VFRendering/Camera.hxx
    include/VFRendering/CombinedRenderer.hxx
    include/VFRendering/CoordinateSystemRenderer.hxx
    include/VFRendering/FPSCounter.hxx
    include/VFRendering/Geometry.hxx
    include/VFRendering/IsosurfaceRenderer.hxx
    include/VFRendering/Options.hxx
    include/VFRendering/RendererBase.hxx
    include/VFRendering/SurfaceRenderer.hxx
    include/VFRendering/Utilities.hxx
    include/VFRendering/VectorSphereRenderer.hxx
    include/VFRendering/View.hxx
)

add_library(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} libqhullcpp libqhullstatic_r)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_EXTENSIONS OFF)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/thirdparty/glm/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/thirdparty/glad/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_BINARY_DIR}/thirdparty-install/include/libqhullcpp)

# Build demo

add_executable(${PROJECT_NAME}Demo demo.cxx)
target_link_libraries(${PROJECT_NAME}Demo ${PROJECT_NAME})

find_package(glfw3 3.2 REQUIRED)
target_link_libraries(${PROJECT_NAME}Demo glfw)

set_property(TARGET ${PROJECT_NAME}Demo PROPERTY CXX_STANDARD 11)
set_property(TARGET ${PROJECT_NAME}Demo PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET ${PROJECT_NAME}Demo PROPERTY CXX_EXTENSIONS OFF)

target_include_directories(${PROJECT_NAME}Demo PRIVATE ${PROJECT_SOURCE_DIR}/thirdparty/glad/include)
