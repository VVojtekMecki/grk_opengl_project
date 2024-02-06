#include "glew.h"
#include <GLFW/glfw3.h>
#include "glm.hpp"
#include "ext.hpp"
#include <iostream>
#include <cmath>

#include "Shader_Loader.h"
#include "Render_Utils.h"
#include "Texture.h"

#include "Box.cpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>
#include <..\..\cw 7\src\SOIL\SOIL.h>
#include <vector>
#include "objects/planets_list.cpp"
#include "objects/player_ship.cpp"
#include "objects/skybox.h"

GLuint program;

Core::Shader_Loader shaderLoader;

glm::vec3 cameraPos = glm::vec3(-4.f, 0, 0);
glm::vec3 cameraDir = glm::vec3(1.f, 0.f, 0.f);


glm::vec3 spotlightPos = glm::vec3(1.4, 1.4, 1.4);
glm::vec3 spotlightConeDir = glm::vec3(1.4, 1.4, 1.4);

glm::vec3 spaceshipPos = glm::vec3(-4.f, 0, 0);
glm::vec3 spaceshipDir = glm::vec3(1.f, 0.f, 0.f);
GLuint VAO,VBO;

float aspectRatio = 1.f;

float lastFrameTime = 0.0f;
float deltaTime = 0.0f;
float planetRough = 0.3f;
float planetMetal = 0.3f;
float lightPower = 25.f;
glm::vec3 lightColor = glm::vec3(lightPower, lightPower, lightPower);


glm::mat4 createCameraMatrix()
{
	glm::vec3 cameraSide = glm::normalize(glm::cross(cameraDir,glm::vec3(0.f,1.f,0.f)));
	glm::vec3 cameraUp = glm::normalize(glm::cross(cameraSide,cameraDir));
	glm::mat4 cameraRotrationMatrix = glm::mat4({
		cameraSide.x,cameraSide.y,cameraSide.z,0,
		cameraUp.x,cameraUp.y,cameraUp.z ,0,
		-cameraDir.x,-cameraDir.y,-cameraDir.z,0,
		0.,0.,0.,1.,
		});
	cameraRotrationMatrix = glm::transpose(cameraRotrationMatrix);
	glm::mat4 cameraMatrix = cameraRotrationMatrix * glm::translate(-cameraPos);

	return cameraMatrix;
}

glm::mat4 createPerspectiveMatrix()
{
	
	glm::mat4 perspectiveMatrix;
	float n = 0.05;
	float f = 1000.;
	float a1 = glm::min(aspectRatio, 1.f);
	float a2 = glm::min(1 / aspectRatio, 1.f);
	perspectiveMatrix = glm::mat4({
		1,0.,0.,0.,
		0.,aspectRatio,0.,0.,
		0.,0.,(f+n) / (n - f),2*f * n / (n - f),
		0.,0.,-1.,0.,
		});

	
	perspectiveMatrix=glm::transpose(perspectiveMatrix);

	return perspectiveMatrix;
}

SpaceObjectsList spaceObjectsList(glfwGetTime(), createPerspectiveMatrix()* createCameraMatrix());

PlayerShip player;

Skybox skybox;

void renderScene(GLFWwindow* window)
{

	glClearColor(0.0f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 transformation;
	float time = glfwGetTime();
	deltaTime = time - lastFrameTime;
	deltaTime = glm::min(deltaTime, 0.1f);
	lastFrameTime = time;

	//spaceObjectsList.updateTime(time);
	

	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix() * glm::translate(cameraPos);
	transformation = viewProjectionMatrix;
	skybox.renderSkybox(transformation);


	glClear(GL_DEPTH_BUFFER_BIT);

	std::map<std::string, glm::mat4> modelMatrixMap = {
		{ "sun", glm::mat4() * glm::scale(glm::vec3(4.f)) },
		{ "earth", glm::eulerAngleY(time / 3) * glm::translate(glm::vec3(10.f, 0, 0)) * glm::scale(glm::vec3(1.8f)) },
		{ "moon", glm::eulerAngleY(time / 3) * glm::translate(glm::vec3(10.f, 0, 0)) * glm::eulerAngleY(time) * glm::translate(glm::vec3(3.f, 0, 0)) * glm::scale(glm::vec3(0.6f)) },
		{ "mars", glm::eulerAngleY((time + 6) / 3) * glm::translate(glm::vec3(15.f, 0, 0)) * glm::scale(glm::vec3(0.7f))},
		{ "aliensPlanet", glm::eulerAngleY(time / 3.3f) * glm::translate(glm::vec3(20.f, 0, 0)) * glm::scale(glm::vec3(1.5f))},
		{ "venus", glm::eulerAngleY(time / 4) * glm::translate(glm::vec3(25.f, 0, 0)) * glm::scale(glm::vec3(0.8f))},
		{ "humea", glm::eulerAngleY(time / 5) * glm::translate(glm::vec3(30.f, 0, 0)) * glm::scale(glm::vec3(2.f))},
		{ "mercury", glm::eulerAngleY(time / 7) * glm::translate(glm::vec3(35.f, 0, 0)) * glm::scale(glm::vec3(0.79f))},
	};

	std::map<std::string, glm::vec3> startPlanetPosition = {
		//{ "sun", glm::mat4()},
		//{ "earth",glm::eulerAngleY(time / 3)},
		//{ "moon",  glm::eulerAngleY(time / 3)},
		//{ "mars", glm::eulerAngleY((time + 6) / 3)},
		//{ "aliensPlanet", glm::eulerAngleY(time / 3.3f)},
		//{ "venus", glm::eulerAngleY(time / 4)},
		//{ "humea", glm::eulerAngleY(time / 5)},
		//{ "mercury", glm::eulerAngleY(time / 7)},
		{ "sun", glm::vec3()},
		{ "earth",glm::vec3(10.f, 0, 0)},
		{ "moon",  glm::vec3(10.f, 0, 0)},
		{ "mars", glm::vec3(15.f, 0, 0)},
		{ "aliensPlanet", glm::vec3(20.f, 0, 0)},
		{ "venus", glm::vec3(25.f, 0, 0)},
		{ "humea", glm::vec3(30.f, 0, 0)},
		{ "mercury",glm::vec3(35.f, 0, 0)},
	};
	glm::mat4 projectionMatrix = createPerspectiveMatrix() * createCameraMatrix();

	//for (SpaceObjectProperties obj : spaceObjectsList.spaceObjectsList) {
	//	obj.object->drawObjectTexture(projectionMatrix, modelMatrixMap.at(obj.name));
	//}
	// 
	for (SpaceObjectProperties obj : spaceObjectsList.spaceObjectsList) {
		obj.object->drawWithPBR(projectionMatrix, modelMatrixMap.at(obj.name), planetRough, planetMetal, lightColor, lightPower, cameraPos, 
			startPlanetPosition.at(obj.name), spotlightPos, spotlightConeDir);
	}

	//for (SpaceObjectProperties obj : spaceObjectsList.spaceObjectsList) {
	//	obj.object->drawWithPBR(projectionMatrix, modelMatrixMap.at(obj.name), planetRough, planetMetal, lightColor, lightPower, cameraPos,
	//		startPlanetPosition.at(obj.name), spotlightPos, spotlightConeDir);
	//}

	glm::vec3 spaceshipSide = glm::normalize(glm::cross(spaceshipDir, glm::vec3(0.f, 1.f, 0.f)));
	glm::vec3 spaceshipUp = glm::normalize(glm::cross(spaceshipSide, spaceshipDir));
	glm::mat4 specshipCameraRotrationMatrix = glm::mat4({
		spaceshipSide.x,spaceshipSide.y,spaceshipSide.z,0,
		spaceshipUp.x,spaceshipUp.y,spaceshipUp.z ,0,
		-spaceshipDir.x,-spaceshipDir.y,-spaceshipDir.z,0,
		0.,0.,0.,1.,
		});

	glm::mat4 shipModelMatrix = glm::translate(spaceshipPos) * specshipCameraRotrationMatrix * glm::eulerAngleY(glm::pi<float>()) * glm::scale(glm::vec3(0.04f));

	//player.ship->drawObjectTexture(projectionMatrix, shipModelMatrix);
	player.ship->drawWithPBR(projectionMatrix, shipModelMatrix, planetRough, planetMetal, lightColor, lightPower, cameraPos, glm::vec3(),
		spotlightPos, spotlightConeDir);
	spotlightPos = spaceshipPos + 0.2 * spaceshipDir;
	spotlightConeDir = spaceshipDir;
	glfwSwapBuffers(window);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	aspectRatio = width / float(height);
	glViewport(0, 0, width, height);
}

void loadModelToContext(std::string path, Core::RenderContext& context)
{
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}
	context.initFromAssimpMesh(scene->mMeshes[0]);
}

void init(GLFWwindow* window)
{
	skybox.init();

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glEnable(GL_DEPTH_TEST);
	program = shaderLoader.CreateProgram("shaders/shader_5_1.vert", "shaders/shader_5_1.frag");

	spaceObjectsList.init();
	player.init();
}

void shutdown(GLFWwindow* window)
{
	shaderLoader.DeleteProgram(program);
}

void processInput(GLFWwindow* window)
{
	glm::vec3 spaceshipSide = glm::normalize(glm::cross(spaceshipDir, glm::vec3(0.f, 1.f, 0.f)));
	glm::vec3 spaceshipUp = glm::vec3(0.f, 1.f, 0.f);
	float angleSpeed = 2.f * deltaTime;
	float moveSpeed = 2.f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		spaceshipPos += spaceshipDir * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		spaceshipPos -= spaceshipDir * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
		spaceshipPos += spaceshipSide * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
		spaceshipPos -= spaceshipSide * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		spaceshipPos += spaceshipUp * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		spaceshipPos -= spaceshipUp * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		spaceshipDir = glm::vec3(glm::eulerAngleY(angleSpeed) * glm::vec4(spaceshipDir, 0));
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		spaceshipDir = glm::vec3(glm::eulerAngleY(-angleSpeed) * glm::vec4(spaceshipDir, 0));
	//jasno??
	float powerSpeed = 0.05f;
	
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && lightPower < 25.f)
		lightPower += powerSpeed;
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && lightPower > 2.5f)
		lightPower -= powerSpeed;

	lightColor = glm::vec3(lightPower, lightPower, lightPower);

	cameraPos = spaceshipPos - 1.5 * spaceshipDir + glm::vec3(0, 1, 0) * 0.5f;
	cameraDir = spaceshipDir;

}

void renderLoop(GLFWwindow* window) {
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		renderScene(window);
		glfwPollEvents();
	}
}