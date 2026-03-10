#include "Model.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glm/common.hpp>
#include <limits>

bool Model::load(const std::string &path)
{
    Assimp::Importer importer;

    const aiScene *scene = importer.ReadFile(
        path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices);

    if (!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode) return false;

    m_meshes.clear();
    m_gpuReady = false;
    m_boundsMin = glm::vec3(std::numeric_limits<float>::max());
    m_boundsMax = glm::vec3(std::numeric_limits<float>::lowest());

    processNode(scene->mRootNode, scene);
    computeBounds();

    return !m_meshes.empty();
}

void Model::processNode(aiNode *node, const aiScene *scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        m_meshes.push_back(processMesh(mesh));
    }
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh *mesh)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    vertices.reserve(mesh->mNumVertices);
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex v;
        v.position = {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};

        m_boundsMin = glm::min(m_boundsMin, v.position);
        m_boundsMax = glm::max(m_boundsMax, v.position);

        if (mesh->HasNormals()) {
            v.normal = {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};
        } else {
            v.normal = {0.0f, 1.0f, 0.0f};
        }
        vertices.push_back(v);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        const aiFace &face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    return Mesh(std::move(vertices), std::move(indices));
}

void Model::computeBounds()
{
    m_center = (m_boundsMin + m_boundsMax) * 0.5f;
    m_extent = glm::length(m_boundsMax - m_boundsMin);
    if (m_extent < 1e-6f) m_extent = 1.0f;
}

void Model::setup(QOpenGLFunctions_3_3_Core *gl)
{
    for (auto &mesh : m_meshes) mesh.setup(gl);
    m_gpuReady = true;
}

void Model::draw(QOpenGLFunctions_3_3_Core *gl) const
{
    if (!m_gpuReady) return;
    for (const auto &mesh : m_meshes) mesh.draw(gl);
}

void Model::cleanup(QOpenGLFunctions_3_3_Core *gl)
{
    for (auto &mesh : m_meshes) mesh.cleanup(gl);
    m_gpuReady = false;
}
