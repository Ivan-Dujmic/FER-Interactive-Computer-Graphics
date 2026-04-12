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

// argv[0] = program path ; argv[1] = object to load
int main(int argc, char *argv[]) {
	if (argc < 2) {
		std::cerr << "Usage: ./vjezba4 <.obj name>\n";
		return EXIT_FAILURE;
	}
	WindowState windowState(WIDTH, HEIGHT);
	Graphics graphics(windowState);
	Renderer renderer;
	FPSManager fpsManager(graphics.getWindow(), 60, 1.0, "Window");
	ResourceManager &resources = ResourceManager::getInstance();
	std::shared_ptr<Shader> shader = resources.getShader("wireframe");
	std::vector<std::shared_ptr<Renderable>> scene = resources.getScene(argv[1]);
	std::shared_ptr<Object> object = std::make_shared<Object>(shader, scene);
	object->normalize();
	renderer.addObject(object);

	while (!graphics.shouldClose()) {
		renderer.render();
	}

    return EXIT_SUCCESS;
}
