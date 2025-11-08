#ifndef TIMER_H
#define TIMER_H

#include "GLFW/glfw3.h"

struct Timer {
    double last = 0.0f;

    void reset() {
        last = glfwGetTime();;
    }

    double delta() {
        double now = glfwGetTime();
        double dt = last - now;
        last = now;
        return dt;
    }
};

#endif // TIMER_H
