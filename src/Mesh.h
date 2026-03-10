#pragma once

#include <glm/glm.hpp>
#include <QOpenGLFunctions_3_3_Core>
#include <vector>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
};

class Mesh
{
public:
    Mesh() = default;
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);
    ~Mesh();

    Mesh(Mesh &&other) noexcept;
    Mesh &operator=(Mesh &&other) noexcept;

    Mesh(const Mesh &) = delete;
    Mesh &operator=(const Mesh &) = delete;

    void setup(QOpenGLFunctions_3_3_Core *gl);
    void draw(QOpenGLFunctions_3_3_Core *gl) const;
    void cleanup(QOpenGLFunctions_3_3_Core *gl);

private:
    std::vector<Vertex> m_vertices;
    std::vector<unsigned int> m_indices;
    unsigned int m_vao = 0, m_vbo = 0, m_ebo = 0;
    bool m_ready = false;
};
