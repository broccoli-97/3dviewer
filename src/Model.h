#pragma once

#include "Mesh.h"
#include <glm/glm.hpp>
#include <string>
#include <vector>

struct aiNode;
struct aiScene;
struct aiMesh;

class Model
{
public:
    Model() = default;

    bool load(const std::string &path);
    void setup(QOpenGLFunctions_3_3_Core *gl);
    void draw(QOpenGLFunctions_3_3_Core *gl) const;
    void cleanup(QOpenGLFunctions_3_3_Core *gl);

    glm::vec3 getCenter() const { return m_center; }
    float getExtent() const { return m_extent; }
    bool isLoaded() const { return !m_meshes.empty(); }

private:
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh);
    void computeBounds();

    std::vector<Mesh> m_meshes;
    glm::vec3 m_boundsMin{0.0f};
    glm::vec3 m_boundsMax{0.0f};
    glm::vec3 m_center{0.0f};
    float m_extent = 1.0f;
    bool m_gpuReady = false;
};
