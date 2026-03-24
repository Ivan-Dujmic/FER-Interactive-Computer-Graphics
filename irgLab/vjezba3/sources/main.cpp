#include "Shader.h"
#include "FPSManager.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cstdio>
#include <cstdlib>

#include <iostream>

int width = 500;
int height = 500;

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

void framebuffer_size_callback(GLFWwindow *window, int Width, int Height) {
	width = Width;
	height = Height;

	glViewport(0, 0, width, height);
}
  
int main(int argc, char * argv[]) {
	std::cout << argv[0] << std::endl;
	/*********************************************************************************************/
	// Setting OpenGL context, fetching available OpenGL commands
	GLFWwindow* window;

	glfwInit();
	gladLoadGL();

	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);

	window = glfwCreateWindow(width, height, "Task X", nullptr, nullptr);
	if (window == nullptr) {
		fprintf(stderr, "Failed to Create OpenGL Context");
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);

	// Fetch all available OpenGL functions
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		fprintf(stderr, "Failed to initialize GLAD");
		exit(-1);
	}
	fprintf(stderr, "OpenGL %s\n", glGetString(GL_VERSION));

	glEnable(GL_DEPTH_TEST); // Z buffer
	glDepthFunc(GL_LESS);

	//glEnable(GL_CULL_FACE); // Remove obstructed polygons
	//glCullFace(GL_BACK); 

	glClearColor(0.15, 0.1, 0.1, 1);

	glfwSwapInterval(0); // Don't wait after drawing (vsync)

	FPSManager fpsManager(window, 60, 1.0, "Task X");

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	
	/*********************************************************************************************/
	// Indexed vertices and colors in separate arrays. Concrete data

	// Each row is a single coordinate (x, y, z)
	float indexedVertices[6 * 3] = {
		-1, -1, 0,
		1, -1, 0,
		0, 1, 0,
		-0.4, -0.1, 0,
		0.4, -0.1, 0,
		0, -0.9, 0
	};

	// Each row is a single color (r, g, b) related to the vertex at the same position in the last array
	float indexedColors[6 * 3] = {
		1, 0, 0,
		0, 1, 0,
		0, 0, 1,
		0, 1, 1,
		1, 0, 1,
		1, 1, 0
	};

	// Each row is a single triangle
	unsigned int indices[4 * 3] = {
		0, 5, 3,
		3, 5, 4,
		5, 1, 4,
		3, 4, 2
	};

	// Load shader and fetch uniform variables
	Shader *shader = loadShader(argv[0], "shader");
	GLint locUniformVar = glGetUniformLocation(shader->ID, "tMatrix");

	/*********************************************************************************************/
	// Data transfer and data format
	// Buffer generation
	GLuint VAO;
	GLuint VBO[2];
	GLuint EBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(2, VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
		// Coordinates buffer and connect with the zeroth position in shader -- layout (location = 0)
		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(indexedVertices), indexedVertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		
		// Colors buffer and connect with first position in shader -- layout (location = 1)
		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(indexedColors), indexedColors, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		// Index buffer - only one GL_ELEMENT_ARRAY_BUFFER per VAO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), (void*)(&indices[0]), GL_STATIC_DRAW);
	glBindVertexArray(0);

	/*********************************************************************************************/
	// Each matrix represents a model transform for one instance
	// Create a transformation field that places the objects in a 4x4 grid

	glm::mat4 identityMatrix = glm::mat4(1);
	glm::mat4 scalingMatrix = glm::scale(identityMatrix, glm::vec3(0.25, 0.25, 0.25));

	glm::mat4 transformationField[32];
	
	glm::vec3 rotationAxis = glm::vec3(1, 0, 0);
	float rotationAngle = M_PI / 32;

	int counter = 0;
	for (float i = -1 ; i < 1 ; i += 0.5) {
		for (float j = -1 ; j < 1 ; j += 0.5, counter++) {
			transformationField[counter] = glm::translate(identityMatrix, glm::vec3(j + 0.25, i + 0.25 , 0)) * scalingMatrix;

			transformationField[counter + 16] = glm::translate(identityMatrix, glm::vec3(j + 0.25, i + 0.25, 0)) *
												glm::rotate(identityMatrix, counter * rotationAngle, rotationAxis) * 
												glm::rotate(identityMatrix, (float)M_PI, glm::vec3(0, 0, 1)) *
												scalingMatrix;
		}
	}

	/*********************************************************************************************/
	// Main display loop
	while (glfwWindowShouldClose(window) == false) {
		float deltaTime = (float)fpsManager.enforceFPS(false);

		/****************************/
		// Refresh data
		transformationField[28] = transformationField[28]* glm::rotate(identityMatrix, deltaTime * (float)M_PI / 4, glm::vec3(0, 0, 1));

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/****************************/
		// Draw
		// For each object instance we send a draw command
		// Model data stays on the GPU, we only change the uniform variable
		glUseProgram(shader->ID);

		glBindVertexArray(VAO);
			for (int i = 0 ; i < 32 ; i++) {
				glUniformMatrix4fv(locUniformVar, 1, GL_FALSE, &transformationField[i][0][0]);
				glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);  
			}
		glBindVertexArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, true);
		}
	}   
		
	delete shader;
	glDeleteBuffers(2, VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &VAO);

	glfwTerminate();

    return EXIT_SUCCESS;
}
