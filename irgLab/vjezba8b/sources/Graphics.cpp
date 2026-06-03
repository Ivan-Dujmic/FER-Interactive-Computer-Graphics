#include "Graphics.h"

#include <cmath>
#include <iostream>
#include <string>

GLFWwindow *Graphics::window = nullptr;

void Graphics::framebufferSizeCallback(GLFWwindow *window, int width, int height) {
    auto* graphics = static_cast<Graphics*>(glfwGetWindowUserPointer(window));
    if (!graphics) return;

    graphics->windowState.setWindowWidth(width);
    graphics->windowState.setWindowHeight(height);
    glViewport(0, 0, width, height);
}

void Graphics::cursorPosCallback(GLFWwindow *window, double xpos, double ypos) {
    auto* graphics = static_cast<Graphics*>(glfwGetWindowUserPointer(window));
    if (!graphics) return;
    if (!graphics->myCursorPosCallback) return;

    graphics->windowState.setCursorPosition(glm::vec2(static_cast<float>(xpos), static_cast<float>(ypos)));
    graphics->myCursorPosCallback(graphics->windowState.getPrevCursorPosition(), graphics->windowState.getCursorPosition());
}

void Graphics::mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
    if (action != GLFW_PRESS) return;

    auto* graphics = static_cast<Graphics*>(glfwGetWindowUserPointer(window));
    if (!graphics) return;

    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        // PASS
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        // PASS
    }
}

void Graphics::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    auto *graphics = static_cast<Graphics*>(glfwGetWindowUserPointer(window));
    if (!graphics) return;
    if (key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(window, true);
        return;
    }

    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            graphics->keys[key] = true;
        } else if (action == GLFW_RELEASE) {
            graphics->keys[key] = false;
        }
    }
}

void Graphics::loadGlfw() {
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	window = glfwCreateWindow(windowState.getWindowWidth(), windowState.getWindowHeight(), "Window", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create window\n";
        exit(EXIT_FAILURE);
    }

	glfwMakeContextCurrent(window);
    glfwSetWindowUserPointer(window, this);
    glfwSwapInterval(0);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetKeyCallback(window, keyCallback);
}

Graphics::Graphics(WindowState windowState, glm::vec3 clearColor) :
    keys{}, // Fill with false
    windowState(std::move(windowState)),
    clearColor(clearColor)
{
    loadGlfw();
    gladLoadGL();
    setClearColor(clearColor);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CW);
    glDepthFunc(GL_LESS);
    glViewport(0, 0, windowState.getWindowWidth(), windowState.getWindowHeight());
}

Graphics::~Graphics() {
	glfwTerminate();
}

GLFWwindow* Graphics::getWindow() {
    return window;
}

glm::vec3 Graphics::getClearColor() const {
    return clearColor;
}

const WindowState& Graphics::getWindowState() const {
    return windowState;
}

void Graphics::setClearColor(const glm::vec3 c) {
    clearColor = c;
    glClearColor(c.r, c.g, c.b, 1.0f);
}

const bool* Graphics::getKeys() const {
    return keys;
}

void Graphics::setMyCursorPosCallback(const std::function<void(glm::vec2, glm::vec2)>& func) {
    myCursorPosCallback = func;
}

bool Graphics::shouldClose() {
    return glfwWindowShouldClose(window);
}

void Graphics::frameBegin() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Graphics::frameEnd() {
    glfwSwapBuffers(window);
    glfwPollEvents();
}