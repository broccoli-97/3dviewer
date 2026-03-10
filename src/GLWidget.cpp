#include "GLWidget.h"

#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
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
    if (m_model) m_model->cleanup(this);
    m_renderer.cleanup();
    doneCurrent();
}

bool GLWidget::loadModel(const std::string &path)
{
    auto newModel = std::make_unique<Model>();
    if (!newModel->load(path)) return false;

    makeCurrent();
    if (m_model) m_model->cleanup(this);
    m_model = std::move(newModel);
    if (m_glReady) m_model->setup(this);
    resetCamera();
    doneCurrent();
    update();
    return true;
}

void GLWidget::resetCamera()
{
    m_rotX = 20.0f;
    m_rotY = -30.0f;
    m_panX = 0.0f;
    m_panY = 0.0f;
    m_distance = m_model ? m_model->getExtent() * 1.5f : 3.0f;
}

// --------------- GL callbacks ---------------

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    m_renderer.init(this);

    if (m_model) m_model->setup(this);
    m_glReady = true;
}

void GLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!m_model || !m_model->isLoaded()) return;

    const float aspect = (height() > 0) ? float(width()) / float(height()) : 1.0f;
    const float nearP = std::max(0.001f, m_distance * 0.01f);
    const float farP = m_distance * 100.0f;

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect, nearP, farP);

    glm::mat4 view(1.0f);
    view = glm::translate(view, glm::vec3(m_panX, m_panY, -m_distance));
    view = glm::rotate(view, glm::radians(m_rotX), glm::vec3(1, 0, 0));
    view = glm::rotate(view, glm::radians(m_rotY), glm::vec3(0, 1, 0));

    m_renderer.render(this, *m_model, projection, view);
}

// --------------- mouse interaction ---------------

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    m_lastPos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    const int dx = event->pos().x() - m_lastPos.x();
    const int dy = event->pos().y() - m_lastPos.y();

    if (event->buttons() & Qt::LeftButton) {
        m_rotY += dx * 0.5f;
        m_rotX += dy * 0.5f;
        m_rotX = glm::clamp(m_rotX, -89.0f, 89.0f);
    }

    if (event->buttons() & (Qt::RightButton | Qt::MiddleButton)) {
        const float s = m_distance * 0.002f;
        m_panX += dx * s;
        m_panY -= dy * s;
    }

    m_lastPos = event->pos();
    update();
}

void GLWidget::wheelEvent(QWheelEvent *event)
{
    const float delta = event->angleDelta().y() / 120.0f;
    m_distance *= (1.0f - delta * 0.1f);
    m_distance = std::max(m_distance, 0.001f);
    update();
}
