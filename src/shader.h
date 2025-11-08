#ifndef SHADER_H
#define SHADER_H

#define GLAD_IMPLEMENTATION
#include "glad/glad.h"

#include "file.h"

struct Shader {
    GLuint program = 0;

    bool init(const std::string& vertexSourcePath, const std::string& fragmentSourcePath)
    {
        auto vertSource = File::readFile(vertexSourcePath);
        if (!vertSource.has_value()) {
            fprintf(stderr, "err: failed to read vertex shader file: %s\n", vertexSourcePath.c_str());
            return false;
        }

        auto fragSource = File::readFile(fragmentSourcePath);
        if (!fragSource.has_value()) {
            fprintf(stderr, "err: failed to read fragment shader file: %s\n", fragmentSourcePath.c_str());
            return false;
        }

        GLuint vertShader = createShader(GL_VERTEX_SHADER, vertSource.value().c_str(), "vertex");
        GLuint fragShader = createShader(GL_FRAGMENT_SHADER, fragSource.value().c_str(), "fragment");

        program = glCreateProgram();
        if (!program) {
            fprintf(stderr, "err: failed to create shader program\n");
            glDeleteShader(vertShader);
            glDeleteShader(fragShader);
            return false;
        }

        glAttachShader(program, vertShader);
        glAttachShader(program, fragShader);
        glLinkProgram(program);

        glDeleteShader(vertShader);
        glDeleteShader(fragShader);

        return true;
    }

    GLuint createShader(GLenum type, const char* source, const char* name)
    {
        GLuint shader = glCreateShader(type);
        if (shader == GL_INVALID_ENUM) {
            fprintf(stderr, "err: invalid enum type for %s shader\n", name);
            return 0;
        }

        if (!shader) {
            fprintf(stderr, "err: failed to create %s shader\n", name);
            return 0;
        }

        glShaderSource(shader, 1, &source, NULL);
        glCompileShader(shader);
        if (!shaderCompiled(shader, name)) {
            return 0;
        }

        return shader;
    }

    bool shaderCompiled(GLuint shader, const char* name) const
    {
        GLint compiled;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLsizei logLength;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

            GLchar infoLog[logLength];
            glGetShaderInfoLog(shader, logLength, NULL, infoLog);

            fprintf(stderr, "err: failed to compile %s shader: %s\n", name, infoLog);
            return false;
        }

        return true;
    }

    bool programLinked(GLuint program) const
    {
        GLint linked;
        glGetProgramiv(program, GL_LINK_STATUS, &linked);
        if (!linked) {
            GLsizei logLength;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);

            GLchar infoLog[logLength];
            glGetProgramInfoLog(program, logLength, NULL, infoLog);

            fprintf(stderr, "err: failed to link shader program: %s\n", infoLog);
            return false;
        }

        return true;
    }
};

#endif // SHADER_H
