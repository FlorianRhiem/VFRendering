#include "VFRendering/RendererBase.hxx"

namespace VFRendering {
RendererBase::RendererBase(const View& view) : m_view(view) {}

const Options& RendererBase::options() const {
    return m_view.options();
}

const std::vector<glm::vec3>& RendererBase::positions() const {
    return m_view.positions();
}

const std::vector<glm::vec3>& RendererBase::directions() const {
    return m_view.directions();
}

const std::vector<std::array<Geometry::index_type, 3>>& RendererBase::surfaceIndices() const {
    return m_view.surfaceIndices();
}

const std::vector<std::array<Geometry::index_type, 4>>& RendererBase::volumeIndices() const {
    return m_view.volumeIndices();
}
}
