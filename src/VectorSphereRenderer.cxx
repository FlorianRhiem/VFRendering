#include "VFRendering/VectorSphereRenderer.hxx"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "VFRendering/Utilities.hxx"

#include "shaders/sphere_points.vert.glsl.generated.hxx"
#include "shaders/sphere_points.frag.glsl.generated.hxx"
#include "shaders/sphere_background.vert.glsl.generated.hxx"
#include "shaders/sphere_background.frag.glsl.generated.hxx"

namespace VFRendering {
VectorSphereRenderer::VectorSphereRenderer(const View& view) : RendererBase(view) {
    glGenVertexArrays(1, &m_sphere_points_vao);
    glBindVertexArray(m_sphere_points_vao);
    glGenBuffers(1, &m_sphere_points_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_sphere_points_vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, nullptr);
    glEnableVertexAttribArray(0);

    glGenVertexArrays(1, &m_sphere_background_vao);
    glBindVertexArray(m_sphere_background_vao);
    glGenBuffers(1, &m_sphere_background_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_sphere_background_vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, nullptr);
    std::vector<GLfloat> vertices = {
        -1, -1, 0,
        1, -1, 0,
        -1, 1, 0,
        -1, 1, 0,
        1, -1, 0,
        1, 1, 0
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);

    updateShaderProgram();
    update(false);
}

VectorSphereRenderer::~VectorSphereRenderer() {
    glDeleteVertexArrays(1, &m_sphere_points_vao);
    glDeleteVertexArrays(1, &m_sphere_background_vao);
    glDeleteBuffers(1, &m_sphere_background_vbo);
    glDeleteBuffers(1, &m_sphere_points_vbo);
    glDeleteProgram(m_sphere_points_program);
    glDeleteProgram(m_sphere_background_program);
}

void VectorSphereRenderer::optionsHaveChanged(const std::vector<int>& changed_options) {
    bool update_shader = false;
    for (auto option_index : changed_options) {
        switch (option_index) {
        case View::Option::COLORMAP_IMPLEMENTATION:
            update_shader = true;
            break;
        }
    }
    if (update_shader) {
        updateShaderProgram();
    }
}

void VectorSphereRenderer::update(bool keep_geometry) {
    (void)keep_geometry;
    glBindVertexArray(m_sphere_points_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_sphere_points_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * directions().size(), directions().data(), GL_STREAM_DRAW);
    m_num_instances = directions().size();
}

void VectorSphereRenderer::draw(float aspect_ratio) const {
    float inner_sphere_radius = options().get<VectorSphereRenderer::Option::INNER_SPHERE_RADIUS>();
    if (inner_sphere_radius > 0.0f) {
        if (inner_sphere_radius > 0.99f) {
            inner_sphere_radius = 0.99f;
        }
        glUseProgram(m_sphere_background_program);

        glBindVertexArray(m_sphere_background_vao);
        glUniform1f(glGetUniformLocation(m_sphere_background_program, "uAspectRatio"), aspect_ratio);
        glUniform1f(glGetUniformLocation(m_sphere_background_program, "uInnerSphereRadius"), inner_sphere_radius);
        glDisable(GL_CULL_FACE);
        glDepthMask(false);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glDepthMask(true);
        glEnable(GL_CULL_FACE);
    }

    if (m_num_instances <= 0) {
        return;
    }

    glUseProgram(m_sphere_points_program);
    glBindVertexArray(m_sphere_points_vao);

    glm::vec2 z_range = options().get<View::Option::Z_RANGE>();
    if (z_range.x <= -1) {
        z_range.x = -2;
    }
    if (z_range.y >= 1) {
        z_range.y = 2;
    }
    glm::vec3 camera_position = options().get<View::Option::CAMERA_POSITION>();
    glm::vec3 center_position = options().get<View::Option::CENTER_POSITION>();
    glm::vec3 upVector = options().get<View::Option::UP_VECTOR>();
    glm::vec2 point_size_range = options().get<VectorSphereRenderer::Option::POINT_SIZE_RANGE>();

    glm::mat4 projection_matrix;
    if (aspect_ratio > 1) {
        projection_matrix = glm::ortho(-aspect_ratio, aspect_ratio, -1.0f, 1.0f, 2.0f, 0.0f);
    } else {
        projection_matrix = glm::ortho(-1.0f, 1.0f, -1 / aspect_ratio, 1 / aspect_ratio, 2.0f, 0.0f);
    }
    glm::mat4 modelview_matrix = glm::lookAt(glm::normalize(camera_position - center_position), {0, 0, 0}, upVector);
    glm::vec4 light_position = modelview_matrix * glm::vec4(camera_position, 1.0f);

    glUniformMatrix4fv(glGetUniformLocation(m_sphere_points_program, "uProjectionMatrix"), 1, false, glm::value_ptr(projection_matrix));
    glUniformMatrix4fv(glGetUniformLocation(m_sphere_points_program, "uModelviewMatrix"), 1, false, glm::value_ptr(modelview_matrix));
    glUniform3f(glGetUniformLocation(m_sphere_points_program, "uLightPosition"), light_position[0], light_position[1], light_position[2]);
    glUniform2f(glGetUniformLocation(m_sphere_points_program, "uZRange"), z_range[0], z_range[1]);
    glUniform2f(glGetUniformLocation(m_sphere_points_program, "uPointSizeRange"), point_size_range[0], point_size_range[1]);

    glUniform1f(glGetUniformLocation(m_sphere_points_program, "uAspectRatio"), aspect_ratio);
    glUniform1f(glGetUniformLocation(m_sphere_points_program, "uInnerSphereRadius"), inner_sphere_radius);
    bool use_sphere_fake_perspective = options().get<VectorSphereRenderer::Option::USE_SPHERE_FAKE_PERSPECTIVE>();
    if (use_sphere_fake_perspective) {
        glUniform1f(glGetUniformLocation(m_sphere_points_program, "uUseFakePerspective"), 1.0f);
    } else {
        glUniform1f(glGetUniformLocation(m_sphere_points_program, "uUseFakePerspective"), 0.0f);
    }

    glEnable(GL_PROGRAM_POINT_SIZE);
    glDisable(GL_CULL_FACE);
    glDrawArrays(GL_POINTS, 0, m_num_instances);
    glEnable(GL_CULL_FACE);
    glDisable(GL_PROGRAM_POINT_SIZE);
}

void VectorSphereRenderer::updateShaderProgram() {
    {
        if (m_sphere_points_program) {
            glDeleteProgram(m_sphere_points_program);
        }
        std::string vertex_shader_source = SPHERE_POINTS_VERT_GLSL;
        std::string fragment_shader_source = SPHERE_POINTS_FRAG_GLSL;
        fragment_shader_source += options().get<View::Option::COLORMAP_IMPLEMENTATION>();
        m_sphere_points_program = Utilities::createProgram(vertex_shader_source, fragment_shader_source, {"ivDirection"});
    }
    {
        if (m_sphere_background_program) {
            glDeleteProgram(m_sphere_background_program);
        }
        std::string vertex_shader_source = SPHERE_BACKGROUND_VERT_GLSL;
        std::string fragment_shader_source = SPHERE_BACKGROUND_FRAG_GLSL;
        m_sphere_background_program = Utilities::createProgram(vertex_shader_source, fragment_shader_source, {"ivPosition"});
    }
}
}
