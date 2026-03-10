#pragma once

#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <glm/glm.hpp>

class Model;

class Renderer
{
public:
    bool init(QOpenGLFunctions_3_3_Core *gl);
    void render(QOpenGLFunctions_3_3_Core *gl, const Model &model,
                const glm::mat4 &projection, const glm::mat4 &view);
    void cleanup();

private:
    QOpenGLShaderProgram *m_shader = nullptr;

    // cached uniform locations
    int m_locModel = -1;
    int m_locView = -1;
    int m_locProjection = -1;
    int m_locNormalMatrix = -1;
    int m_locLightDir = -1;
    int m_locViewPos = -1;
    int m_locObjectColor = -1;
};
