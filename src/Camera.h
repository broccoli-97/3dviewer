#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <QPoint>

class Camera
{
public:
    glm::mat4 viewMatrix() const;
    glm::mat4 projectionMatrix(float aspect) const;
    glm::vec3 position() const;

    void reset(float modelExtent);

    // input — screen space
    void rotate(const QPoint &from, const QPoint &to, int viewportW, int viewportH);
    void pan(int dx, int dy);
    void zoom(float delta);

private:
    glm::vec3 mapToSphere(const QPoint &pos, int w, int h) const;

    glm::quat m_rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    float m_panX = 0.0f;
    float m_panY = 0.0f;
    float m_distance = 3.0f;
};
