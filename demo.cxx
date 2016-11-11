#include <iostream>
#include <fstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "VFRendering/View.hxx"
#include "VFRendering/CombinedRenderer.hxx"
#include "VFRendering/ArrowRenderer.hxx"
#include "VFRendering/IsosurfaceRenderer.hxx"

static bool needs_redraw = false;
static VFRendering::View* view_ptr = nullptr;

void mouseWheelCallback(GLFWwindow* window, double x_offset, double y_offset) {
    (void)window;
    (void)x_offset;
    if (view_ptr) {
        view_ptr->mouseScroll(y_offset);
        needs_redraw = true;
    }
}

void mousePositionCallback(GLFWwindow* window, double x_position, double y_position) {
    static glm::vec2 previous_mouse_position(0, 0);
    glm::vec2 current_mouse_position(x_position, y_position);
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        if (view_ptr) {
            auto movement_mode = VFRendering::CameraMovementModes::ROTATE;
            if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
                movement_mode = VFRendering::CameraMovementModes::TRANSLATE;
            }
            view_ptr->mouseMove(previous_mouse_position, current_mouse_position, movement_mode);
            needs_redraw = true;
        }
    }
    previous_mouse_position = current_mouse_position;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    (void)window;
    if (view_ptr) {
        view_ptr->setFramebufferSize(width, height);
        needs_redraw = true;
    }
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    (void)window;
    (void)scancode;
    if (mods != 0) {
        return;
    }
    if (action != GLFW_PRESS && action != GLFW_REPEAT) {
        return;
    }
    switch (key) {
    case GLFW_KEY_R:
        if (view_ptr) {
            view_ptr->setCamera(glm::vec3(0, 0, 30), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
            needs_redraw = true;
        }
        break;
    case GLFW_KEY_1:
        if (view_ptr) {
            view_ptr->renderers(VFRendering::VisualizationMode::ARROWS, true, true, VFRendering::WidgetLocation::BOTTOM_LEFT, true, VFRendering::WidgetLocation::BOTTOM_RIGHT);
            needs_redraw = true;
        }
        break;
    case GLFW_KEY_2:
        if (view_ptr) {
            view_ptr->renderers(VFRendering::VisualizationMode::SURFACE, true, true, VFRendering::WidgetLocation::BOTTOM_LEFT, true, VFRendering::WidgetLocation::BOTTOM_RIGHT);
            needs_redraw = true;
        }
        break;
    case GLFW_KEY_3:
        if (view_ptr) {
            view_ptr->renderers(VFRendering::VisualizationMode::ISOSURFACE, true, true, VFRendering::WidgetLocation::BOTTOM_LEFT, true, VFRendering::WidgetLocation::BOTTOM_RIGHT);
            needs_redraw = true;
        }
        break;
    }
}

void windowRefreshCallback(GLFWwindow* window) {
    (void)window;
    needs_redraw = true;
}

int main(void) {
    GLFWwindow* window;
    if (!glfwInit()) {
        return -1;
    }
    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
    glfwWindowHint(GLFW_SAMPLES, 16);
    window = glfwCreateWindow(800, 800, "Demo", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetScrollCallback(window, mouseWheelCallback);
    glfwSetCursorPosCallback(window, mousePositionCallback);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetKeyCallback(window, keyCallback);
    VFRendering::View view;
    glEnable(GL_MULTISAMPLE);
    view_ptr = &view;
    framebufferSizeCallback(window, 800, 800);

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> directions;
    {
        std::ifstream f("data.txt");
        while (f.good()) {
            glm::vec3 position;
            glm::vec3 direction;
            f >> position.x >> position.y >> position.z >> direction.x >> direction.y >> direction.z;
            if (f.good()) {
                positions.push_back(position * 10.0f);
                directions.push_back(direction);
            }
        }
    }
    VFRendering::Geometry geometry = VFRendering::Geometry::cartesianGeometry({21, 21, 21}, {-20, -20, -20}, {20, 20, 20});

    view.update(geometry, directions);
    VFRendering::Options options;
    options.set<VFRendering::View::Option::BOUNDING_BOX_MIN>(geometry.min());
    options.set<VFRendering::View::Option::BOUNDING_BOX_MAX>(geometry.max());
    options.set<VFRendering::View::Option::SYSTEM_CENTER>((geometry.min() + geometry.max()) * 0.5f);
    options.set<VFRendering::View::Option::COLORMAP_IMPLEMENTATION>(VFRendering::Utilities::getColormapImplementation(VFRendering::Utilities::Colormap::HSV));
    view.updateOptions(options);

    std::vector<std::shared_ptr<VFRendering::RendererBase>> renderers;
    renderers.push_back(std::make_shared<VFRendering::IsosurfaceRenderer>(view, [] (const glm::vec3& position, const glm::vec3& direction) -> VFRendering::IsosurfaceRenderer::isovalue_type {
        (void)position;
        return direction.z;
    }, 0.0));
    renderers.push_back(std::make_shared<VFRendering::IsosurfaceRenderer>(view, [] (const glm::vec3& position, const glm::vec3& direction) -> VFRendering::IsosurfaceRenderer::isovalue_type {
        (void)direction;
        return position.x;
    }, 0.0));
    renderers.push_back(std::make_shared<VFRendering::ArrowRenderer>(view));
    view.renderers({{std::make_shared<VFRendering::CombinedRenderer>(view, renderers), {{0, 0, 1, 1}}}});

    while (!glfwWindowShouldClose(window)) {
        if (needs_redraw) {
            needs_redraw = false;
            view.draw();
            glfwSwapBuffers(window);
        }
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
