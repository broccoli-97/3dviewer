#include "Scene.h"

bool Scene::loadModel(const std::string &path, QOpenGLFunctions_3_3_Core *gl)
{
    auto newModel = std::make_unique<Model>();
    if (!newModel->load(path)) return false;

    if (m_model && gl) m_model->cleanup(gl);
    m_model = std::move(newModel);
    if (gl) m_model->setup(gl);
    return true;
}

void Scene::setup(QOpenGLFunctions_3_3_Core *gl)
{
    if (m_model) m_model->setup(gl);
}

void Scene::draw(QOpenGLFunctions_3_3_Core *gl) const
{
    if (m_model) m_model->draw(gl);
}

void Scene::cleanup(QOpenGLFunctions_3_3_Core *gl)
{
    if (m_model) m_model->cleanup(gl);
}

bool Scene::hasModel() const
{
    return m_model && m_model->isLoaded();
}

glm::vec3 Scene::modelCenter() const
{
    return m_model ? m_model->getCenter() : glm::vec3(0.0f);
}

float Scene::modelExtent() const
{
    return m_model ? m_model->getExtent() : 1.0f;
}
