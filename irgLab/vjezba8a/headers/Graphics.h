#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "WindowState.h"
#include <functional>

class Graphics {
private:
	static GLFWwindow *window;
    WindowState windowState;
	glm::vec3 clearColor;
	bool keys[1024];

	std::function<void(glm::vec2, glm::vec2)> myCursorPosCallback; // prevCursorPosition, cursorPosition

    static void framebufferSizeCallback(GLFWwindow *window, int width, int height);
	static void cursorPosCallback(GLFWwindow *window, double xpos, double ypos);
	static void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
    static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
	void loadGlfw();

public:
	Graphics(WindowState windowState, glm::vec3 clearColor = glm::vec3(0.0f)); 
	~Graphics();

    static GLFWwindow* getWindow();
    glm::vec3 getClearColor() const;
    const WindowState& getWindowState() const;
    void setClearColor(const glm::vec3 c);
	const bool* getKeys() const;
	void setMyCursorPosCallback(const std::function<void(glm::vec2, glm::vec2)>& func);

	bool shouldClose();
	void frameBegin();
	void frameEnd();
};
