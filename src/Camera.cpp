#include "Camera.h"

#include <algorithm>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>

glm::mat4 Camera::viewMatrix() const
{
    glm::mat4 view(1.0f);
    view = glm::translate(view, glm::vec3(m_panX, m_panY, -m_distance));
    view *= glm::mat4_cast(m_rotation);
    return view;
}

glm::mat4 Camera::projectionMatrix(float aspect) const
{
    const float nearP = std::max(0.001f, m_distance * 0.01f);
    const float farP  = m_distance * 100.0f;
    return glm::perspective(glm::radians(45.0f), aspect, nearP, farP);
}

glm::vec3 Camera::position() const
{
    return glm::vec3(glm::inverse(viewMatrix()) * glm::vec4(0, 0, 0, 1));
}

void Camera::reset(float modelExtent)
{
    glm::quat pitchQ = glm::angleAxis(glm::radians(20.0f),  glm::vec3(1, 0, 0));
    glm::quat yawQ   = glm::angleAxis(glm::radians(-30.0f), glm::vec3(0, 1, 0));
    m_rotation = pitchQ * yawQ;

    m_panX = 0.0f;
    m_panY = 0.0f;
    m_distance = modelExtent * 1.5f;
}

// --------------- arcball ---------------

glm::vec3 Camera::mapToSphere(const QPoint &pos, int w, int h) const
{
    const float radius = std::min(w, h) * 0.5f;
    const float cx = w * 0.5f;
    const float cy = h * 0.5f;

    float x = (pos.x() - cx) / radius;
    float y = (cy - pos.y()) / radius;

    float len2 = x * x + y * y;
    if (len2 <= 1.0f)
        return glm::vec3(x, y, std::sqrt(1.0f - len2));

    float s = 1.0f / std::sqrt(len2);
    return glm::vec3(x * s, y * s, 0.0f);
}

void Camera::rotate(const QPoint &from, const QPoint &to,
                    int viewportW, int viewportH)
{
    glm::vec3 va = mapToSphere(from, viewportW, viewportH);
    glm::vec3 vb = mapToSphere(to,   viewportW, viewportH);

    float dot   = glm::clamp(glm::dot(va, vb), -1.0f, 1.0f);
    float angle = std::acos(dot);

    if (angle > 1e-5f) {
        glm::vec3 axis = glm::normalize(glm::cross(va, vb));
        m_rotation = glm::angleAxis(angle, axis) * m_rotation;
    }
}

void Camera::pan(int dx, int dy)
{
    const float s = m_distance * 0.002f;
    m_panX += dx * s;
    m_panY -= dy * s;
}

void Camera::zoom(float delta)
{
    m_distance *= (1.0f - delta * 0.1f);
    m_distance = std::max(m_distance, 0.001f);
}
