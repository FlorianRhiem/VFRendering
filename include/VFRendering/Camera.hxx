#ifndef VFRENDERING_CAMERA_H
#define VFRENDERING_CAMERA_H

#include <glm/glm.hpp>

namespace VFRendering {
namespace Utilities {
class Camera {
public:
    Camera();
    Camera(const glm::vec3& camera_position, const glm::vec3& center_position, const glm::vec3& up_vector, const float& aspect_ratio);

    void setAspectRatio(float aspect_ratio);
    void lookAt(glm::vec3 eye, glm::vec3 center, glm::vec3 up);

    const glm::vec3& cameraPosition() const;
    const glm::vec3& centerPosition() const;
    const glm::vec3& upVector() const;
    const float& aspectRatio() const;

private:
    glm::vec3 m_camera_position;
    glm::vec3 m_center_position;
    glm::vec3 m_up_vector;
    float m_aspect_ratio;
};
}
}

#endif
