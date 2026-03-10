#include "GLWidget.h"

#include <QMouseEvent>
#include <QWheelEvent>

// --------------- GLWidget ---------------

GLWidget::GLWidget(QWidget *parent) : QOpenGLWidget(parent)
{
    setFocusPolicy(Qt::StrongFocus);
}

GLWidget::~GLWidget()
{
    makeCurrent();
    m_scene.cleanup(this);
    m_renderer.cleanup();
    doneCurrent();
}

bool GLWidget::loadModel(const std::string &path)
{
    makeCurrent();
    bool ok = m_scene.loadModel(path, m_glReady ? this : nullptr);
    if (ok) m_camera.reset(m_scene.modelExtent());
    doneCurrent();
    update();
    return ok;
}

// --------------- GL callbacks ---------------

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    m_renderer.init(this);
    m_scene.setup(this);
    m_glReady = true;
}

void GLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!m_scene.hasModel()) return;

    const float aspect = (height() > 0) ? float(width()) / float(height()) : 1.0f;
    m_renderer.render(this, m_scene, m_camera, aspect);
}

// --------------- mouse interaction ---------------

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    m_lastPos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
        m_camera.rotate(m_lastPos, event->pos(), width(), height());

    if (event->buttons() & (Qt::RightButton | Qt::MiddleButton)) {
        m_camera.pan(event->pos().x() - m_lastPos.x(),
                     event->pos().y() - m_lastPos.y());
    }

    m_lastPos = event->pos();
    update();
}

void GLWidget::wheelEvent(QWheelEvent *event)
{
    m_camera.zoom(event->angleDelta().y() / 120.0f);
    update();
}
