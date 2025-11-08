#define GLAD_IMPLEMENTATION
#include "glad/glad.h"

struct Vao {
    GLuint id {};
    Vao() { glGenVertexArrays(1, &id); }
    ~Vao() { glDeleteVertexArrays(1, &id); }
    void bind() const { glBindVertexArray(id); }
    void unbind() const { glBindVertexArray(0); }
    void attrib(GLuint index, GLint size, GLenum type, GLboolean norm, GLsizei stride, GLintptr offset) const { 
        glVertexAttribPointer(index, size, type, norm, stride, (void*)offset);
        glEnableVertexAttribArray(index);
    }
};

struct Vbo {
    GLuint id {};
    GLenum target {};

    explicit Vbo(GLenum t = GL_ARRAY_BUFFER)
        : target(t)
    {
        glGenBuffers(1, &id);
    }
    ~Vbo() { glDeleteBuffers(1, &id); }

    void bind() const { glBindBuffer(target, id); }
    void unbind() const { glBindBuffer(target, 0); }

    void fill(GLsizeiptr size, const void* data, GLenum usage = GL_STATIC_DRAW) const
    {
        glBufferData(target, size, data, usage);
    }
};

struct Ebo {
    GLuint id {};
    GLenum target {};

    explicit Ebo(GLenum t = GL_ELEMENT_ARRAY_BUFFER)
        : target(t)
    {
        glGenBuffers(1, &id);
    }
    ~Ebo() { glDeleteBuffers(1, &id); }

    void bind() const { glBindBuffer(target, id); }
    void unbind() const { glBindBuffer(target, 0); }

    void fill(GLsizeiptr size, const void* data, GLenum usage = GL_STATIC_DRAW) const
    {
        glBufferData(target, size, data, usage);
    }
};
