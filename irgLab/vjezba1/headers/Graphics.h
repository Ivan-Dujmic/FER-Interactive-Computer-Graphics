#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <string>
#include <vector>
#include <cstdint>

#include <fstream>
#include <sstream>

#include "Shader.h"

class Graphics {
private:
	Shader* loadRasterShader(char *path);
	int width;
	int height;
	glm::vec3 clearColor;
	static GLFWwindow *window;
	float rasterVertices[20] = { 
	//  verticesCoord	textureCoord
		-1, -1, 0,		0, 0,
		 1, -1, 0,		1, 0,
		-1,  1, 0,		0, 1,
		 1,  1, 0,		1, 1
	};
	float *raster;
	unsigned int rasterID;
	unsigned int VAO;
	unsigned int VBO;
	Shader *rasterShader;
	static void(*mouse_callback_user)(int, int, int);

	static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
	static void cursor_position_callback(GLFWwindow *window, double xpos, double ypos);
	void loadGlfw();

public:
	static glm::vec2 cursorPosition;

	Graphics(int width, int height, glm::vec3 clearColor, char *path); 
	~Graphics();

	int lightFragment(int x, int y, glm::vec3 boja);
	int lightFragment(int x, int y);
	void clearWindow();
	void drawRaster();
	bool shouldClose();
	static int registerMouseClickFunction(void(*mouse_callback_user)(int, int, int));
};
