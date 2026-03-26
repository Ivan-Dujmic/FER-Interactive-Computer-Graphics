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
	RED = 0,
	GREEN = 1,
	BLUE = 2
};

std::string colorString(ColorPicker color) {
	switch (color) {
		case RED: 
			return "RED";
		case GREEN:
			return "GREEN";
		case BLUE:
			return "BLUE";
		default:
			return "UNKNOWN";
	}
}

class ProgramState {
public:
	int width, height;
	glm::vec2 cursorPos = {0.0f, 0.0f};

	glm::vec3 currentColor = {1.0f, 1.0f, 1.0f};
	ColorPicker colorPicker = RED;

	std::vector<glm::vec2> vertices;
	std::vector<glm::vec3> colors;
	std::vector<GLuint> triangles;
	
	bool isDataDirty = false;

	bool stripDrawMode = false;

	ProgramState(int width, int height) :
		width(width),
		height(height)
		{}

	void addVertex() {
		float x = (cursorPos.x / width) * 2.0f - 1.0f;
		float y = ((height - cursorPos.y) / height) * 2.0f - 1.0f;

		std::cout << "Placing vertex: " << x << ' ' << y << '\n';

		vertices.push_back({x, y});
		colors.push_back(currentColor);

		std::size_t n  = vertices.size();
		if (n >= 3) {
			triangles.push_back(n - 3);
			triangles.push_back(n - 2);
			triangles.push_back(n - 1);
		}

		isDataDirty = true;	
	}

	void handleKeyPress(int key) {
		float value;
		switch (key) {
			case GLFW_KEY_R:
				std::cout << "Changing " << colorString(RED) << " value" << std::endl;
				colorPicker = RED;
				break;

			case GLFW_KEY_G:
				std::cout << "Changing " << colorString(GREEN) << " value" << std::endl;
				colorPicker = GREEN;
				break;
				
			case GLFW_KEY_B:
				std::cout << "Changing " << colorString(BLUE) << " value" << std::endl;
				colorPicker = BLUE;
				break;
				
			case GLFW_KEY_A:
				value = currentColor[colorPicker];
				currentColor[colorPicker] = (value < 1.0f) ? value + 0.1f : 0.0f;
				std::cout << colorString(colorPicker) << " value is now " << currentColor[colorPicker] << std::endl;
				break;
				
			case GLFW_KEY_S:
				value = currentColor[colorPicker];
				currentColor[colorPicker] = (value > 0.0f) ? value - 0.1f : 1.0f;
				std::cout << colorPicker << " value is now " << currentColor[colorPicker] << std::endl;
				break;
		}
	}
};

class Renderer {
private:
	Shader *squareShader;
	GLuint VAOsquare, VBOsquare;
	GLint squareVarLocUniColor;
	std::vector<glm::vec2> palleteVertices = {
        {-0.9f, 0.9f},
        {-0.8f, 0.9f},
        {-0.9f, 0.8f},
        {-0.8f, 0.8f}
    };

	Shader *triangleShader;
	GLuint VAOtriangle, VBOtrianglePos, VBOtriangleColor, EBOtriangle;

	Shader *lineShader;
	GLuint VAOline, VBOline;
	
	Shader* loadShader(const char *path, const char *name) {
		std::string sPath(path);
		std::string pathVert, pathFrag;
	
		pathVert.append(path, sPath.find_last_of("\\/") + 1);
		pathFrag.append(path, sPath.find_last_of("\\/") + 1);
		if (pathFrag.back() == '/') {
			pathVert.append("shaders/");
			pathFrag.append("shaders/");
		} else if (pathFrag.back() == '\\') {
			pathVert.append("shaders\\");
			pathFrag.append("shaders\\");
		} else {
			std::cerr << "Unknown shader position format";
			exit(1);
		}
	
		pathVert.append(name).append(".vert");
		pathFrag.append(name).append(".frag");
	
		return new Shader(pathVert.c_str(), pathFrag.c_str());
	}

public:
	Renderer(const char *path) {
		squareShader = loadShader(path, "square");
		squareVarLocUniColor = glGetUniformLocation(squareShader->ID, "uColor");

		glGenVertexArrays(1, &VAOsquare);
		glGenBuffers(1, &VBOsquare);

		glBindVertexArray(VAOsquare);
			glBindBuffer(GL_ARRAY_BUFFER, VBOsquare);
			glBufferData(GL_ARRAY_BUFFER, palleteVertices.size() * sizeof(glm::vec2), palleteVertices.data(), GL_STATIC_DRAW);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
			glEnableVertexAttribArray(0);
		glBindVertexArray(0);

		triangleShader = loadShader(path, "triangle");

		glGenVertexArrays(1, &VAOtriangle);
		glGenBuffers(1, &VBOtrianglePos);
		glGenBuffers(1, &VBOtriangleColor);
		glGenBuffers(1, &EBOtriangle);

		glBindVertexArray(VAOtriangle);
			glBindBuffer(GL_ARRAY_BUFFER, VBOtrianglePos);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, VBOtriangleColor);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOtriangle);
		glBindVertexArray(0);

		lineShader = loadShader(path, "line");

		glLineWidth(5.0f);

		glGenVertexArrays(1, &VAOline);
		glGenBuffers(1, &VBOline);

		glBindVertexArray(VAOline);
			glBindBuffer(GL_ARRAY_BUFFER, VBOline);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
			glEnableVertexAttribArray(0);
		glBindVertexArray(0);
	}

	~Renderer() {
		delete squareShader;
		delete triangleShader;
		delete lineShader;
	}

	void bufferData(ProgramState &state) {
		if (!state.isDataDirty) {
			return;
		}

		glBindBuffer(GL_ARRAY_BUFFER, VBOtrianglePos);
		glBufferData(GL_ARRAY_BUFFER, state.vertices.size() * sizeof(glm::vec2), state.vertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, VBOtriangleColor);
		glBufferData(GL_ARRAY_BUFFER, state.colors.size() * sizeof(glm::vec3), state.colors.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOtriangle);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, state.triangles.size() * sizeof(GLuint), state.triangles.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, VBOline);
		glBufferData(GL_ARRAY_BUFFER, state.vertices.size() * sizeof(glm::vec2), state.vertices.data(), GL_STATIC_DRAW);

		state.isDataDirty = false;
	}

	void draw(const ProgramState &state) {
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(squareShader->ID);
			glUniform3f(squareVarLocUniColor, state.currentColor.r, state.currentColor.g, state.currentColor.b);
			glBindVertexArray(VAOsquare);
				glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			glBindVertexArray(0);

		glUseProgram(triangleShader->ID);
			glBindVertexArray(VAOtriangle);
				if (state.stripDrawMode) {
					glDrawArrays(GL_TRIANGLE_STRIP, 0, state.vertices.size());
				} else {
					glDrawElements(GL_TRIANGLES, state.triangles.size(), GL_UNSIGNED_INT, 0);
				}
			glBindVertexArray(0);

		glUseProgram(lineShader->ID);
			glBindVertexArray(VAOline);
			glDrawArrays(GL_LINE_STRIP, 0, state.vertices.size());
	}
};

void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
    auto *state = static_cast<ProgramState*>(glfwGetWindowUserPointer(window));
    if (state) {
        state->width = width;
        state->height = height;
    }
    glViewport(0, 0, width, height);
}

void mousePositionCallback(GLFWwindow *window, double x, double y) {
    auto *state = static_cast<ProgramState*>(glfwGetWindowUserPointer(window));
    if (state) {
        state->cursorPos = {static_cast<float>(x), static_cast<float>(y)};
    }
}

void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
    if (action == GLFW_PRESS) {
        auto *state = static_cast<ProgramState*>(glfwGetWindowUserPointer(window));
        if (state) {
			if (button == GLFW_MOUSE_BUTTON_LEFT) {
				state->addVertex();
			} else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
				if (state->stripDrawMode) {
					std::cout << "Changing triangle draw mode to GL_TRIANGLES" << std::endl;
				} else {
					std::cout << "Changing triangle draw mode to GL_TRIANGLE_STRIP" << std::endl;
				}
				state->stripDrawMode = !state->stripDrawMode;
			}
        }
    }
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(window, true);
        } else {
            auto *state = static_cast<ProgramState*>(glfwGetWindowUserPointer(window));
            if (state) {
                state->handleKeyPress(key);
            }
        }
    }
}

int main(int argc, char *argv[]) {
	std::cout << argv[0] << std::endl;

	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	ProgramState state(500, 500);

	GLFWwindow* window = glfwCreateWindow(state.width, state.height, "Task 3", nullptr, nullptr);
	if (!window) {
		fprintf(stderr, "Failed to Create OpenGL Context");
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		fprintf(stderr, "Failed to initialize GLAD");
		exit(-1);
	}
	fprintf(stderr, "OpenGL %s\n", glGetString(GL_VERSION));

	glfwSetWindowUserPointer(window, &state);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetCursorPosCallback(window, mousePositionCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetKeyCallback(window, keyCallback);

	glfwSwapInterval(1);
	glClearColor(0.15f, 0.15f, 0.15f, 1);

	FPSManager fpsManager(window, 60, 1.0, "Task 3");

	{
		Renderer renderer(argv[0]);

		while (!glfwWindowShouldClose(window)) {
			renderer.bufferData(state);
			renderer.draw(state);		

			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	}

	glfwTerminate();

    return EXIT_SUCCESS;
}
