// TODO: remove unnecessary includes
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>      
#include <assimp/scene.h>           
#include <assimp/postprocess.h> 
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>
#include "WindowState.h"
#include "Graphics.h"
#include "Renderer.h"
#include "FPSManager.h"
#include "Shader.h"
#include "Renderable.h"
#include "ResourceManager.h"

#define WIDTH 1000
#define HEIGHT 1000
#define SHADER "wireframe"
#define CLEAR_COLOR glm::vec3(0.0f)
#define SPEED 0.1f
#define SENSETIVITY 0.1

// argv[0] = program path ; argv[1] = object to load
int main(int argc, char *argv[]) {
	if (argc < 3) {
		std::cerr << "Usage: ./vjezba5 <.obj name> <.obj name>\n";
		return EXIT_FAILURE;
	}
	WindowState windowState(WIDTH, HEIGHT);
	Graphics graphics(windowState, CLEAR_COLOR);
	FPSManager fpsManager(graphics.getWindow(), 60, 1.0, "Window");
	std::shared_ptr<Camera> camera = std::make_shared<Camera>();
	Renderer renderer(camera);
	ResourceManager &resources = ResourceManager::getInstance();
	std::shared_ptr<Shader> shader = resources.getShader(SHADER);
	
	std::vector<std::shared_ptr<Renderable>> scene1 = resources.getScene(argv[1]);
	std::shared_ptr<Object> object1 = std::make_shared<Object>(shader, scene1);
	object1->normalize();
	object1->globalMove(glm::vec3(-1.5f, 0.0f, 0.0f));
	renderer.addObject(object1);

	camera->setPosition(glm::vec3(3.0f, 4.0f, 1.0f));
	camera->setOrientation(object1->getPosition(), glm::vec3(0.0f, 1.0f, 0.0f));
	
	std::function<void(int)> myKeyCallback =
	[camera](int action) {
		switch (action) {
			case GLFW_KEY_W:
				camera->localMove(glm::vec3(0.0f, 0.0f, SPEED));
				break;

			case GLFW_KEY_A:
				camera->localMove(glm::vec3(SPEED, 0.0f, 0.0f));
				break;

			case GLFW_KEY_S:
				camera->localMove(glm::vec3(0.0f, 0.0f, -SPEED));
				break;

			case GLFW_KEY_D:
				camera->localMove(glm::vec3(-SPEED, 0.0f, 0.0f));
				break;

			case GLFW_KEY_Q:
				camera->localMove(glm::vec3(0.0f, -SPEED, 0.0f));
				break;

			case GLFW_KEY_E:
				camera->localMove(glm::vec3(0.0f, SPEED, 0.0f));
				break;
		}	
	}; graphics.setMyKeyCallback(myKeyCallback);

	std::function<void(glm::vec2, glm::vec2)> myCursorPosCallback =
	[camera](glm::vec2 prev, glm::vec2 curr) {
		float deltaX = curr.x - prev.x;
		float deltaY = curr.y - prev.y;
		camera->rotateFPS(SENSETIVITY * deltaX, SENSETIVITY * deltaY);
	}; graphics.setMyCursorPosCallback(myCursorPosCallback);

	while (!graphics.shouldClose()) {
		graphics.frameBegin();
		renderer.render();
		graphics.frameEnd();
	}

    return EXIT_SUCCESS;
}