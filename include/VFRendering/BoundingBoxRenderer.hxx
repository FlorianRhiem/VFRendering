#ifndef VFRENDERING_BOUNDING_BOX_RENDERER_H
#define VFRENDERING_BOUNDING_BOX_RENDERER_H

#include <VFRendering/RendererBase.hxx>

namespace VFRendering {
class BoundingBoxRenderer : public RendererBase {
public:
    enum Option {
        COLOR = 600
    };


    BoundingBoxRenderer(const View& view);
    virtual ~BoundingBoxRenderer();
    virtual void update(bool keep_geometry) override;
    virtual void draw(float aspect_ratio) override;
    virtual void optionsHaveChanged(const std::vector<int>& changed_options) override;

private:
    void updateVertexData();

    unsigned int m_program = 0;
    unsigned int m_vao = 0;
    unsigned int m_vbo = 0;
};
}

// Options

template<>
struct VFRendering::Option<VFRendering::BoundingBoxRenderer::Option::COLOR>{
    glm::vec3 default_value = {1.0, 1.0, 1.0};
};


#endif
