#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices)
    : m_vertices(std::move(vertices)), m_indices(std::move(indices))
{
}

Mesh::~Mesh() = default;

Mesh::Mesh(Mesh &&other) noexcept
    : m_vertices(std::move(other.m_vertices)),
      m_indices(std::move(other.m_indices)),
      m_vao(other.m_vao),
      m_vbo(other.m_vbo),
      m_ebo(other.m_ebo),
      m_ready(other.m_ready)
{
    other.m_vao = other.m_vbo = other.m_ebo = 0;
    other.m_ready = false;
}

Mesh &Mesh::operator=(Mesh &&other) noexcept
{
    if (this != &other) {
        m_vertices = std::move(other.m_vertices);
        m_indices = std::move(other.m_indices);
        m_vao = other.m_vao;
        m_vbo = other.m_vbo;
        m_ebo = other.m_ebo;
        m_ready = other.m_ready;
        other.m_vao = other.m_vbo = other.m_ebo = 0;
        other.m_ready = false;
    }
    return *this;
}

void Mesh::setup(QOpenGLFunctions_3_3_Core *gl)
{
    if (m_ready) return;

    gl->glGenVertexArrays(1, &m_vao);
    gl->glGenBuffers(1, &m_vbo);
    gl->glGenBuffers(1, &m_ebo);

    gl->glBindVertexArray(m_vao);

    gl->glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    gl->glBufferData(GL_ARRAY_BUFFER,
                     static_cast<GLsizeiptr>(m_vertices.size() * sizeof(Vertex)),
                     m_vertices.data(),
                     GL_STATIC_DRAW);

    gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    gl->glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     static_cast<GLsizeiptr>(m_indices.size() * sizeof(unsigned int)),
                     m_indices.data(),
                     GL_STATIC_DRAW);

    // position
    gl->glVertexAttribPointer(0,
                              3,
                              GL_FLOAT,
                              GL_FALSE,
                              sizeof(Vertex),
                              reinterpret_cast<void *>(offsetof(Vertex, position)));
    gl->glEnableVertexAttribArray(0);

    // normal
    gl->glVertexAttribPointer(1,
                              3,
                              GL_FLOAT,
                              GL_FALSE,
                              sizeof(Vertex),
                              reinterpret_cast<void *>(offsetof(Vertex, normal)));
    gl->glEnableVertexAttribArray(1);

    gl->glBindVertexArray(0);
    m_ready = true;
}

void Mesh::draw(QOpenGLFunctions_3_3_Core *gl) const
{
    if (!m_ready) return;
    gl->glBindVertexArray(m_vao);
    gl->glDrawElements(
        GL_TRIANGLES, static_cast<GLsizei>(m_indices.size()), GL_UNSIGNED_INT, nullptr);
    gl->glBindVertexArray(0);
}

void Mesh::cleanup(QOpenGLFunctions_3_3_Core *gl)
{
    if (!m_ready) return;
    gl->glDeleteVertexArrays(1, &m_vao);
    gl->glDeleteBuffers(1, &m_vbo);
    gl->glDeleteBuffers(1, &m_ebo);
    m_vao = m_vbo = m_ebo = 0;
    m_ready = false;
}
