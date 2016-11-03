#ifndef VFRENDERING_VIEW_H
#define VFRENDERING_VIEW_H

#include <array>
#include <memory>

#include <glm/glm.hpp>

#include <VFRendering/Camera.hxx>
#include <VFRendering/Options.hxx>
#include <VFRendering/FPSCounter.hxx>
#include <VFRendering/Utilities.hxx>
#include <VFRendering/Geometry.hxx>

namespace VFRendering {
class RendererBase;
class View;

using Options = ::Options<View>;

template<int index>
struct Option;


enum class CameraMovementModes {
    TRANSLATE,
    ROTATE
};

enum class WidgetLocation {
    BOTTOM_LEFT,
    BOTTOM_RIGHT,
    TOP_LEFT,
    TOP_RIGHT
};

enum class VisualizationMode {
    SURFACE,
    ISOSURFACE,
    ARROWS,
    SPHERE
};


class View {
public:
    enum Option {
        SHOW_BOUNDING_BOX,
        BOUNDING_BOX_MIN,
        BOUNDING_BOX_MAX,
        SYSTEM_CENTER,
        SHOW_MINIVIEW,
        MINIVIEW_LOCATION,
        SHOW_COORDINATE_SYSTEM,
        COORDINATE_SYSTEM_LOCATION,
        VISUALIZATION_MODE,
        VERTICAL_FIELD_OF_VIEW,
        BACKGROUND_COLOR,
        COLORMAP_IMPLEMENTATION,
        Z_RANGE,
        CAMERA_POSITION,
        CENTER_POSITION,
        UP_VECTOR
    };

    View();
    virtual ~View();
    void draw();

    void mouseMove(const glm::vec2& position_before, const glm::vec2& position_after, CameraMovementModes mode);
    void mouseScroll(const float& wheel_delta);
    void setCamera(glm::vec3 camera_position);
    void setCamera(glm::vec3 camera_position, glm::vec3 center_position, glm::vec3 up_vector);
    void setFramebufferSize(float width, float height);
    float getFramerate() const;

    void updateOptions(const Options& options);
    void options(const Options& options);
    const Options& options() const;

    void update(const Geometry& geometry, const std::vector<glm::vec3>& vectors);
    void updateVectors(const std::vector<glm::vec3>& vectors);

    const std::vector<glm::vec3>& positions() const;
    const std::vector<glm::vec3>& directions() const;
    const std::vector<std::array<Geometry::index_type, 3>>& surfaceIndices() const;
    const std::vector<std::array<Geometry::index_type, 4>>& volumeIndices() const;

private:
    void updateRenderers();
    void optionsHaveChanged(const std::vector<int>& changed_options);

    Geometry m_geometry;
    std::vector<glm::vec3> m_vectors;
    std::vector<std::pair<std::shared_ptr<RendererBase>, std::array<float, 4>>> m_renderers;
    Utilities::Camera m_camera;
    Utilities::FPSCounter m_fps_counter;
    glm::vec2 m_framebuffer_size;
    bool m_is_centered;

    Options m_options;
};

}

template<>
template<int index>
struct Options<VFRendering::View>::Option : public VFRendering::Option<index> {};

template<>
struct VFRendering::Option<VFRendering::View::Option::SHOW_BOUNDING_BOX> {
    bool default_value = true;
};

template<>
struct VFRendering::Option<VFRendering::View::Option::BOUNDING_BOX_MIN> {
    glm::vec3 default_value = {-1, -1, -1};
};

template<>
struct VFRendering::Option<VFRendering::View::Option::BOUNDING_BOX_MAX> {
    glm::vec3 default_value = {1, 1, 1};
};

template<>
struct VFRendering::Option<VFRendering::View::Option::SYSTEM_CENTER> {
    glm::vec3 default_value = {0, 0, 0};
};

template<>
struct VFRendering::Option<VFRendering::View::Option::SHOW_MINIVIEW> {
    bool default_value = true;
};

template<>
struct VFRendering::Option<VFRendering::View::Option::MINIVIEW_LOCATION> {
    VFRendering::WidgetLocation default_value = VFRendering::WidgetLocation::BOTTOM_LEFT;
};

template<>
struct VFRendering::Option<VFRendering::View::Option::SHOW_COORDINATE_SYSTEM> {
    bool default_value = true;
};

template<>
struct VFRendering::Option<VFRendering::View::Option::COORDINATE_SYSTEM_LOCATION> {
    VFRendering::WidgetLocation default_value = VFRendering::WidgetLocation::BOTTOM_RIGHT;
};

template<>
struct VFRendering::Option<VFRendering::View::Option::VISUALIZATION_MODE> {
    VFRendering::VisualizationMode default_value = VFRendering::VisualizationMode::ARROWS;
};

template<>
struct VFRendering::Option<VFRendering::View::Option::VERTICAL_FIELD_OF_VIEW> {
    float default_value = 45.0;
};

template<>
struct VFRendering::Option<VFRendering::View::Option::BACKGROUND_COLOR> {
    glm::vec3 default_value = {0, 0, 0};
};

template<>
struct VFRendering::Option<VFRendering::View::Option::COLORMAP_IMPLEMENTATION> {
    std::string default_value = VFRendering::Utilities::getColormapImplementation(VFRendering::Utilities::Colormap::DEFAULT);
};

template<>
struct VFRendering::Option<VFRendering::View::Option::Z_RANGE> {
    glm::vec2 default_value = {-1, 1};
};

template<>
struct VFRendering::Option<VFRendering::View::Option::CAMERA_POSITION> {
    glm::vec3 default_value = {14.5, 14.5, 30};
};

template<>
struct VFRendering::Option<VFRendering::View::Option::CENTER_POSITION> {
    glm::vec3 default_value = {14.5, 14.5, 0};
};

template<>
struct VFRendering::Option<VFRendering::View::Option::UP_VECTOR> {
    glm::vec3 default_value = {0, 1, 0};
};

#endif
