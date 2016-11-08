#ifndef VFRENDERING_ISOSURFACE_RENDERER_H
#define VFRENDERING_ISOSURFACE_RENDERER_H

#include <functional>

#include <VFRendering/RendererBase.hxx>

namespace VFRendering {
class IsosurfaceRenderer : public RendererBase {
public:
    enum Option {
        ISOVALUE = 700,
        VALUEFUNCTION
    };

    IsosurfaceRenderer(const View& view);
    virtual ~IsosurfaceRenderer();
    virtual void update(bool keep_geometry) override;
    virtual void draw(float aspect_ratio) const override;
    virtual void optionsHaveChanged(const std::vector<int>& changed_options) override;

private:
    void updateShaderProgram();
    void updateIsosurfaceIndices();

    unsigned int m_program = 0;
    unsigned int m_vao = 0;
    unsigned int m_ibo = 0;
    unsigned int m_position_vbo = 0;
    unsigned int m_direction_vbo = 0;
    unsigned int m_num_indices = 0;
};
}

template<>
struct VFRendering::Option<VFRendering::IsosurfaceRenderer::Option::ISOVALUE> {
    double default_value = 0;
};

template<>
struct VFRendering::Option<VFRendering::IsosurfaceRenderer::Option::VALUEFUNCTION> {
    std::function<float(const glm::vec3&, const glm::vec3&)> default_value = [] (const glm::vec3& position, const glm::vec3& direction) {
        (void)direction;
        return position.x+position.y;
    };
};


#endif
