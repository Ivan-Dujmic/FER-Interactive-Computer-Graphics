#include "Shader.h"
#include "FPSManager.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cstdio>
#include <cstdlib>

#include <iostream>

enum ColorPicker {
	RED, // 0
	GREEN, // 1
	BLUE // 2
};

std::string colorString(ColorPicker color) {
	switch (color) {
		case RED: return "RED";
		case GREEN: return "GREEN";
		case BLUE: return "BLUE";
		default: return "UNKNOWN";
	}
}

int width = 500;
int height = 500;

glm::vec2 cursorPos = {0.0, 0.0};

std::vector<glm::vec2> vertices = {
	{0.2, 0.5},
	{0.7, 0.7},
	{-0.3, -0.2},
	{0.8, -0.7}
};
std::vector<glm::vec3> colors = {
	{0, 0, 0},
	{0, 0, 0},
	{1, 0, 0},
	{0, 0, 1}
};
std::vector<GLuint> triangles = {
	0, 1, 2,
	1, 2, 3
};

glm::vec3 currentColor = {1.0, 1.0, 1.0};
ColorPicker selectedColor = RED;
std::vector<glm::vec2> palleteVertices = {
	{-0.9, 0.9},
	{-0.8, 0.9},
	{-0.9, 0.8},
	{-0.8, 0.8}
};

Shader* loadShader(char *path, char *name) {
	std::string sPath(path);
	std::string pathVert;
	std::string pathFrag;

	pathVert.append(path, sPath.find_last_of("\\/") + 1);
	pathFrag.append(path, sPath.find_last_of("\\/") + 1);
	if (pathFrag[pathFrag.size() - 1] == '/') {
		pathVert.append("shaders/");
		pathFrag.append("shaders/");
	} else if (pathFrag[pathFrag.size() - 1] == '\\') {
		pathVert.append("shaders\\");
		pathFrag.append("shaders\\");
	} else {
		std::cerr << "Unknown shader position format";
		exit(1);
	}

	pathVert.append(name);
	pathVert.append(".vert");
	pathFrag.append(name);
	pathFrag.append(".frag");

	return new Shader(pathVert.c_str(), pathFrag.c_str());
}

void framebufferSizeCallback(GLFWwindow *window, int Width, int Height) {
	width = Width;
	height = Height;

	glViewport(0, 0, width, height);
}

void mousePositionCallback(GLFWwindow *window, double x, double y) {
	cursorPos = {x, y};
}

void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
	if (action == GLFW_PRESS) {
		std::cout << "Click: " << cursorPos.x << ' ' << height - cursorPos.y << '\n';
		float x = (cursorPos.x / width) * 2.0 - 1.0;
		float y = ((height - cursorPos.y) / height) * 2.0 - 1.0;
		vertices.push_back({x, y});
		colors.push_back(currentColor);
		std::size_t n  = vertices.size();
		if (n >= 3) {
			triangles.push_back(n - 3);
			triangles.push_back(n - 2);
			triangles.push_back(n - 1);
		}
	}
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		float value;
		switch (key) {
			case GLFW_KEY_R:
				std::cout << "Changing " << colorString(RED) << " value" << std::endl;
				selectedColor = RED;
				break;

			case GLFW_KEY_G:
				std::cout << "Changing " << colorString(GREEN) << " value" << std::endl;
				selectedColor = GREEN;
				break;
				
			case GLFW_KEY_B:
				std::cout << "Changing " << colorString(BLUE) << " value" << std::endl;
				selectedColor = BLUE;
				break;
				
			case GLFW_KEY_A:
				value = currentColor[selectedColor];
				currentColor[selectedColor] = (value < 1.0) ? value + 0.1 : 0.0;
				std::cout << colorString(selectedColor) << " value is now " << currentColor[selectedColor] << std::endl;
				break;
				
			case GLFW_KEY_S:
				value = currentColor[selectedColor];
				currentColor[selectedColor] = (value > 0.0) ? value - 0.1 : 1.0;
				std::cout << selectedColor << " value is now " << currentColor[selectedColor] << std::endl;
				break;
		}
	}
}
  
int main(int argc, char * argv[]) {
	std::cout << argv[0] << std::endl;

	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(width, height, "Task 3", nullptr, nullptr);
	if (window == nullptr) {
		fprintf(stderr, "Failed to Create OpenGL Context");
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		fprintf(stderr, "Failed to initialize GLAD");
		exit(-1);
	}
	fprintf(stderr, "OpenGL %s\n", glGetString(GL_VERSION));

	glClearColor(0.0, 0.0, 0.0, 1);

	glfwSwapInterval(1);

	FPSManager fpsManager(window, 60, 1.0, "Task 3");

	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetCursorPosCallback(window, mousePositionCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetKeyCallback(window, keyCallback);

	Shader *squareShader = loadShader(argv[0], "square");
	GLint squareVarLocUniColor = glGetUniformLocation(squareShader->ID, "uColor");
	GLuint VAOsquare;
	GLuint VBOsquare;
	glGenVertexArrays(1, &VAOsquare);
	glGenBuffers(1, &VBOsquare);

	glBindVertexArray(VAOsquare);
		glBindBuffer(GL_ARRAY_BUFFER, VBOsquare);
		glBufferData(GL_ARRAY_BUFFER, palleteVertices.size() * sizeof(glm::vec2), palleteVertices.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
		glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	Shader *triangleShader = loadShader(argv[0], "triangle");
	GLuint VAOtriangle;
	GLuint VBOtrianglePos;
	GLuint VBOtriangleColor;
	GLuint EBOtriangle;
	glGenVertexArrays(1, &VAOtriangle);
	glGenBuffers(1, &VBOtrianglePos);
	glGenBuffers(1, &VBOtriangleColor);
	glGenBuffers(1, &EBOtriangle);

	glBindVertexArray(VAOtriangle);
		glBindBuffer(GL_ARRAY_BUFFER, VBOtrianglePos);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec2), vertices.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, VBOtriangleColor);
		glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), colors.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOtriangle);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangles.size() * sizeof(GLuint), triangles.data(), GL_STATIC_DRAW);
	glBindVertexArray(0);

	while (glfwWindowShouldClose(window) == false) {
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(squareShader->ID);
			glUniform3f(squareVarLocUniColor, currentColor.r, currentColor.g, currentColor.b);

			glBindVertexArray(VAOsquare);
				glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			glBindVertexArray(0);

		if (vertices.size() >= 3) {
			glUseProgram(triangleShader->ID);
				glBindVertexArray(VAOtriangle);
					glDrawElements(GL_TRIANGLES, triangles.size(), GL_UNSIGNED_INT, 0);
				glBindVertexArray(0);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, true);
		}
	}

	glfwTerminate();

    return EXIT_SUCCESS;
}
