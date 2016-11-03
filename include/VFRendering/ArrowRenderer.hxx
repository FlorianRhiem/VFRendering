#ifndef VFRENDERING_ARROW_RENDERER_H
#define VFRENDERING_ARROW_RENDERER_H

#include <VFRendering/RendererBase.hxx>

namespace VFRendering {
class ArrowRenderer : public RendererBase {
public:
    enum Option {
        CONE_RADIUS = 200,
        CONE_HEIGHT,
        CYLINDER_RADIUS,
        CYLINDER_HEIGHT,
        LEVEL_OF_DETAIL
    };


    ArrowRenderer(const View& view);
    virtual ~ArrowRenderer();
    virtual void update(bool keep_geometry) override;
    virtual void draw(float aspect_ratio) const override;
    virtual void optionsHaveChanged(const std::vector<int>& changed_options) override;

private:
    void updateShaderProgram();
    void updateVertexData();

    unsigned int m_program = 0;
    unsigned int m_vao = 0;
    unsigned int m_vbo = 0;
    unsigned int m_ibo = 0;
    unsigned int m_instance_position_vbo = 0;
    unsigned int m_instance_direction_vbo = 0;
    unsigned int m_num_indices = 0;
    unsigned int m_num_instances = 0;
};
}

// Options

template<>
struct VFRendering::Option<VFRendering::ArrowRenderer::Option::CONE_RADIUS> {
    float default_value = 0.25f;
};


template<>
struct VFRendering::Option<VFRendering::ArrowRenderer::Option::CONE_HEIGHT> {
    float default_value = 0.6f;
};


template<>
struct VFRendering::Option<VFRendering::ArrowRenderer::Option::CYLINDER_RADIUS> {
    float default_value = 0.125f;
};


template<>
struct VFRendering::Option<VFRendering::ArrowRenderer::Option::CYLINDER_HEIGHT> {
    float default_value = 0.7f;
};


template<>
struct VFRendering::Option<VFRendering::ArrowRenderer::Option::LEVEL_OF_DETAIL> {
    unsigned int default_value = 20;
};


#endif
