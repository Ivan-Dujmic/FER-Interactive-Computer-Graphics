#include "Shader.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <filesystem>

void Shader::checkCompilerErrors(unsigned int shader, std::string type) {
	int success;
	char infolog[1024];

	if (type != "PROGRAM") {
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

		if (!success) {
			glGetShaderInfoLog(shader, 1024, nullptr, infolog);
			fprintf(stderr, "ERROR::SHADER_COMPILATION_ERROR of type: %s\n%s\n-----------------------------------------------------\n", type.c_str(), infolog);
		}
	} else {
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shader, 1024, nullptr, infolog);
			fprintf(stderr, "ERROR::PROGRAM_LINKING_ERROR of type: %s\n%s\n-------------------------------------------------------\n", type.c_str(), infolog);
		}
	}
}

Shader::Shader(const std::string &name) {
	//std::cout << vertexPath << std::endl;
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;

	// Ensure ifstream objects can throw exceptions:
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try {
		std::string pathVert = std::filesystem::current_path().string() + SHADERS_PATH + name + "/" + name + ".vert";
		std::string pathFrag = std::filesystem::current_path().string() + SHADERS_PATH + name + "/" + name + ".frag";

		// Open files
		vShaderFile.open(pathVert);
		fShaderFile.open(pathFrag);
		std::stringstream vShaderStream, fShaderStream;

		// Read file's buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();

		// Close file handlers
		vShaderFile.close();
		fShaderFile.close();

		// Convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	} catch (std::ifstream::failure e) {
		fprintf(stderr, "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ\n");
	}

	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	// 2. compile shaders
	unsigned int vertex, fragment;
	int success;
	char infoLog[512];

	// Vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER); // Create object
	glShaderSource(vertex, 1, &vShaderCode, NULL); // Pass source code to OpenGL
	glCompileShader(vertex); // Compile
	checkCompilerErrors(vertex, "VERTEX");

	// Fragment shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER); // Create object
	glShaderSource(fragment, 1, &fShaderCode, NULL); // Pass source code to OpenGL
	glCompileShader(fragment); // Compile
	checkCompilerErrors(fragment, "FRAGMENT");

	// Shader program
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	checkCompilerErrors(ID, "PROGRAM");

	// Delete the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

Shader::~Shader() {
	glDeleteProgram(ID);
}

void Shader::use() {
	glUseProgram(ID); // Makes this shader program active for the subsequent drawing calls
}

void Shader::setUniform(const std::string &name, bool value) const {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setUniform(const std::string &name, int value) const {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setUniform(const std::string &name, float value) const {
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setUniform(const std::string &name, const glm::vec3 &vec) const {
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(vec));
}

void Shader::setUniform(const std::string &name, const glm::vec4 &mat) const {
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}