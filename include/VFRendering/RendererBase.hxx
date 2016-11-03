#ifndef VFRENDERING_RENDERER_BASE_H
#define VFRENDERING_RENDERER_BASE_H

#include <vector>

#include <glm/glm.hpp>

#include <VFRendering/Options.hxx>
#include <VFRendering/View.hxx>

namespace VFRendering {

class GLSpins;

class RendererBase {
public:

    RendererBase(const View& view);

    virtual ~RendererBase() {};
    virtual void update(bool keep_geometry) = 0;
    virtual void draw(float aspect_ratio) const = 0;
    virtual void optionsHaveChanged(const std::vector<int>& changed_options) = 0;

protected:
    const Options& options() const;
    const std::vector<glm::vec3>& positions() const;
    const std::vector<glm::vec3>& directions() const;
    const std::vector<std::array<Geometry::index_type, 3>>& surfaceIndices() const;
    const std::vector<std::array<Geometry::index_type, 4>>& volumeIndices() const;

private:
    const View& m_view;
};

}

#endif
