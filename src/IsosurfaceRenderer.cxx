#include "VFRendering/IsosurfaceRenderer.hxx"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "VFRendering/View.hxx"
#include "VFRendering/Utilities.hxx"

#include "VectorfieldIsosurface.hxx"
#include "shaders/surface.vert.glsl.generated.hxx"
#include "shaders/surface.frag.glsl.generated.hxx"

namespace VFRendering {
IsosurfaceRenderer::IsosurfaceRenderer(const View& view) : RendererBase(view), m_value_function_changed(true), m_isovalue_changed(true) {
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
    m_num_indices = 0;

    glGenBuffers(1, &m_position_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_position_vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, nullptr);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &m_direction_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_direction_vbo);
    glVertexAttribPointer(1, 3, GL_FLOAT, false, 0, nullptr);
    glEnableVertexAttribArray(1);

    updateShaderProgram();
    updateIsosurfaceIndices();
}

IsosurfaceRenderer::~IsosurfaceRenderer() {
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_ibo);
    glDeleteBuffers(1, &m_position_vbo);
    glDeleteBuffers(1, &m_direction_vbo);
    glDeleteProgram(m_program);
}

void IsosurfaceRenderer::optionsHaveChanged(const std::vector<int>& changed_options) {
    bool update_shader = false;
    for (auto option_index : changed_options) {
        switch (option_index) {
            case Option::ISOVALUE:
                m_isovalue_changed = true;
                break;
            case Option::VALUE_FUNCTION:
                m_value_function_changed = true;
                break;
            case View::Option::COLORMAP_IMPLEMENTATION:
            case View::Option::IS_VISIBLE_IMPLEMENTATION:
            update_shader = true;
            break;
        }
    }
    if (update_shader) {
        updateShaderProgram();
    }
}

void IsosurfaceRenderer::update(bool keep_geometry) {
    (void)keep_geometry;
    updateIsosurfaceIndices();
}

void IsosurfaceRenderer::draw(float aspect_ratio) {
    if (m_value_function_changed || m_isovalue_changed) {
        updateIsosurfaceIndices();
    }
    if (m_num_indices <= 0) {
        return;
    }
    glBindVertexArray(m_vao);
    glUseProgram(m_program);

    // Disable z-Filtering, that's what the isosurface is for, after all.
    glm::vec2 z_range = {-2, 2};

    auto matrices = Utilities::getMatrices(options(), aspect_ratio);
    auto model_view_matrix = matrices.first;
    auto projection_matrix = matrices.second;

    glm::vec3 camera_position = options().get<View::Option::CAMERA_POSITION>();
    glm::vec4 light_position = model_view_matrix * glm::vec4(camera_position, 1.0f);

    glUniformMatrix4fv(glGetUniformLocation(m_program, "uProjectionMatrix"), 1, false, glm::value_ptr(projection_matrix));
    glUniformMatrix4fv(glGetUniformLocation(m_program, "uModelviewMatrix"), 1, false, glm::value_ptr(model_view_matrix));
    glUniform3f(glGetUniformLocation(m_program, "uLightPosition"), light_position[0], light_position[1], light_position[2]);
    glUniform2f(glGetUniformLocation(m_program, "uZRange"), z_range[0], z_range[1]);

    glDisable(GL_CULL_FACE);
    glDrawElements(GL_TRIANGLES, m_num_indices, GL_UNSIGNED_INT, nullptr);
    glEnable(GL_CULL_FACE);
}

void IsosurfaceRenderer::updateShaderProgram() {
    if (m_program) {
        glDeleteProgram(m_program);
    }
    std::string vertex_shader_source = SURFACE_VERT_GLSL;
    vertex_shader_source += options().get<View::Option::COLORMAP_IMPLEMENTATION>();
    std::string fragment_shader_source = SURFACE_FRAG_GLSL;
    fragment_shader_source += options().get<View::Option::COLORMAP_IMPLEMENTATION>();
    fragment_shader_source += options().get<View::Option::IS_VISIBLE_IMPLEMENTATION>();
    m_program = Utilities::createProgram(vertex_shader_source, fragment_shader_source, {"ivPosition", "ivDirection"});
}

void IsosurfaceRenderer::updateIsosurfaceIndices() {
    m_value_function_changed = false;
    m_isovalue_changed = false;

    auto value_function = options().get<Option::VALUE_FUNCTION>();
    auto isovalue = options().get<Option::ISOVALUE>();

    const auto& volume_indices = volumeIndices();

    if (volume_indices.size() == 0) {
        m_num_indices = 0;
        return;
    } else if (positions().size() < 4) {
        m_num_indices = 0;
        return;
    } else if (!value_function) {
        m_num_indices = 0;
        return;
    }

    std::vector<float> values;
    for (Geometry::index_type i = 0; i < positions().size(); i++) {
        const glm::vec3& position = positions()[i];
        const glm::vec3& direction = directions()[i];
        values.push_back(value_function(position, direction));
    }

    VectorfieldIsosurface isosurface(VectorfieldIsosurface::calculate(positions(), directions(), values, isovalue, volume_indices));

    const std::vector<GLuint> surface_indices(isosurface.triangle_indices.begin(), isosurface.triangle_indices.end());

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_position_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * isosurface.positions.size(), isosurface.positions.data(), GL_STREAM_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, m_direction_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * isosurface.directions.size(), isosurface.directions.data(), GL_STREAM_DRAW);

    // Enforce valid range
    if (surface_indices.size() < 3) {
        m_num_indices = 0;
        return;
    }
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * surface_indices.size(), surface_indices.data(), GL_STREAM_DRAW);
    m_num_indices = surface_indices.size();
}
}
