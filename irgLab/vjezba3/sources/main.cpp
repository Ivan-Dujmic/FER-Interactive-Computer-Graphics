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

	// Defines the portion of the window OpenGL will draw into
	// (0, 0, width, height) maps the range [0, width]x[0, height] to [-1, 1]x[-1, 1] (normalized device coordinates)
	glViewport(0, 0, width, height);
}
  
int main(int argc, char * argv[]) {
	std::cout << argv[0] << std::endl;
	/*********************************************************************************************/
	// Setting OpenGL context, fetching available OpenGL commands

	glfwInit(); // Initialize GLFW before we can use GLFW functions

	// Configure the window before creating it
	// We could leave these hints out and defaults would be used
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Creates a window with all the previously set hints (context) applied
	GLFWwindow* window = glfwCreateWindow(width, height, "Task 3", nullptr, nullptr);
	if (window == nullptr) {
		fprintf(stderr, "Failed to Create OpenGL Context");
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window); // All the following OpenGL calls refer to this context

	// OpenGL function pointers have to be loaded at runtime
	// This has to be called after some context is created and set as current
	// Could also use gladLoadGL()
	// Glad uses glfwGetProcAddress to query function addresses from the current context
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		fprintf(stderr, "Failed to initialize GLAD");
		exit(-1);
	}
	fprintf(stderr, "OpenGL %s\n", glGetString(GL_VERSION));

	glEnable(GL_DEPTH_TEST); // Track Z-index for every pixel
	glDepthFunc(GL_LESS); // Set comparison function for depth testing (if it's less than the current depth in buffer)

	glEnable(GL_CULL_FACE); // Remove obstructed polygons (triangles facing a certain direction)
	glCullFace(GL_BACK); // Which face should be culled (back is also the default)

	glClearColor(0.15, 0.1, 0.1, 1); // Sets the color for the clear screen command

	 // 0 means don't wait after drawing (no vsync)
	 // 1 would sync to monitor refresh rate
	 // 2 would be half the monitor refresh rate, but values above 1 are ignored on many systems
	glfwSwapInterval(0);

	FPSManager fpsManager(window, 60, 1.0, "Task 3");

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
	// It references the vertices in indexedVertices
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
	GLuint VAO; // Vertex array object (how vertex data is interpreted)
	GLuint VBO[2]; // Vertex buffer object (vertex data - positions and colors)
	GLuint EBO; // Element buffer object (uses VBO by index)

	// Generate IDs
	glGenVertexArrays(1, &VAO);
	glGenBuffers(2, VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO); // The following setup gets store inside this VAO
		// Set VBO[0] as current array buffer
		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		// Allocate GPU memory & copy indexedVertices into it
		glBufferData(GL_ARRAY_BUFFER, sizeof(indexedVertices), indexedVertices, GL_STATIC_DRAW);
		// Zeroth position, vec3, type float, don't normalize, stride of 3 floats, no offset
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		
		// Set VBO[1] as current array buffer
		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		// Allocate GPU memory & copy indexedColors into it
		glBufferData(GL_ARRAY_BUFFER, sizeof(indexedColors), indexedColors, GL_STATIC_DRAW);
		// First position, vec3, type float, don't normalize, stride of 3 floats, no offset
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

		// Enable the set attributes
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		// Index buffer - only one GL_ELEMENT_ARRAY_BUFFER per VAO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glBindVertexArray(0); // We are no longer modifying this VAO

	/*********************************************************************************************/
	// Each matrix represents a model transform for one instance
	// Create a transformation field that places the objects in a 4x4 grid

	glm::mat4 identityMatrix = glm::mat4(1);
	glm::mat4 scalingMatrix = glm::scale(identityMatrix, glm::vec3(0.25, 0.25, 0.25)); // Downscale x4

	glm::mat4 transformationField[32]; // 32 rotation steps
	
	glm::vec3 rotationAxis = glm::vec3(1, 0, 0); // Rotating around the x axis
	float rotationAngle = M_PI / 32;

	int counter = 0;
	// Place into 16 locations non-overlapping positions on screen
	// The first one is identical for each
	// The second one has a different rotation for each (+ z flipped)
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
		float deltaTime = (float)fpsManager.enforceFPS(false); // Time since last frame (make animation framerate independent)

		/****************************/
		// Refresh data
		transformationField[28] = transformationField[28]* glm::rotate(identityMatrix, deltaTime * (float)M_PI / 4, glm::vec3(0, 0, 1));

		// Clear color buffer (wipe image) and clear depth buffer (z-index)
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/****************************/
		// Draw
		// For each object instance we send a draw command
		// Model data stays on the GPU, we only change the uniform variable
		glUseProgram(shader->ID); // Use this shader program for upcoming draw calls (shader.frag + shader.vert)

		glBindVertexArray(VAO); // Restores the earlier attribute setup
			for (int i = 0 ; i < 32 ; i++) {
				// Location of tMatrix, one matrix, don't transpose, pointer to matrix data
				glUniformMatrix4fv(locUniformVar, 1, GL_FALSE, &transformationField[i][0][0]);
				// Interpret indices as triangles, index count, index type, EBO offset
				glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);  
			}
		glBindVertexArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents(); // Handle keyboard, mouse, window events...

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) { // ESC key closes window
			glfwSetWindowShouldClose(window, true);
		}
	}   
		
	// Cleanup
	delete shader;
	glDeleteBuffers(2, VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &VAO);

	glfwTerminate();

    return EXIT_SUCCESS;
}
