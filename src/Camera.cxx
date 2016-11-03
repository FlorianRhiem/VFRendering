#include "VFRendering/Camera.hxx"

#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace VFRendering {
namespace Utilities {
Camera::Camera() : Camera(glm::vec3(0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), 1.0) {}

Camera::Camera(const glm::vec3& camera_position, const glm::vec3& center_position, const glm::vec3& up_vector, const float& aspect_ratio) : m_camera_position(camera_position), m_center_position(center_position), m_up_vector(up_vector), m_aspect_ratio(aspect_ratio) {}

void Camera::lookAt(glm::vec3 eye, glm::vec3 center, glm::vec3 up) {
    m_camera_position = eye;
    m_center_position = center;
    m_up_vector = up;
}

void Camera::setAspectRatio(float aspect_ratio) {
    m_aspect_ratio = aspect_ratio;
}

const glm::vec3& Camera::cameraPosition() const {
    return m_camera_position;
}

const glm::vec3& Camera::centerPosition() const {
    return m_center_position;
}

const glm::vec3& Camera::upVector() const {
    return m_up_vector;
}

const float& Camera::aspectRatio() const {
    return m_aspect_ratio;
}

}
}
