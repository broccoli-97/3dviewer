#include "GLWidget.h"

#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <QMouseEvent>
#include <QWheelEvent>

// --------------- shaders ---------------

static const char *kVertSrc = R"glsl(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;

out vec3 FragPos;
out vec3 Normal;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal  = normalMatrix * aNormal;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
)glsl";

static const char *kFragSrc = R"glsl(
#version 330 core
in vec3 FragPos;
in vec3 Normal;

uniform vec3 lightDir;
uniform vec3 viewPos;
uniform vec3 objectColor;

out vec4 FragColor;

void main() {
    vec3 norm = normalize(Normal);
    vec3 ld   = normalize(-lightDir);

    // two-sided lighting
    if (dot(norm, ld) < 0.0)
        norm = -norm;

    // ambient
    vec3 ambient = 0.15 * vec3(1.0);

    // diffuse
    float diff = max(dot(norm, ld), 0.0);
    vec3 diffuse = diff * vec3(1.0);

    // specular (Blinn-Phong)
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 half_   = normalize(ld + viewDir);
    float spec   = pow(max(dot(norm, half_), 0.0), 64.0);
    vec3 specular = 0.4 * spec * vec3(1.0);

    FragColor = vec4((ambient + diffuse + specular) * objectColor, 1.0);
}
)glsl";

// --------------- GLWidget ---------------

GLWidget::GLWidget(QWidget *parent) : QOpenGLWidget(parent)
{
    setFocusPolicy(Qt::StrongFocus);
}

GLWidget::~GLWidget()
{
    makeCurrent();
    if (m_model) m_model->cleanup(this);
    delete m_shader;
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

    m_shader = new QOpenGLShaderProgram(this);
    m_shader->addShaderFromSourceCode(QOpenGLShader::Vertex, kVertSrc);
    m_shader->addShaderFromSourceCode(QOpenGLShader::Fragment, kFragSrc);
    m_shader->link();

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

    glm::mat4 modelMat = glm::translate(glm::mat4(1.0f), -m_model->getCenter());
    glm::mat3 normMat = glm::transpose(glm::inverse(glm::mat3(modelMat)));
    glm::vec3 viewPos = glm::vec3(glm::inverse(view) * glm::vec4(0, 0, 0, 1));

    m_shader->bind();

    glUniformMatrix4fv(
        m_shader->uniformLocation("projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(m_shader->uniformLocation("view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(m_shader->uniformLocation("model"), 1, GL_FALSE, glm::value_ptr(modelMat));
    glUniformMatrix3fv(
        m_shader->uniformLocation("normalMatrix"), 1, GL_FALSE, glm::value_ptr(normMat));

    glm::vec3 lightDir(-0.3f, -1.0f, -0.5f);
    glUniform3fv(m_shader->uniformLocation("lightDir"), 1, glm::value_ptr(lightDir));
    glUniform3fv(m_shader->uniformLocation("viewPos"), 1, glm::value_ptr(viewPos));
    glUniform3f(m_shader->uniformLocation("objectColor"), 0.7f, 0.75f, 0.8f);

    m_model->draw(this);

    m_shader->release();
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
