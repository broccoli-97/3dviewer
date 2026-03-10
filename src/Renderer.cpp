#include "Renderer.h"
#include "Model.h"

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

void Renderer::render(QOpenGLFunctions_3_3_Core *gl, const Model &model,
                      const glm::mat4 &projection, const glm::mat4 &view)
{
    glm::mat4 modelMat = glm::translate(glm::mat4(1.0f), -model.getCenter());
    glm::mat3 normMat  = glm::transpose(glm::inverse(glm::mat3(modelMat)));
    glm::vec3 viewPos  = glm::vec3(glm::inverse(view) * glm::vec4(0, 0, 0, 1));

    m_shader->bind();

    gl->glUniformMatrix4fv(m_locProjection, 1, GL_FALSE, glm::value_ptr(projection));
    gl->glUniformMatrix4fv(m_locView, 1, GL_FALSE, glm::value_ptr(view));
    gl->glUniformMatrix4fv(m_locModel, 1, GL_FALSE, glm::value_ptr(modelMat));
    gl->glUniformMatrix3fv(m_locNormalMatrix, 1, GL_FALSE, glm::value_ptr(normMat));

    glm::vec3 lightDir(-0.3f, -1.0f, -0.5f);
    gl->glUniform3fv(m_locLightDir, 1, glm::value_ptr(lightDir));
    gl->glUniform3fv(m_locViewPos, 1, glm::value_ptr(viewPos));
    gl->glUniform3f(m_locObjectColor, 0.7f, 0.75f, 0.8f);

    model.draw(gl);

    m_shader->release();
}

void Renderer::cleanup()
{
    delete m_shader;
    m_shader = nullptr;
}
