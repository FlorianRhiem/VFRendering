#ifndef VFRENDERING_ISOSURFACE_RENDERER_H
#define VFRENDERING_ISOSURFACE_RENDERER_H

#include <functional>

#include <VFRendering/RendererBase.hxx>

namespace VFRendering {
class IsosurfaceRenderer : public RendererBase {
public:

    typedef float isovalue_type;
    typedef std::function<isovalue_type(const glm::vec3&, const glm::vec3&)> value_function_type;

    IsosurfaceRenderer(const View& view, const value_function_type& value_function, const isovalue_type& isovalue);
    virtual ~IsosurfaceRenderer();
    virtual void update(bool keep_geometry) override;
    virtual void draw(float aspect_ratio) override;
    virtual void optionsHaveChanged(const std::vector<int>& changed_options) override;
    virtual void valueFunction(const value_function_type& value_function);
    virtual void isovalue(const isovalue_type& isovalue);
    virtual const value_function_type& valueFunction();
    virtual const isovalue_type& isovalue();

private:
    void updateShaderProgram();
    void updateIsosurfaceIndices();

    unsigned int m_program = 0;
    unsigned int m_vao = 0;
    unsigned int m_ibo = 0;
    unsigned int m_position_vbo = 0;
    unsigned int m_direction_vbo = 0;
    unsigned int m_num_indices = 0;

    value_function_type m_value_function;
    bool m_value_function_changed;
    isovalue_type m_isovalue;
    bool m_isovalue_changed;
};
}

#endif
