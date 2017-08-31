#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "pybind11_glm.hpp"

#include <VFRendering/View.hxx>
#include <VFRendering/Geometry.hxx>

namespace py = pybind11;

typedef unsigned int index_type;

PYBIND11_MODULE(vfrendering, m)
{
    py::class_<VFRendering::Geometry>(m, "Geometry")
        .def(py::init<>())
        .def(py::init<const std::vector<glm::vec3>&, const std::vector<std::array<index_type, 3>>&, const std::vector<std::array<index_type, 4>>&, const bool&>())
        .def("cartesianGeometry", &VFRendering::Geometry::cartesianGeometry)
        .def("rectilinearGeometry", &VFRendering::Geometry::rectilinearGeometry);

    py::class_<VFRendering::View>(m, "View")
        .def(py::init<>())
        .def("update", &VFRendering::View::update);
}
