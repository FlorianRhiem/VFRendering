#ifndef VFRENDERING_SURFACE_RENDERER_H
#define VFRENDERING_SURFACE_RENDERER_H

#include <array>

#include <VFRendering/RendererBase.hxx>

namespace VFRendering {

class SurfaceRenderer : public RendererBase {
public:
    SurfaceRenderer(const View& view);
    virtual ~SurfaceRenderer();
    virtual void update(bool keep_geometry) override;
    virtual void draw(float aspect_ratio) override;
    virtual void optionsHaveChanged(const std::vector<int>& changed_options) override;

private:
    void updateShaderProgram();
    void updateSurfaceIndices();

    unsigned int m_program = 0;
    unsigned int m_vao = 0;
    unsigned int m_ibo = 0;
    unsigned int m_position_vbo = 0;
    unsigned int m_direction_vbo = 0;
    unsigned int m_num_indices = 0;
};

}

#endif
