#pragma once

#include "Model.h"
#include <glm/glm.hpp>
#include <memory>

class Scene
{
public:
    bool loadModel(const std::string &path, QOpenGLFunctions_3_3_Core *gl);
    void setup(QOpenGLFunctions_3_3_Core *gl);
    void draw(QOpenGLFunctions_3_3_Core *gl) const;
    void cleanup(QOpenGLFunctions_3_3_Core *gl);

    bool hasModel() const;
    glm::vec3 modelCenter() const;
    float modelExtent() const;

    glm::vec3 lightDirection() const { return m_lightDir; }
    glm::vec3 objectColor() const { return m_objectColor; }

private:
    std::unique_ptr<Model> m_model;
    glm::vec3 m_lightDir{-0.3f, -1.0f, -0.5f};
    glm::vec3 m_objectColor{0.7f, 0.75f, 0.8f};
};
