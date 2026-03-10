#pragma once

#include "Camera.h"
#include "Renderer.h"
#include "Scene.h"
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
    Camera   m_camera;
    Scene    m_scene;
    Renderer m_renderer;

    QPoint m_lastPos;
    bool   m_glReady = false;
};
