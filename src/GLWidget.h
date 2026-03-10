#pragma once

#include "Model.h"
#include "Renderer.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <memory>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT

public:
    explicit GLWidget(QWidget *parent = nullptr);
    ~GLWidget() override;

    bool loadModel(const std::string &path);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    void resetCamera();
    glm::vec3 mapToSphere(const QPoint &pos) const;

    Renderer m_renderer;
    std::unique_ptr<Model> m_model;

    // camera — arcball rotation stored as quaternion
    glm::quat m_rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    float m_panX = 0.0f;
    float m_panY = 0.0f;
    float m_distance = 3.0f;

    QPoint m_lastPos;
    bool m_glReady = false;
};
