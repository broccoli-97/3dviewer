#include "GLWidget.h"

#include <algorithm>
#include <cmath>
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
    // slight initial tilt so the model isn't viewed head-on
    glm::quat pitchQ = glm::angleAxis(glm::radians(20.0f), glm::vec3(1, 0, 0));
    glm::quat yawQ   = glm::angleAxis(glm::radians(-30.0f), glm::vec3(0, 1, 0));
    m_rotation = pitchQ * yawQ;

    m_panX = 0.0f;
    m_panY = 0.0f;
    m_distance = m_model ? m_model->getExtent() * 1.5f : 3.0f;
}

// --------------- arcball helpers ---------------

glm::vec3 GLWidget::mapToSphere(const QPoint &pos) const
{
    // Map pixel position to [-1, 1] using the smaller viewport dimension as radius
    const float radius = std::min(width(), height()) * 0.5f;
    const float cx = width() * 0.5f;
    const float cy = height() * 0.5f;

    float x = (pos.x() - cx) / radius;
    float y = (cy - pos.y()) / radius;   // flip Y — screen Y is top-down

    float len2 = x * x + y * y;
    if (len2 <= 1.0f) {
        // inside the sphere — project onto hemisphere
        return glm::vec3(x, y, std::sqrt(1.0f - len2));
    }
    // outside the sphere — project onto edge (normalize to unit circle)
    float s = 1.0f / std::sqrt(len2);
    return glm::vec3(x * s, y * s, 0.0f);
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
    view *= glm::mat4_cast(m_rotation);

    m_renderer.render(this, *m_model, projection, view);
}

// --------------- mouse interaction ---------------

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    m_lastPos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        // arcball rotation
        glm::vec3 va = mapToSphere(m_lastPos);
        glm::vec3 vb = mapToSphere(event->pos());

        float dot = glm::clamp(glm::dot(va, vb), -1.0f, 1.0f);
        float angle = std::acos(dot);

        if (angle > 1e-5f) {
            glm::vec3 axis = glm::normalize(glm::cross(va, vb));
            glm::quat drag = glm::angleAxis(angle, axis);
            m_rotation = drag * m_rotation;
        }
    }

    if (event->buttons() & (Qt::RightButton | Qt::MiddleButton)) {
        const int dx = event->pos().x() - m_lastPos.x();
        const int dy = event->pos().y() - m_lastPos.y();
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
