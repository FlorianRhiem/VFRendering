#include "VFRendering/View.hxx"

#include <iostream>

#include <glm/gtx/transform.hpp>
#include <glad/glad.h>

#include "VFRendering/RendererBase.hxx"
#include "VFRendering/ArrowRenderer.hxx"
#include "VFRendering/SurfaceRenderer.hxx"
#include "VFRendering/IsosurfaceRenderer.hxx"
#include "VFRendering/VectorSphereRenderer.hxx"
#include "VFRendering/BoundingBoxRenderer.hxx"
#include "VFRendering/CombinedRenderer.hxx"
#include "VFRendering/CoordinateSystemRenderer.hxx"

namespace VFRendering {
View::View() {
    if (!gladLoadGL()) {
        std::cerr << "Failed to initialize glad" << std::endl;
        return;
    }
    // Reset any errors potentially caused by the extension loader
    glGetError();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    auto system_center = options().get<Option::SYSTEM_CENTER>();
    auto bounds_min = options().get<Option::BOUNDING_BOX_MIN>();
    auto bounds_max = options().get<Option::BOUNDING_BOX_MAX>();
    setCamera(system_center + glm::vec3(0, 0, glm::length(bounds_max - bounds_min)));
    renderers(VisualizationMode::ARROWS, true, true, WidgetLocation::BOTTOM_LEFT, true, WidgetLocation::BOTTOM_RIGHT);
}

View::~View() {}

void View::update(const Geometry& geometry, const std::vector<glm::vec3>& vectors) {
    m_geometry = geometry;
    m_vectors = vectors;
    for (auto it : m_renderers) {
        auto renderer = it.first;
        renderer->update(false);
    }
}

void View::updateVectors(const std::vector<glm::vec3>& vectors) {
    m_vectors = vectors;
    for (auto it : m_renderers) {
        auto renderer = it.first;
        renderer->update(true);
    }
}

void View::draw() {
    auto background_color = m_options.get<View::Option::BACKGROUND_COLOR>();
    glClearColor(background_color.x, background_color.y, background_color.z, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Options options;
    options.set<View::Option::CAMERA_POSITION>(m_camera.cameraPosition());
    options.set<View::Option::CENTER_POSITION>(m_camera.centerPosition());
    options.set<View::Option::UP_VECTOR>(m_camera.upVector());
    auto bounds_max = m_options.get<View::Option::BOUNDING_BOX_MAX>();
    auto center = m_options.get<View::Option::SYSTEM_CENTER>();
    options.set<CoordinateSystemRenderer::Option::AXIS_LENGTH>({
            fmax(fabs(bounds_max[0] - center[0]), 1.0), fmax(fabs(bounds_max[1] - center[1]), 1.0), fmax(fabs(bounds_max[2] - center[2]), 1.0)
        });
    updateOptions(options);

    glClear(GL_COLOR_BUFFER_BIT);
    for (auto it : m_renderers) {
        auto renderer = it.first;
        auto viewport = it.second;
        float width = m_framebuffer_size.x;
        float height = m_framebuffer_size.y;
        glViewport((GLint)(viewport[0] * width), (GLint)(viewport[1] * height), (GLsizei)(viewport[2] * width), (GLsizei)(viewport[3] * height));
        glClear(GL_DEPTH_BUFFER_BIT);
        renderer->draw(viewport[2] / viewport[3] * m_camera.aspectRatio());
    }
    m_fps_counter.tick();
}

float View::getFramerate() const {
    return m_fps_counter.getFramerate();
}

void View::mouseMove(const glm::vec2& position_before, const glm::vec2& position_after, CameraMovementModes mode) {
    if (position_before == position_after) {
        return;
    }
    auto delta = position_after - position_before;
    auto length = glm::length(delta);
    auto forward = glm::normalize(m_camera.centerPosition() - m_camera.cameraPosition());
    auto camera_distance = glm::length(m_camera.centerPosition() - m_camera.cameraPosition());
    auto up = glm::normalize(m_camera.upVector());
    auto right = glm::normalize(glm::cross(forward, up));
    up = glm::normalize(glm::cross(right, forward));
    delta = glm::normalize(delta);
    switch (mode) {
    case CameraMovementModes::ROTATE: {
        auto axis = glm::normalize(delta.x * up + delta.y * right);
        float angle = -length * 0.1f / 180 * 3.14f;
        auto rotation_matrix = glm::rotate(angle, axis);
        up = glm::mat3(rotation_matrix) * up;
        forward = glm::mat3(rotation_matrix) * forward;
        m_camera.lookAt(m_camera.centerPosition() - forward * camera_distance, m_camera.centerPosition(), up);
    }
    break;
    case CameraMovementModes::TRANSLATE: {
        float factor = 0.001f * camera_distance * length;
        auto translation = factor * (delta.y * up - delta.x * right);
        m_camera.lookAt(m_camera.cameraPosition() + translation, m_camera.centerPosition() + translation, up);
        m_is_centered = false;
    }
    break;
    default:
        break;
    }
}

void View::mouseScroll(const float& wheel_delta) {
    auto forward = m_camera.centerPosition() - m_camera.cameraPosition();
    float camera_distance = glm::length(forward);
    float new_camera_distance = (float)(1 + 0.02 * wheel_delta) * camera_distance;
    float min_camera_distance = 1;
    if (new_camera_distance < min_camera_distance) {
        new_camera_distance = min_camera_distance;
    }

    auto camera_position = m_camera.centerPosition() - new_camera_distance / camera_distance * forward;
    m_camera.lookAt(camera_position, m_camera.centerPosition(), m_camera.upVector());
}

void View::setFramebufferSize(float width, float height) {
    m_framebuffer_size = glm::vec2(width, height);
    m_camera.setAspectRatio(width / height);
}

void View::setCamera(glm::vec3 camera_position) {
    glm::vec3 center_position = options().get<View::Option::SYSTEM_CENTER>();
    glm::vec3 up_vector(0, 1, 0);
    m_camera.lookAt(camera_position, center_position, up_vector);
    m_is_centered = true;
}

void View::setCamera(glm::vec3 camera_position, glm::vec3 center_position, glm::vec3 up_vector) {
    m_camera.lookAt(camera_position, center_position, up_vector);
    m_is_centered = false;
}

static void recenterCamera(Utilities::Camera& camera, glm::vec3 center_position) {
    glm::vec3 camera_position = camera.cameraPosition() + center_position - camera.centerPosition();
    glm::vec3 up_vector = camera.upVector();
    camera.lookAt(camera_position, center_position, up_vector);
}

static std::array<float, 4> locationToViewport(WidgetLocation location) {
    switch (location) {
    case WidgetLocation::BOTTOM_LEFT:
        return {
                   {
                       0.0f, 0.0f, 0.2f, 0.2f
                   }
        };
    case WidgetLocation::BOTTOM_RIGHT:
        return {
                   {
                       0.8f, 0.0f, 0.2f, 0.2f
                   }
        };
    case WidgetLocation::TOP_LEFT:
        return {
                   {
                       0.0f, 0.8f, 0.2f, 0.2f
                   }
        };
    case WidgetLocation::TOP_RIGHT:
        return {
                   {
                       0.8f, 0.8f, 0.2f, 0.2f
                   }
        };
    }
    return {
               {
                   0, 0, 1, 1
               }
    };
}

/** Convenience function that creates the std::vector of renderers and viewports the most common use cases */
void View::renderers(const VisualizationMode& mode, bool show_bounding_box, bool show_miniview, WidgetLocation miniview_location, bool show_coordinate_system, WidgetLocation coordinate_system_location) {
    std::vector<std::pair<std::shared_ptr<RendererBase>, std::array<float, 4>>> renderers;

    std::shared_ptr<RendererBase> main_renderer;
    switch (mode) {
    case VisualizationMode::ARROWS:
        main_renderer = std::make_shared<ArrowRenderer>(*this);
        break;
    case VisualizationMode::SURFACE:
        main_renderer = std::make_shared<SurfaceRenderer>(*this);
        break;
    case VisualizationMode::ISOSURFACE:
            // TODO: make these values configurable
            main_renderer = std::make_shared<IsosurfaceRenderer>(*this, [] (const glm::vec3& position, const glm::vec3& direction) -> IsosurfaceRenderer::isovalue_type {
                                                                     (void)position;
                                                                     return direction.z;
                                                                 }, 0.0);
        break;
    case VisualizationMode::SPHERE:
        main_renderer = std::make_shared<VectorSphereRenderer>(*this);
        break;
    }
    if (show_bounding_box && mode != VisualizationMode::SPHERE) {
        std::vector<std::shared_ptr<RendererBase>> r = {
            main_renderer, std::make_shared<BoundingBoxRenderer>(*this)
        };
        main_renderer = std::make_shared<CombinedRenderer>(*this, r);
    }
    renderers.push_back({main_renderer, {{0, 0, 1, 1}}});

    if (show_coordinate_system) {
        std::shared_ptr<RendererBase> renderer = std::make_shared<CoordinateSystemRenderer>(*this);
        renderers.push_back({renderer, locationToViewport(coordinate_system_location)});
    }
    if (show_miniview) {
        std::shared_ptr<RendererBase> renderer;
        if (mode == VisualizationMode::SPHERE) {
            renderer = std::make_shared<SurfaceRenderer>(*this);
            if (show_bounding_box) {
                std::vector<std::shared_ptr<RendererBase>> r = {
                    renderer, std::make_shared<BoundingBoxRenderer>(*this)
                };
                renderer = std::make_shared<CombinedRenderer>(*this, r);
            }
        } else {
            renderer = std::make_shared<VectorSphereRenderer>(*this);
        }
        renderers.push_back({renderer, locationToViewport(miniview_location)});
    }

    this->renderers(renderers);
}

void View::renderers(const std::vector<std::pair<std::shared_ptr<RendererBase>, std::array<float, 4>>>& renderers) {
    m_renderers = renderers;
    const auto option_keys = options().keys();
    for (auto it : m_renderers) {
        auto renderer = it.first;
        renderer->optionsHaveChanged(option_keys);
    }
}

void View::updateOptions(const Options& options) {
    auto changed_options = m_options.update(options);
    if (changed_options.size() == 0) {
        return;
    }
    optionsHaveChanged(changed_options);
}

void View::options(const Options& options) {
    m_options = options;
    updateOptions(Options());
}

const Options& View::options() const {
    return m_options;
}

void View::optionsHaveChanged(const std::vector<int>& changed_options) {
    bool recenter_camera = false;
    for (int option_index : changed_options) {
        switch (option_index) {
        case Option::SYSTEM_CENTER:
            recenter_camera = true;
            break;
        }
    }
    if (m_is_centered && recenter_camera) {
        recenterCamera(m_camera, options().get<Option::SYSTEM_CENTER>());
    }
    for (auto it : m_renderers) {
        auto renderer = it.first;
        renderer->optionsHaveChanged(changed_options);
    }
}

const std::vector<glm::vec3>& View::positions() const {
    return m_geometry.positions();
}

const std::vector<glm::vec3>& View::directions() const {
    return m_vectors;
}

const std::vector<std::array<Geometry::index_type, 3>>& View::surfaceIndices() const {
    return m_geometry.surfaceIndices();
}

const std::vector<std::array<Geometry::index_type, 4>>& View::volumeIndices() const {
    return m_geometry.volumeIndices();
}
}
