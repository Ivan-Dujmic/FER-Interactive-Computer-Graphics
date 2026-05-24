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
#include "Transform.h"
#include <algorithm>
#include <array>
#include <functional>
#include <cmath>
#include <vector>
#include "Object.h"
#include "Curve.h"
#include "BezierBuilder.h"
#include "Light.h"

#define WIDTH 1000
#define HEIGHT 1000
#define SHADER1 "plane_side"
#define SHADER2 "winding"
#define SHADER3 "wireframe"
#define SHADER4 "constant"
#define CLEAR_COLOR glm::vec3(0.0f)
#define SPEED 7.5f
#define SENSETIVITY 0.1f

static std::shared_ptr<Object> makeCurveObject(
    const std::shared_ptr<Shader>& shader,
    const std::shared_ptr<Curve>& curve,
    const glm::vec3& color = glm::vec3(1.0f, 0.0f, 1.0f)
) {
    std::vector<std::shared_ptr<Renderable>> scene;
    scene.push_back(curve);
    return std::make_shared<Object>(shader, scene, color);
}

static void refreshCurves(
    const std::vector<glm::vec3>& controlPoints,
    std::shared_ptr<Curve>& controlPolygon,
    std::shared_ptr<Curve>& approximationCurve,
    std::shared_ptr<Curve>& interpolationCurve
) {
    controlPolygon->setVertices(BezierBuilder::makeControlPolygon(controlPoints));
    approximationCurve->setVertices(BezierBuilder::makeApproximationCurve(controlPoints));
    interpolationCurve->setVertices(BezierBuilder::makeInterpolationCurveFromLast4(controlPoints));
}

// argv[0] = program path ; argv[1...] = objects to load
int main(int argc, char *argv[]) {
	if (argc < 3) {
		std::cerr << "Usage: ./vjezba5 <.obj name> <.obj name>\n";
		return EXIT_FAILURE;
	}
	WindowState windowState(WIDTH, HEIGHT);
	Graphics graphics(windowState, CLEAR_COLOR);
	FPSManager fpsManager(graphics.getWindow(), 60, 1.0, "Window");
	std::shared_ptr<Camera> camera = std::make_shared<Camera>();
	std::shared_ptr<Light> light = std::make_shared<Light>(
		glm::vec3(0.0f, 3.0f, 3.0f),
		glm::vec3(0.2f),
		glm::vec3(0.9f),
		glm::vec3(1.0f)
	);
	Renderer renderer(camera, light);
	ResourceManager &resources = ResourceManager::getInstance();
	std::shared_ptr<Shader> shader1 = resources.getShader(SHADER1);
	std::shared_ptr<Shader> shader2 = resources.getShader(SHADER2);
	std::shared_ptr<Shader> shader3 = resources.getShader(SHADER3, false);
	std::shared_ptr<Shader> shader4 = resources.getShader(SHADER4);
	
	std::vector<std::shared_ptr<Renderable>> scene1 = resources.getScene(argv[1]);
	std::shared_ptr<Object> object1 = std::make_shared<Object>(shader4, scene1);
	object1->normalize();
	object1->globalMove(glm::vec3(-1.5f, 0.0f, 0.0f));
	renderer.addObject(object1);
	
	std::shared_ptr<Object> object2 = std::make_shared<Object>(shader4, scene1);
	object2->normalize();
	object2->globalMove(glm::vec3(1.5f, 0.0f, 0.0f));
	object2->setScale(glm::vec3(0.7f, 2.0f, 1.3f));
	renderer.addObject(object2);
	
	std::vector<std::shared_ptr<Renderable>> scene2 = resources.getScene(argv[2]);
	std::shared_ptr<Object> object3 = std::make_shared<Object>(shader4, scene2);
	object3->normalize();
	object3->globalMove(glm::vec3(-1.5f, 0.0f, -2.5f));
	renderer.addObject(object3);
	
	camera->setPosition(glm::vec3(3.0f, 4.0f, 1.0f));
	camera->setOrientation(object1->getPosition(), glm::vec3(0.0f, 1.0f, 0.0f));

    auto controlPolygon = std::make_shared<Curve>();
    auto approximationCurve = std::make_shared<Curve>();
    auto interpolationCurve = std::make_shared<Curve>();

    auto axisX = std::make_shared<Curve>(std::vector<glm::vec3>{glm::vec3(0.0f), glm::vec3(3.0f, 0.0f, 0.0f)});
    auto axisY = std::make_shared<Curve>(std::vector<glm::vec3>{glm::vec3(0.0f), glm::vec3(0.0f, 3.0f, 0.0f)});
    auto axisZ = std::make_shared<Curve>(std::vector<glm::vec3>{glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 3.0f)});
	
	auto lightMarkerX = std::make_shared<Curve>(std::vector<glm::vec3>{glm::vec3(-0.15f, 0.0f, 0.0f), glm::vec3(0.15f, 0.0f, 0.0f)});
	auto lightMarkerY = std::make_shared<Curve>(std::vector<glm::vec3>{glm::vec3(0.0f, -0.15f, 0.0f), glm::vec3(0.0f, 0.15f, 0.0f)});
	auto lightMarkerZ = std::make_shared<Curve>(std::vector<glm::vec3>{glm::vec3(0.0f, 0.0f, -0.15f), glm::vec3(0.0f, 0.0f, 0.15f)});

	auto lightMarkerObjectX = makeCurveObject(shader3, lightMarkerX, glm::vec3(1.0f, 0.0f, 0.0f));
	auto lightMarkerObjectY = makeCurveObject(shader3, lightMarkerY, glm::vec3(0.0f, 1.0f, 0.0f));
	auto lightMarkerObjectZ = makeCurveObject(shader3, lightMarkerZ, glm::vec3(0.0f, 0.0f, 1.0f));

	lightMarkerObjectX->setPosition(light->getPosition());
	lightMarkerObjectY->setPosition(light->getPosition());
	lightMarkerObjectZ->setPosition(light->getPosition());

    renderer.addObject(makeCurveObject(shader3, controlPolygon, glm::vec3(1.0f, 0.0f, 0.0f)));
    renderer.addObject(makeCurveObject(shader3, approximationCurve, glm::vec3(0.0f, 1.0f, 0.0f)));
    renderer.addObject(makeCurveObject(shader3, interpolationCurve, glm::vec3(0.0f, 0.5f, 1.0f)));
    renderer.addObject(makeCurveObject(shader3, axisX, glm::vec3(1.0f, 0.0f, 0.0f)));
    renderer.addObject(makeCurveObject(shader3, axisY, glm::vec3(0.0f, 1.0f, 0.0f)));
    renderer.addObject(makeCurveObject(shader3, axisZ, glm::vec3(0.0f, 0.0f, 1.0f)));
	renderer.addObject(lightMarkerObjectX);
	renderer.addObject(lightMarkerObjectY);
	renderer.addObject(lightMarkerObjectZ);

	std::function<void(glm::vec2, glm::vec2)> myCursorPosCallback =
	[camera](glm::vec2 prev, glm::vec2 curr) {
		float deltaX = curr.x - prev.x;
		float deltaY = curr.y - prev.y;
		camera->rotateFPS(SENSETIVITY * deltaX, SENSETIVITY * deltaY);
	};graphics.setMyCursorPosCallback(myCursorPosCallback);

    std::vector<glm::vec3> controlPoints;
    std::array<bool, 1024> previousKeys{};
    bool animationActive = false;
    float animationTime = 0.0f;

	std::shared_ptr<Transform> character = camera;

    std::cout << "Controls:\n"
              << "mouse - rotate camera\n"
              << "W/A/S/D/Q/E - move\n"
              << "G - add control point at camera position\n"
              << "R - clear control points\n"
              << "SPACE - toggle animation\n"
              << "0 - control camera\n"
              << "1/2/3 - control object 1/2/3\n"
			  << "4 - control light\n"
              << "\n";

	while (!graphics.shouldClose()) {
		auto deltaTime = static_cast<float>(fpsManager.enforceFPS(false));
		const bool* keys = graphics.getKeys();

		auto pressedOnce = [&](int key) {
			return keys[key] && !previousKeys[key];
		};

		if (pressedOnce(GLFW_KEY_G)) {
            controlPoints.push_back(camera->getPosition());
            refreshCurves(controlPoints, controlPolygon, approximationCurve, interpolationCurve);
            animationActive = false;
            animationTime = 0.0f;
        }

        if (pressedOnce(GLFW_KEY_R)) {
            controlPoints.clear();
            refreshCurves(controlPoints, controlPolygon, approximationCurve, interpolationCurve);
            animationActive = false;
            animationTime = 0.0f;
        }

        if (pressedOnce(GLFW_KEY_SPACE) && controlPoints.size() >= 4) {
            animationActive = !animationActive;
            animationTime = 0.0f;
        }

		if (animationActive && controlPoints.size() >= 4) {
            std::vector<glm::vec3> path = interpolationCurve->getVertices();
            if (!path.empty()) {
                animationTime += 0.25f * deltaTime;
                if (animationTime > 1.0f) animationTime -= 1.0f;

                float fIndex = animationTime * static_cast<float>(path.size() - 1);
                int i0 = static_cast<int>(std::floor(fIndex));
                int i1 = std::min(i0 + 1, static_cast<int>(path.size() - 1));
                float localT = fIndex - static_cast<float>(i0);
                camera->setPosition(glm::mix(path[i0], path[i1], localT));
                camera->setOrientation(glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            }
		} else {
			glm::vec3 move(0.0f);
			if (keys[GLFW_KEY_W]) move.z += 1.0f;
			if (keys[GLFW_KEY_S]) move.z -= 1.0f;
			if (keys[GLFW_KEY_A]) move.x += 1.0f;
			if (keys[GLFW_KEY_D]) move.x -= 1.0f;
			if (keys[GLFW_KEY_Q]) move.y -= 1.0f;
			if (keys[GLFW_KEY_E]) move.y += 1.0f;

			if (glm::length(move) > 0.0001f) {
				if (character != camera) move.x *= -1.0f;
				character->localMove(glm::normalize(move) * SPEED * deltaTime);
			}
		}

		if (keys[GLFW_KEY_0]) character = camera;
		else if (keys[GLFW_KEY_1]) character = object1;
		else if (keys[GLFW_KEY_2]) character = object2;
		else if (keys[GLFW_KEY_3]) character = object3;
		else if (keys[GLFW_KEY_4]) character = light;
		for (int i = 0; i < 1024; ++i) {
			previousKeys[i] = keys[i];
		}

		lightMarkerObjectX->setPosition(light->getPosition());
		lightMarkerObjectY->setPosition(light->getPosition());
		lightMarkerObjectZ->setPosition(light->getPosition());

		graphics.frameBegin();
		renderer.render();
		graphics.frameEnd();
	}

    return EXIT_SUCCESS;
}