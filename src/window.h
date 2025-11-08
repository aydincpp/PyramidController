#ifndef WINDOW_H
#define WINDOW_H

#define GLAD_IMPLEMENTATION
#include "glad/glad.h"

#define GLFW_INCLUDE
#include "GLFW/glfw3.h"

#include <cstdlib>
#include <string>

struct Window {
    GLFWwindow* m_handle;
    int m_width;
    int m_height;
    bool m_fullscreen;
    bool m_maximized;

    bool init(const std::string& title, bool maximized = true, int width = 1200, int height = 720, bool fullscreen = false)
    {
        if (!glfwInit()) {
            fprintf(stderr, "failed to initialize glfw\n");
            return false;
        }

        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        m_maximized = maximized;
        m_fullscreen = fullscreen;
        m_width = (m_maximized || m_fullscreen) ? mode->width : width;
        m_height =(m_maximized || m_fullscreen) ? mode->height : height;

        m_handle = glfwCreateWindow(m_width, m_height, title.c_str(), m_fullscreen ? monitor : NULL, NULL);
        if (!m_handle) {
            fprintf(stderr, "failed to create GLFWwindow\n");
            shutdown();
            return false;
        }

        glfwSwapInterval(1);
        setHints();

        glfwMakeContextCurrent(m_handle);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            fprintf(stderr, "err: failed to initialize glad\n");
            shutdown();
            return false;
        }

        return true;
    }

    bool shouldClose() const
    {
        return glfwWindowShouldClose(m_handle);
    }

    void pollEvents() const
    {
        glfwPollEvents();
    }

    void swapBuffers() const
    {
        glfwSwapBuffers(m_handle);
    }

    void shutdown()
    {
        glfwTerminate();
    }

    GLFWwindow* get()
    {
        return m_handle;
    }

    void setHints()
    {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif
    }

    static void onResize(GLFWwindow* window, int width, int height) {
        auto self = (Window*)glfwGetWindowUserPointer(window);
        self->m_width = width;
        self->m_height = height;
        glViewport(0, 0, width, height);
    }
};

#endif // WINDOW_H
