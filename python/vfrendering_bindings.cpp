#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "pybind11_glm.hpp"

#include <VFRendering/View.hxx>
#include <VFRendering/Geometry.hxx>
#include <VFRendering/RendererBase.hxx>
#include <VFRendering/ArrowRenderer.hxx>
#include <VFRendering/BoundingBoxRenderer.hxx>
#include <VFRendering/CoordinateSystemRenderer.hxx>
#include <VFRendering/Options.hxx>

#include <memory>

namespace py = pybind11;
using namespace VFRendering;

typedef unsigned int index_type;

PYBIND11_MODULE(pyVFRendering, m)
{
    // Module docstring
    m.doc() = "VFRendering is a C++ library for rendering vectorfields using OpenGL,"
              "wrapped for Python using pybind11.";


    // Module functions
    m.def("getColormapImplementation", &Utilities::getColormapImplementation, "Get a CM implementation from the CM enum");


    // Geometry class
    py::class_<Geometry>(m, "Geometry",
        "This class defines the positions of the data points and the triangulation indices between the data points."
        " In 2D, a triangulation can be used to display data on a smooth surface of triangles."
        " In 3D, tetrahedra can be used e.g. to calculate isosurfaces.")
        .def(py::init<>())
        .def(py::init<const std::vector<glm::vec3>&, const std::vector<std::array<index_type, 3>>&, const std::vector<std::array<index_type, 4>>&, const bool&>())
        .def("cartesianGeometry", &Geometry::cartesianGeometry,
            "Create a geometry corresponding to a cartesian grid")
        .def("rectilinearGeometry", &Geometry::rectilinearGeometry,
            "Create a geometry corresponding to a rectilinear grid")
        .def("min", &Geometry::min,
            "Retrieve the x-, y- and z- minimum values of all positions")
        .def("max", &Geometry::max,
            "Retrieve the x-, y- and z- maximum values of all positions");


    // View
    py::class_<View>(m, "View",
        "This class holds the Renderers and defines what is drawn into the current OpenGL context.")
        // Constructor
        .def(py::init<>())
        // Actions
        .def("draw", &View::draw,
            "Draw into current OpenGL context")
        .def("update", &View::update,
            "Update the View and redraw OpenGL")
        .def("updateOptions", &View::updateOptions,
            "Update the set of options given to the View")
        .def("mouseMove", &View::mouseMove,
            "Influence the camera according to a movement of the mouse")
        .def("mouseScroll", &View::mouseScroll,
            "Influence the camera according to a scrolling of the mouse")
        // Setters
        .def("setFramebufferSize", &View::setFramebufferSize,
            "Set the size of the Framebuffer into which the View should render, i.e. the number of pixels")
        // Getters
        .def("renderers", &View::renderers,
            "Retrieve the renderers currently in use by the View")
        .def("getFramerate", &View::getFramerate,
            "Retrieve the last known framerate of OpenGL draws");


    // View Options
    py::class_<Options>(m, "Options",
        "This class holds the View's various options.")
        // Constructor
        .def(py::init<>())
        // Camera set
        .def("set_verticalFOV",             &Options::set<View::Option::VERTICAL_FIELD_OF_VIEW>,
            "Set the vertical field of view (FOV) of the camera")
        .def("set_systemCenter",            &Options::set<View::Option::SYSTEM_CENTER>,
            "Set the system center, i.e. the center point of the system's bounds")
        .def("set_cameraPosition",          &Options::set<View::Option::CAMERA_POSITION>,
            "Set the position of the camera")
        .def("set_centerPosition",          &Options::set<View::Option::CENTER_POSITION>,
            "Set the center position, i.e. the focal point of the View")
        .def("set_upVector",                &Options::set<View::Option::UP_VECTOR>,
            "Set the up-vector of the camera")
        // Camera get
        .def("get_verticalFOV",             &Options::get<View::Option::VERTICAL_FIELD_OF_VIEW>,
            "Retrieve the vertical field of view (FOV) of the camera")
        .def("get_systemCenter",            &Options::get<View::Option::SYSTEM_CENTER>,
            "Retrieve the system center, i.e. the center point of the system's bounds")
        .def("get_cameraPosition",          &Options::get<View::Option::CAMERA_POSITION>,
            "Retrieve the position of the camera")
        .def("get_centerPosition",          &Options::get<View::Option::CENTER_POSITION>,
            "Retrieve the center position, i.e. the focal point of the View")
        .def("get_upVector",                &Options::get<View::Option::UP_VECTOR>,
            "Retrieve the up-vector of the camera")
        // Colors set
        .def("set_backgroundColor",         &Options::set<View::Option::BACKGROUND_COLOR>,
            "Set the background color of the View")
        .def("set_boundingboxColor",        &Options::set<BoundingBoxRenderer::Option::COLOR>,
            "Set the color of the bounding box")
        .def("set_colormapImplementation",  &Options::set<View::Option::COLORMAP_IMPLEMENTATION>,
            "Set the implementation of the colormap")
        // Colors get
        .def("get_backgroundColor",         &Options::get<View::Option::BACKGROUND_COLOR>,
            "Retrieve the background color of the View")
        .def("get_boundingBoxColor",        &Options::get<BoundingBoxRenderer::Option::COLOR>,
            "Retrieve the color of the bounding box")
        // Filters set
        .def("set_isVisibleImplementation", &Options::set<View::Option::IS_VISIBLE_IMPLEMENTATION>,
            "Set a filter for the visibility of objects");


    // Colormap enum
    py::enum_<Utilities::Colormap>(m, "Colormap")
        .value("hsv",            Utilities::Colormap::HSV)
        .value("blue_white_red", Utilities::Colormap::BLUEWHITERED)
        .value("blue_green_red", Utilities::Colormap::BLUEGREENRED)
        .value("black", Utilities::Colormap::BLACK)
        .value("white", Utilities::Colormap::WHITE)
        .export_values();


    // Camera movement modes enum to be used in View::mouseMove
    py::enum_<CameraMovementModes>(m, "CameraMovementModes")
        .value("rotate_bounded", CameraMovementModes::ROTATE_BOUNDED)
        .value("rotate_free",    CameraMovementModes::ROTATE_FREE)
        .value("translate",      CameraMovementModes::TRANSLATE)
        .export_values();


    // Renderer base class
    py::class_<RendererBase, std::shared_ptr<RendererBase>>(m, "RendererBase", "Renderer base class");


    // ArrowRenderer
    py::class_<ArrowRenderer, RendererBase, std::shared_ptr<ArrowRenderer>>(m, "ArrowRenderer",
        "This class is used to draw arrows directly corresponding to a vectorfield.")
        .def(py::init<View&>());


    // BoundingBoxRenderer
    py::class_<BoundingBoxRenderer, RendererBase, std::shared_ptr<BoundingBoxRenderer>>(m, "BoundingBoxRenderer",
        "This Renderer draws a bounding box of the specified dimensions. It may include indicators for"
        " periodical boundary conditions along the three cardinal directions each.")
        .def("forCuboid", &BoundingBoxRenderer::forCuboid);


    // CoordinateSystemRenderer
    py::class_<CoordinateSystemRenderer, RendererBase, std::shared_ptr<CoordinateSystemRenderer>>(m, "CoordinateSystemRenderer",
        "This Renderer draws a coordinate cross consisting of three arrows, colored according to the applied colormap.")
        .def(py::init<View&>())
        .def("set_axisLength", &CoordinateSystemRenderer::setOption<CoordinateSystemRenderer::Option::AXIS_LENGTH>)
        .def("set_normalize",  &CoordinateSystemRenderer::setOption<CoordinateSystemRenderer::Option::NORMALIZE>);
}
