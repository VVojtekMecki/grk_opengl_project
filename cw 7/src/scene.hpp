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
using namespace std;

GLuint program;

Core::Shader_Loader shaderLoader;

glm::vec3 cameraPos = glm::vec3(-4.f, 0, 0);
glm::vec3 cameraDir = glm::vec3(1.f, 0.f, 0.f);

glm::vec3 spaceshipPos = glm::vec3(-4.f, 0, 0);
glm::vec3 spaceshipDir = glm::vec3(1.f, 0.f, 0.f);
GLuint VAO,VBO;

namespace texture {
	GLuint asteroid_grey;
	GLuint asteroid_red;
	GLuint asteroidNormal;
}
GLuint programTex;

GLuint asteroidVAO;
GLuint asteroidVBO; // Буфер вершин
GLuint asteroidEBO; // Буфер индексов
GLsizei asteroidIndicesCount;

int positionX, positionY, positionZ = 0;
const int amountAsteroids = 100;

int asteroidPositions[amountAsteroids][3];
GLuint asteroidTextures[amountAsteroids];

GLuint asteroidTexture;
Core::RenderContext asteroidContext;



GLuint getRandomAsteroidTexture() {
	int randomIndex = std::rand() % 2;
	return (randomIndex == 0) ? texture::asteroid_grey : texture::asteroid_red;

}


int getRandomNumber(int min, int max) {
	int randomNumber = rand() % (max - min + 1) + min;
	return randomNumber;
}

float aspectRatio = 1.f;

float lastFrameTime = 0.0f;
float deltaTime = 0.0f;



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

void drawObjectColor(Core::RenderContext& context, glm::mat4 modelMatrix, glm::vec3 color, int instanceCount) {

	glUseProgram(program);
	glBindVertexArray(asteroidVAO);

	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
	glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
	glUniform3f(glGetUniformLocation(program, "color"), color.x, color.y, color.z);
	glUniform3f(glGetUniformLocation(program, "lightPos"), 0, 0, 0);

	glDrawElementsInstanced(GL_TRIANGLES, asteroidIndicesCount, GL_UNSIGNED_INT, 0, instanceCount);
	Core::DrawContext(context);
	glUseProgram(0);

}

void drawObjectTexture(GLuint program, Core::RenderContext& context, glm::mat4 modelMatrix, GLuint textureID, GLuint normalmapId) {
	glUseProgram(program);
	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
	glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
	Core::SetActiveTexture(textureID, "tex", program, 0);
	Core::SetActiveTexture(normalmapId, "normalSampler", program, 1);

	Core::DrawContext(context);
	glUseProgram(0);
}

void drawAsteroid(Core::RenderContext& asteroidContext, float scale, float rotationSpeed, const glm::vec3& translate, float time, int instanceCount) {
	glm::mat4 asteroidScale = glm::scale(glm::vec3(scale));
	glm::mat4 asteroidRotate = glm::rotate(time * rotationSpeed, glm::vec3(0, 1, 0));
	glm::mat4 asteroidTranslate = glm::translate(translate);

	drawObjectColor(asteroidContext, asteroidScale * asteroidRotate * asteroidTranslate, glm::vec3(0.50, 0.5, 0.5), instanceCount);
}

SpaceObjectsList spaceObjectsList(glfwGetTime(), createPerspectiveMatrix()* createCameraMatrix());

PlayerShip player;

Skybox skybox;

void renderScene(GLFWwindow* window)
{

	glClearColor(0.0f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 transformation;
	float timeGl = glfwGetTime();
	deltaTime = timeGl - lastFrameTime;
	deltaTime = glm::min(deltaTime, 0.1f);
	lastFrameTime = timeGl;

	//spaceObjectsList.updateTime(time);
	

	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix() * glm::translate(cameraPos);
	transformation = viewProjectionMatrix;
	skybox.renderSkybox(transformation);


	glClear(GL_DEPTH_BUFFER_BIT);

	std::map<std::string, glm::mat4> modelMatrixMap = {
		{ "sun", glm::mat4() * glm::scale(glm::vec3(4.f)) },
		{ "earth", glm::eulerAngleY(timeGl / 3) * glm::translate(glm::vec3(10.f, 0, 0)) * glm::scale(glm::vec3(1.8f)) },
		{ "moon", glm::eulerAngleY(timeGl / 3) * glm::translate(glm::vec3(10.f, 0, 0)) * glm::eulerAngleY(timeGl) * glm::translate(glm::vec3(3.f, 0, 0)) * glm::scale(glm::vec3(0.6f)) },
		{ "mars", glm::eulerAngleY((timeGl + 6) / 3) * glm::translate(glm::vec3(15.f, 0, 0)) * glm::scale(glm::vec3(0.7f))},
		{ "aliensPlanet", glm::eulerAngleY(timeGl / 3.3f) * glm::translate(glm::vec3(20.f, 0, 0)) * glm::scale(glm::vec3(1.5f))},
		{ "venus", glm::eulerAngleY(timeGl / 4) * glm::translate(glm::vec3(25.f, 0, 0)) * glm::scale(glm::vec3(0.8f))},
		{ "humea", glm::eulerAngleY(timeGl / 5) * glm::translate(glm::vec3(30.f, 0, 0)) * glm::scale(glm::vec3(2.f))},
		{ "mercury", glm::eulerAngleY(timeGl / 7) * glm::translate(glm::vec3(35.f, 0, 0)) * glm::scale(glm::vec3(0.79f))}
	};

	glm::mat4 projectionMatrix = createPerspectiveMatrix() * createCameraMatrix();

	for (SpaceObjectProperties obj : spaceObjectsList.spaceObjectsList) {
		obj.object->drawObjectTexture(projectionMatrix, modelMatrixMap.at(obj.name));
	}




	float position = -20.0f;

	srand(static_cast<unsigned>(time(0)));
	for (int asteroid = 0; asteroid < amountAsteroids; asteroid++) {
		positionX = asteroidPositions[asteroid][0];
		positionY = asteroidPositions[asteroid][1];
		positionZ = asteroidPositions[asteroid][2];

		glm::mat4 asteroid1Scale = glm::scale(glm::vec3(0.5f));
		glm::mat4 asteroid1Rotate = glm::rotate(timeGl * 0.05f, glm::vec3(0, 1, 0));
		glm::mat4 asteroid1Translate = glm::translate(glm::vec3(positionX, positionY, positionZ));
		drawObjectTexture(programTex, asteroidContext, asteroid1Scale * asteroid1Rotate * asteroid1Translate, asteroidTextures[asteroid], texture::asteroidNormal);
	}





	glm::vec3 spaceshipSide = glm::normalize(glm::cross(spaceshipDir, glm::vec3(0.f, 1.f, 0.f)));
	glm::vec3 spaceshipUp = glm::normalize(glm::cross(spaceshipSide, spaceshipDir));
	glm::mat4 specshipCameraRotrationMatrix = glm::mat4({
		spaceshipSide.x,spaceshipSide.y,spaceshipSide.z,0,
		spaceshipUp.x,spaceshipUp.y,spaceshipUp.z ,0,
		-spaceshipDir.x,-spaceshipDir.y,-spaceshipDir.z,0,
		0.,0.,0.,1.,
		});

	glm::mat4 shipModelMatrix = glm::translate(spaceshipPos) * specshipCameraRotrationMatrix * glm::eulerAngleY(glm::pi<float>()) * glm::scale(glm::vec3(0.04f));

	player.ship->drawObjectTexture(projectionMatrix, shipModelMatrix);

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
	programTex = shaderLoader.CreateProgram("shaders/shader_5_1_tex_copy.vert", "shaders/shader_5_1_tex_copy.frag");

	loadModelToContext("./models/asteroid.obj", asteroidContext);

	texture::asteroid_grey = Core::LoadTexture("textures/asteroids/4k_haumea_fictional.jpg");
	texture::asteroid_red = Core::LoadTexture("textures/asteroids/4k_makemake_fictional.jpg");
	texture::asteroidNormal = Core::LoadTexture("textures/asteroids/asteroid_normal.jpg");

	srand(static_cast<unsigned>(time(0)));
	for (int asteroid = 0; asteroid < amountAsteroids; asteroid++) {
		positionX = getRandomNumber(-65, 65);
		positionY = getRandomNumber(-35, 35);
		positionZ = getRandomNumber(-35, 35);

		bool truePositionZ = false;
		if (positionY > -8 && positionY < 8) {
			while (truePositionZ != true) {
				if (positionZ < -8 || positionZ > 8) {
					truePositionZ = true;
					break;
				}
				positionZ = getRandomNumber(-35, 35);
			}
		}

		asteroidPositions[asteroid][0] = positionX;
		asteroidPositions[asteroid][1] = positionY;
		asteroidPositions[asteroid][2] = positionZ;
		asteroidTextures[asteroid] = getRandomAsteroidTexture();

		spaceObjectsList.init();
		player.init();
	}
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