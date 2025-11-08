#include "gl_buffer.h"
#include "gui.h"
#include "shader.h"
#include "timer.h"
#include "vertex.h"
#include "window.h"
#include <cstddef>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct CameraContext {
    glm::vec3 pos;
    glm::vec3 target;
    glm::vec3 up;
    float fov;
    float zNear;
    float zFar;
    double lastMouseX;
    double lastMouseY;
    bool middleMousePressed;
};

// helper function to draw imgui text
void drawText(const std::string& text, ImVec2 pos)
{
    ImDrawList* dl = ImGui::GetForegroundDrawList();
    dl->AddText(ImGui::GetFont(), ImGui::GetFontSize(), pos, IM_COL32(255, 255, 255, 255), text.c_str());
}

// helper function to draw a 4x4 matrix
void drawMat4(const glm::mat4& m, ImVec2 startPos)
{
    ImDrawList* dl = ImGui::GetForegroundDrawList();

    for (int row = 0; row < 4; row++) {
        char buf[128];
        snprintf(buf, sizeof(buf), "[ %8.3f %8.3f %8.3f %8.3f ]",
            m[0][row], m[1][row], m[2][row], m[3][row]);
        ImVec2 pos(startPos.x, startPos.y + row * (ImGui::GetFontSize()));
        drawText(buf, pos);
    }
}

int main()
{
    // initialize window
    Window window;
    if (!window.init("LearnOpenGL")) {
        return EXIT_FAILURE;
    }

    // set window resize callback
    glfwSetWindowUserPointer(window.get(), &window);
    glfwSetWindowSizeCallback(window.get(), Window::onResize);

    // initialize shader
    Shader shader;
    if (!shader.init("shaders/vertex.glsl", "shaders/fragment.glsl")) {
        return EXIT_FAILURE;
    }

    // initialize gui
    Gui gui;
    if (!gui.init(window.get(), "#version 330 core")) {
        return EXIT_FAILURE;
    }

    // create timer
    Timer timer;
    timer.reset();

    // --- Grid Begin ---
    std::vector<Vertex> gridVertices;
    std::vector<GLuint> gridIndices;

    const int gridSize = 20;
    const float gridSpacing = 20.0f;
    glm::vec4 gridColor(0.7f, 0.7f, 0.7f, 1.0f);

    // generate vertices for grid lines on XZ plane at y=0
    for (int i = -gridSize; i <= gridSize; ++i) {
        // lines parallel to Z-axis
        gridVertices.push_back(Vertex(glm::vec3(i * gridSpacing, 0.0f, -gridSize * gridSpacing), gridColor));
        gridVertices.push_back(Vertex(glm::vec3(i * gridSpacing, 0.0f, gridSize * gridSpacing), gridColor));

        // lines parallel to X-axis
        gridVertices.push_back(Vertex(glm::vec3(-gridSize * gridSpacing, 0.0f, i * gridSpacing), gridColor));
        gridVertices.push_back(Vertex(glm::vec3(gridSize * gridSpacing, 0.0f, i * gridSpacing), gridColor));
    }

    // indices are sequential since each pair of vertices form a line
    for (GLuint i = 0; i < gridVertices.size(); i += 2) {
        gridIndices.push_back(i);
        gridIndices.push_back(i + 1);
    }

    // generate grid buffer objects
    Vao gridVao;
    Vbo gridVbo;
    Ebo gridEbo;

    // bind grid buffer objects
    gridVao.bind();
    gridVbo.bind();
    gridEbo.bind();

    // fill grid vertex buffer object and element buffer object with data
    gridVbo.fill(gridVertices.size() * sizeof(Vertex), gridVertices.data(), GL_STATIC_DRAW);
    gridEbo.fill(gridIndices.size() * sizeof(GLuint), gridIndices.data(), GL_STATIC_DRAW);

    // configure grid vertex attaributes
    gridVao.attrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    gridVao.attrib(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, color));

    // unbind grid buffer objects
    gridVao.unbind();
    gridVbo.unbind();
    gridEbo.unbind();

    // pyramid transform variables
    glm::vec3 gridTranslate = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 gridRotation = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 gridScale = glm::vec3(1.0f, 1.0f, 1.0f);

    // --- Grid End ---

    // --- Pyramid Begin ---

    // pyramid vertices data
    // clang-format off
    std::vector<Vertex> pyVertices = {
        // position                            // color
        Vertex(glm::vec3(+000.0f, +180.0f, +000.0f), glm::vec4(1, 0, 0, 1)), // apex
        Vertex(glm::vec3(-100.0f, -100.0f, -100.0f), glm::vec4(0, 1, 0, 1)), // base bottom-left
        Vertex(glm::vec3(+100.0f, -100.0f, -100.0f), glm::vec4(0, 0, 1, 1)), // base bottom-right
        Vertex(glm::vec3(+100.0f, -100.0f, +100.0f), glm::vec4(1, 1, 0, 1)), // base top-right
        Vertex(glm::vec3(-100.0f, -100.0f, +100.0f), glm::vec4(0, 1, 1, 1)), // base top-left
    };

    // pyramid indices data
    std::vector<GLuint> pyIndices = {
        0, 1, 2,  // front
        0, 2, 3,  // right
        0, 3, 4,  // back
        0, 4, 1,  // left

        // base
        1, 2, 3,
        1, 3, 4
    };
    // clang-format on

    // generate buffer objects
    Vao pyVao;
    Vbo pyVbo;
    Ebo pyEbo;

    // bind pyramid buffer objects
    pyVao.bind();
    pyVbo.bind();
    pyEbo.bind();

    // fill pyramid vertex buffer object
    pyVbo.fill(pyVertices.size() * sizeof(Vertex), pyVertices.data(), GL_DYNAMIC_DRAW);

    // fill pyramid element buffer object
    pyEbo.fill(pyIndices.size() * sizeof(GLuint), pyIndices.data(), GL_DYNAMIC_DRAW);

    // configure pyramid vertex attaributes
    pyVao.attrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    pyVao.attrib(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, color));

    // unbind pyramid buffer objects
    pyVao.unbind();
    pyVbo.unbind();
    pyEbo.unbind();

    // pyramid transform variables
    glm::vec3 pyTranslate = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 pyRotation = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 pyScale = glm::vec3(1.0f, 1.0f, 1.0f);

    // --- Pyramid End ---

    // camera stuff
    CameraContext camera;

    camera.pos = glm::vec3(1000.0f, 500.0f, 500.0f);
    camera.target = glm::vec3(0.0f, 0.0f, 0.0f);
    camera.up = glm::vec3(0.0f, 1.0f, 0.0f);
    camera.fov = 45.0f;
    camera.zNear = 0.1f;
    camera.zFar = 5000.0f;

    // animation controls
    bool pyAnim = false;
    bool gridAnim = false;
    bool rotateAnimX = false;
    bool rotateAnimY = false;
    bool rotateAnimZ = false;
    float animSpeed = 1.0f;

    // enable depth test
    glEnable(GL_DEPTH_TEST);

    bool initCamAnim = true;

    // main loop
    while (!window.shouldClose()) {
        // create model matrix for pyramid
        glm::mat4 modelPyramid(1.0f);
        modelPyramid = glm::translate(modelPyramid, pyTranslate);
        modelPyramid = glm::rotate(modelPyramid, glm::radians(pyRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        modelPyramid = glm::rotate(modelPyramid, glm::radians(pyRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        modelPyramid = glm::rotate(modelPyramid, glm::radians(pyRotation.z), glm::vec3(1.0f, 0.0f, 1.0f));
        modelPyramid = glm::scale(modelPyramid, pyScale);

        // create model matrix for grid
        glm::mat4 modelGrid(1.0f);
        modelGrid = glm::translate(modelGrid, gridTranslate);
        modelGrid = glm::rotate(modelGrid, glm::radians(gridRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        modelGrid = glm::rotate(modelGrid, glm::radians(gridRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        modelGrid = glm::rotate(modelGrid, glm::radians(gridRotation.z), glm::vec3(1.0f, 0.0f, 1.0f));
        modelGrid = glm::scale(modelGrid, gridScale);

        // create the view matrix every frame
        glm::mat4 view = glm::lookAt(camera.pos, camera.target, camera.up);

        // create projection matrix
        glm::mat4 projection = glm::perspective(
            glm::radians(camera.fov),
            window.m_width / (float)window.m_height,
            camera.zNear,
            camera.zFar);

        // handle window events
        window.pollEvents();

        // ____________ CAMERA MOVEMENT WITH MIDDLE MOUSE ____________
        // I still don't know how to orbit the camera around a target, for now this is enough for me
        // get current mouse position
        double xpos, ypos;
        glfwGetCursorPos(window.m_handle, &xpos, &ypos);

        // get middle mouse button state
        int middleState = glfwGetMouseButton(window.m_handle, GLFW_MOUSE_BUTTON_MIDDLE);

        if (middleState == GLFW_PRESS) {
            if (!camera.middleMousePressed) {
                camera.lastMouseX = xpos;
                camera.lastMouseY = ypos;
                camera.middleMousePressed = true;
            } else {
                // calculate mouse delta
                double deltaX = xpos - camera.lastMouseX;
                double deltaY = ypos - camera.lastMouseY;

                // camera movement sensitivity
                float sensitivity = 5.0f;

                camera.pos.x += static_cast<float>(-deltaX * sensitivity);
                camera.pos.y += static_cast<float>(deltaY * sensitivity);

                // update camera last mouse pos
                camera.lastMouseX = xpos;
                camera.lastMouseY = ypos;
            }
        } else {
            camera.middleMousePressed = false;
        }

        // clear color buffer and depth buffer every frame before rendering
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // update pyramid vertices
        pyVbo.bind();
        pyVbo.fill(pyVertices.size() * sizeof(Vertex), pyVertices.data(), GL_DYNAMIC_DRAW);
        pyVbo.unbind();

        // use shader program
        glUseProgram(shader.program);

        // uMVP
        GLint mvpLoc = glGetUniformLocation(shader.program, "uMVP");

        // uTime
        GLint timeLoc = glGetUniformLocation(shader.program, "uTime");
        glUniform1f(timeLoc, glfwGetTime());

        // uObjectID
        GLint objectIDLoc = glGetUniformLocation(shader.program, "uObjectID");

        // animation booleans
        GLint pyAnimLoc = glGetUniformLocation(shader.program, "uPyAnim");

        GLint gridAnimLoc = glGetUniformLocation(shader.program, "uGridAnim");

        // uAnimSpeed
        GLint animSpeedLoc = glGetUniformLocation(shader.program, "uAnimSpeed");
        glUniform1f(animSpeedLoc, animSpeed);

        // rotation toggles
        GLint rotateAnimXLoc = glGetUniformLocation(shader.program, "uRotateAnimX");
        GLint rotateAnimYLoc = glGetUniformLocation(shader.program, "uRotateAnimY");
        GLint rotateAnimZLoc = glGetUniformLocation(shader.program, "uRotateAnimZ");

        // update the anim control variables in the vertex shader
        glUniform1i(rotateAnimXLoc, rotateAnimX);
        glUniform1i(rotateAnimYLoc, rotateAnimY);
        glUniform1i(rotateAnimZLoc, rotateAnimZ);

        // ____________ PYRAMID ____________
        glUniform1i(objectIDLoc, 0);
        glUniform1i(pyAnimLoc, pyAnim);

        // compute mvp for pyramid
        // upload the pyramid mvp matrix before rendering
        glm::mat4 pyMVP = projection * view * modelPyramid;
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(pyMVP));

        // render pyramid
        pyVao.bind();
        pyEbo.bind();
        glDrawElements(GL_TRIANGLES, pyIndices.size(), GL_UNSIGNED_INT, 0);
        pyVao.unbind();
        pyEbo.unbind();

        // disable pyramid animation
        glUniform1i(pyAnimLoc, false);

        // ____________ GRID ____________
        glUniform1i(objectIDLoc, 1);
        glUniform1i(gridAnimLoc, gridAnim);

        // compute mvp for grid
        // upload the grid mvp matrix before rendering
        glm::mat4 gridMVP = projection * view * modelGrid;
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(gridMVP));

        // render grid
        gridVao.bind();
        gridEbo.bind();
        glDrawElements(GL_LINES, gridIndices.size(), GL_UNSIGNED_INT, 0);
        gridVao.unbind();
        gridEbo.unbind();

        // disable grid animation
        glUniform1i(gridAnimLoc, false);

        // if you are reading this code, then this part is optional you can set the initCamAnim to
        // false or just remove this while loop
        if (initCamAnim) {
            float targetX = 500.0f;
            float lerpFactor = 0.08f;

            camera.pos.x = glm::mix(camera.pos.x, targetX, lerpFactor);

            if (fabs(camera.pos.x - targetX) < 1.f) {
                camera.pos.x = targetX;
                initCamAnim = false;
            }
        }

        // create new gui frame
        gui.newFrame();

        // draw gui
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;

        // pyramid transform controls
        if (ImGui::CollapsingHeader("Pyramid Transform", flags)) {
            ImGui::DragFloat3("Translate##Py", glm::value_ptr(pyTranslate), 1.0f);
            ImGui::DragFloat3("Rotation##Py", glm::value_ptr(pyRotation), 1.0f);
            ImGui::DragFloat3("Scale##Py", glm::value_ptr(pyScale), 0.1f);
        }

        // grid transform controls
        if (ImGui::CollapsingHeader("Grid Transform", flags)) {
            ImGui::DragFloat3("Translate##Grid", glm::value_ptr(gridTranslate), 1.0f);
            ImGui::DragFloat3("Rotation##Grid", glm::value_ptr(gridRotation), 1.0f);
            ImGui::DragFloat3("Scale##Grid", glm::value_ptr(gridScale), 0.1f);
        }

        // camera controls
        if (ImGui::CollapsingHeader("Camera", flags)) {
            ImGui::DragFloat3("Camera pos", glm::value_ptr(camera.pos), 1.0f);
            ImGui::DragFloat3("Camera target", glm::value_ptr(camera.target), 1.0f);
            ImGui::DragFloat3("up", glm::value_ptr(camera.up), 1.0f);
            ImGui::SliderFloat("FOV", &camera.fov, 10.0f, 120.0f);
            ImGui::SliderFloat("Z-Near", &camera.zNear, 0.1f, 1.0f);
            ImGui::SliderFloat("z-Far", &camera.zFar, 1.0f, 10000.0f);
        }

        // animation control
        if (ImGui::CollapsingHeader("Animation", flags)) {
            ImGui::Checkbox("Pyramid Animation", &pyAnim);
            ImGui::Checkbox("Grid Animation", &gridAnim);
            ImGui::Checkbox("Rotation X", &rotateAnimX);
            ImGui::Checkbox("Rotation Y", &rotateAnimY);
            ImGui::Checkbox("Rotation Z", &rotateAnimZ);
            ImGui::SliderFloat("Animation Speed", &animSpeed, 1.0f, 20.0f);
        }

        // draw model matrices
        drawText("Pyramid - Model Matrix:", ImVec2(10, 0));
        drawMat4(modelPyramid, ImVec2(10, 30));

        drawText("Grid - Model Matrix:", ImVec2(10, 100));
        drawMat4(modelGrid, ImVec2(10, 130));

        // render gui
        gui.render();

        // display
        window.swapBuffers();
    } // main loop

    // clear resources
    gui.shutdown();
    window.shutdown();

    // exit program
    return EXIT_SUCCESS;
}
