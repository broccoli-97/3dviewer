#include "Renderer.h"
#include "Camera.h"
#include "Scene.h"

#include <QFile>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

static QString loadResource(const QString &path)
{
    QFile f(path);
    f.open(QIODevice::ReadOnly | QIODevice::Text);
    return f.readAll();
}

bool Renderer::init(QOpenGLFunctions_3_3_Core *gl)
{
    Q_UNUSED(gl);

    m_shader = new QOpenGLShaderProgram;
    if (!m_shader->addShaderFromSourceCode(
            QOpenGLShader::Vertex, loadResource(":/shaders/phong.vs")))
        return false;
    if (!m_shader->addShaderFromSourceCode(
            QOpenGLShader::Fragment, loadResource(":/shaders/phong.fs")))
        return false;
    if (!m_shader->link())
        return false;

    m_locModel        = m_shader->uniformLocation("model");
    m_locView         = m_shader->uniformLocation("view");
    m_locProjection   = m_shader->uniformLocation("projection");
    m_locNormalMatrix = m_shader->uniformLocation("normalMatrix");
    m_locLightDir     = m_shader->uniformLocation("lightDir");
    m_locViewPos      = m_shader->uniformLocation("viewPos");
    m_locObjectColor  = m_shader->uniformLocation("objectColor");

    return true;
}

void Renderer::render(QOpenGLFunctions_3_3_Core *gl, const Scene &scene,
                      const Camera &camera, float aspect)
{
    glm::mat4 projection = camera.projectionMatrix(aspect);
    glm::mat4 view       = camera.viewMatrix();
    glm::mat4 modelMat   = glm::translate(glm::mat4(1.0f), -scene.modelCenter());
    glm::mat3 normMat    = glm::transpose(glm::inverse(glm::mat3(modelMat)));
    glm::vec3 viewPos    = camera.position();
    glm::vec3 lightDir   = scene.lightDirection();
    glm::vec3 objColor   = scene.objectColor();

    m_shader->bind();

    gl->glUniformMatrix4fv(m_locProjection,   1, GL_FALSE, glm::value_ptr(projection));
    gl->glUniformMatrix4fv(m_locView,         1, GL_FALSE, glm::value_ptr(view));
    gl->glUniformMatrix4fv(m_locModel,        1, GL_FALSE, glm::value_ptr(modelMat));
    gl->glUniformMatrix3fv(m_locNormalMatrix, 1, GL_FALSE, glm::value_ptr(normMat));

    gl->glUniform3fv(m_locLightDir,    1, glm::value_ptr(lightDir));
    gl->glUniform3fv(m_locViewPos,     1, glm::value_ptr(viewPos));
    gl->glUniform3fv(m_locObjectColor, 1, glm::value_ptr(objColor));

    scene.draw(gl);

    m_shader->release();
}

void Renderer::cleanup()
{
    delete m_shader;
    m_shader = nullptr;
}
