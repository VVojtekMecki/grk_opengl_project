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
//#include <..\..\src\SOIL\SOIL.h>
#include <vector>


namespace texture {
	GLuint earth;
	GLuint clouds;

	GLuint mercury;
	GLuint mercuryNormal;

	GLuint aliensPlanet;
	GLuint aliensPlanetNormal;
	GLuint aliensPlanetClouds;

	GLuint venus;
	GLuint venusNormal;
	
	GLuint mars;
	GLuint marsNormal;

	GLuint jupiter;
	GLuint jupiterNormal;
	
	GLuint haumea;
	GLuint haumeaNormal;

	GLuint asteroid_grey;
	GLuint asteroid_red;
	GLuint asteroidNormal;

	GLuint ship;
	GLuint rust;

	GLuint moon;
	GLuint sun;
	GLuint cubemap;

	GLuint shipScratches;
	GLuint earthNormal;
	GLuint shipNormal;
	GLuint moonNormal;
	GLuint rustNormal;

	GLuint skybox;
}


GLuint program;
GLuint programSun;
GLuint programTex;
GLuint programEarth;
GLuint programProcTex;
GLuint programShip;
GLuint programSkybox;
GLuint programAsteroid;

Core::Shader_Loader shaderLoader;

Core::RenderContext shipContext;
Core::RenderContext sphereContext;
Core::RenderContext cubeMapContex;
Core::RenderContext mercuryPlanetContex;
Core::RenderContext asteroidContext;

glm::vec3 cameraPos = glm::vec3(-4.f, 0, 0);
glm::vec3 cameraDir = glm::vec3(1.f, 0.f, 0.f);

glm::vec3 spaceshipPos = glm::vec3(-4.f, 0, 0);
glm::vec3 spaceshipDir = glm::vec3(1.f, 0.f, 0.f);
GLuint VAO,VBO;
GLuint asteroidVAO;
GLuint asteroidVBO; // Буфер вершин
GLuint asteroidEBO; // Буфер индексов
GLsizei asteroidIndicesCount;
float aspectRatio = 1.f;

float lastFrameTime = 0.0f;
float deltaTime = 0.0f;
struct Vertex {
	glm::vec3 position;
	glm::vec3 color;
	// Дополнительные данные, такие как нормали, текстурные координаты и т.д.
};

// Пример данных для буфера вершин
std::vector<Vertex> vertices = {
	// Вершина 1
	{glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)},
	// Вершина 2
	{glm::vec3(1.0f, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
	// Вершина 3
	{glm::vec3(0.0f,  1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)}
	// Дополнительные вершины...
};

// Пример данных для буфера индексов
std::vector<GLuint> indices = {
	0, 1, 2 // Порядок вершин, определяющий треугольники
	// Дополнительные индексы...
};
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
	float f = 20.;
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
	glUniform3f(glGetUniformLocation(program, "lightPos"), 0,0,0);

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

	if (textureID == texture::sun) {
		Core::SetActiveTexture(texture::sun , "tex", program, 0);
	}
	else if (textureID == texture::earth) {
		Core::SetActiveTexture(textureID, "earth", program, 0);
		Core::SetActiveTexture(texture::clouds, "clouds", program, 1);
		Core::SetActiveTexture(normalmapId, "normalSampler", program, 2);
	}
	else if (textureID == texture::moon) {
		Core::SetActiveTexture(textureID, "tex", program, 0);
		Core::SetActiveTexture(normalmapId, "normalSampler", program, 1);


	}
	else if (textureID == texture::ship) {
		Core::SetActiveTexture(texture::ship, "ship", program, 0);
		Core::SetActiveTexture(texture::rust, "rust", program, 1);
		Core::SetActiveTexture(texture::shipScratches, "asteroid", program, 2);
		Core::SetActiveTexture(normalmapId, "normalSampler", program, 3);


	}
	else if (textureID == texture::aliensPlanet) {
		Core::SetActiveTexture(textureID, "earth", program, 0);
		Core::SetActiveTexture(texture::clouds, "clouds", program, 1);
		Core::SetActiveTexture(normalmapId, "normalSampler", program, 2);
	}
	else {
		Core::SetActiveTexture(textureID, "tex", program, 0);
		Core::SetActiveTexture(normalmapId, "normalSampler", program, 1);
	}

	Core::DrawContext(context);
	glUseProgram(0);
}

void drawAsteroid(Core::RenderContext& asteroidContext, float scale, float rotationSpeed, const glm::vec3& translate, float time, int instanceCount) {
	glm::mat4 asteroidScale = glm::scale(glm::vec3(scale));
	glm::mat4 asteroidRotate = glm::rotate(time * rotationSpeed, glm::vec3(0, 1, 0));
	glm::mat4 asteroidTranslate = glm::translate(translate);

	drawObjectColor(asteroidContext, asteroidScale * asteroidRotate * asteroidTranslate,  glm::vec3(0.50, 0.5, 0.5), instanceCount);
}

	int getRandomNumber(int min, int max) {
		// Инициализация генератора случайных чисел

		// Генерация случайного числа в заданном диапазоне
		int randomNumber = rand() % (max - min + 1) + min;
		return randomNumber;
	}

/*int getRandomNumber(int min, int max)
{
	static const double fraction = 1.0 / (static_cast<double>(RAND_MAX) + 1.0);
	return static_cast<int>(rand() * fraction * (max - min + 1) + min);
}*/


double getDoubleRandomNumber(double min, double max)
{
	static const double fraction = 1.0 / (static_cast<double>(RAND_MAX) + 1.0);
	return static_cast<double>(rand() * fraction * (max - min ) + min);
}

void drawAsteroidInstanced(Core::RenderContext& context, int instanceCount) {
	glUseProgram(programAsteroid);
	glBindVertexArray(asteroidVAO);

	glDrawElementsInstanced(GL_TRIANGLES, asteroidIndicesCount, GL_UNSIGNED_INT, 0, instanceCount);
	Core::DrawContext(context);
	glBindVertexArray(0);
	glUseProgram(0);
}
void renderScene(GLFWwindow* window)
{

	glClearColor(0.0f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 transformation;
	float timeGL = glfwGetTime();
	deltaTime = timeGL - lastFrameTime;
	deltaTime = glm::min(deltaTime, 0.1f);
	lastFrameTime = timeGL;


	glUseProgram(programSkybox);
	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix() * glm::translate(cameraPos);
	transformation = viewProjectionMatrix;
	glUniformMatrix4fv(glGetUniformLocation(programSkybox, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture::cubemap);
	Core::DrawContext(cubeMapContex);
	glClear(GL_DEPTH_BUFFER_BIT);
	//glUniform1i(glGetUniformLocation(skyboxProgram, "skybox"), 0);

	//sun
	drawObjectTexture(programSun, sphereContext, glm::mat4() * glm::scale(glm::vec3(4.f)), texture::sun, texture::sun);
	//earth
	drawObjectTexture(programEarth, sphereContext, glm::eulerAngleY(timeGL / 3) * glm::translate(glm::vec3(10.f, 0, 0)) * glm::scale(glm::vec3(1.8f)), texture::earth, texture::earthNormal);
	//moon
	drawObjectTexture(programTex, sphereContext,
		glm::eulerAngleY(timeGL / 3) * glm::translate(glm::vec3(10.f, 0, 0)) * glm::eulerAngleY(timeGL) * glm::translate(glm::vec3(3.f, 0, 0)) * glm::scale(glm::vec3(0.6f)), texture::moon, texture::moonNormal);
	//mars
	drawObjectTexture(programTex, sphereContext, glm::eulerAngleY((timeGL +6) / 3) * glm::translate(glm::vec3(15.f, 0, 0)) * glm::scale(glm::vec3(0.7f)), texture::mars, texture::marsNormal);
	//aliens planet
	drawObjectTexture(programEarth, sphereContext, glm::eulerAngleY(timeGL /3.3f) * glm::translate(glm::vec3(20.f, 0, 0)) * glm::scale(glm::vec3(1.5f)), texture::aliensPlanet, texture::aliensPlanetNormal);
	//venus
	drawObjectTexture(programTex, sphereContext, glm::eulerAngleY(timeGL / 4) * glm::translate(glm::vec3(25.f, 0, 0)) * glm::scale(glm::vec3(0.8f)), texture::venus, texture::venusNormal);
	//haumea
	drawObjectTexture(programTex, sphereContext, glm::eulerAngleY(timeGL / 5) * glm::translate(glm::vec3(30.f, 0, 0)) * glm::scale(glm::vec3(2.f)), texture::haumea, texture::haumeaNormal);
	//mercury
	drawObjectTexture(programTex, sphereContext, glm::eulerAngleY(timeGL /2) * glm::translate(glm::vec3(35.f, 0, 0)) * glm::scale(glm::vec3(0.79f)), texture::mercury, texture::mercuryNormal);

	float position = -20.0f;
	int positionX, positionY, positionZ = 0;
	int amount = 5;
		/*for (int group = 0; group < 3; group++) {
			for (int asteroid = 0; asteroid < amount; asteroid++) {
				positionX = getRandomNumber()
				glm::mat4 asteroidScale = glm::scale(glm::vec3(0.5f));
				glm::mat4 asteroidRotate = glm::rotate(time * 0.01f, glm::vec3(0, 1, 0));
				//glm::vec3 translate = glm::vec3(position + 2.0f, float(group % 3), position + 12.0f + group / 3.0f);

				glm::mat4 asteroidTranslate = glm::translate(glm::vec3(position-1.0f , position-2.0f ,position-1.0f +0.3f));
				//glm::vec3 translate = glm::vec3(float(groupPositionX) + relativeX, float(groupPositionY) + relativeY, 12.0f + relativeZ);
				drawObjectTexture(programTex, asteroidContext, asteroidScale*asteroidRotate*asteroidTranslate,texture::asteroid_grey, texture::asteroidNormal) ;
				position++;
			}
			position = position + 10.0f;

			amount--;

		}*/
	srand(static_cast<unsigned>(time(NULL)));

	//positionX = 0;
	//positionY = -1;
	//positionZ = 12;
	for (int asteroid=0; asteroid < 20; asteroid++) {
		positionX = getRandomNumber(-2, 2);
		positionY = getRandomNumber(-10, 10);
		positionZ = getRandomNumber(-15, 15);
		bool truePositionZ = false;



		/*while (truePositionZ != true) {
			positionZ = getRandomNumber(-15, 15);

			if (positionZ < -8 || positionZ > 8) {
				truePositionZ = true;
				break;
			}
		}*/
		glm::mat4 asteroid1Scale = glm::scale(glm::vec3(0.5f));
		glm::mat4 asteroid1Rotate = glm::rotate(timeGL * 0.01f, glm::vec3(0, 1, 0));
		glm::mat4 asteroid1Translate = glm::translate(glm::vec3(float(positionX), float(positionY), float(positionZ)));
		drawObjectTexture(programTex, asteroidContext, asteroid1Scale * asteroid1Rotate * asteroid1Translate, texture::sun, texture::asteroidNormal);
	}
	/*glm::mat4 asteroid1Scale = glm::scale(glm::vec3(0.5f));
	glm::mat4 asteroid1Rotate = glm::rotate(time * 0.01f, glm::vec3(1, 1, 0));
	glm::mat4 asteroid1Translate = glm::translate(glm::vec3(0, 0, 12));

	glm::mat4 asteroid2Scale = glm::scale(glm::vec3(0.5f));
	glm::mat4 asteroid2Rotate = glm::rotate(time * 0.01f, glm::vec3(1, 1, 0));
	glm::mat4 asteroid2Translate = glm::translate(glm::vec3(2, 6, 12.2));

	glm::mat4 asteroid3Scale = glm::scale(glm::vec3(0.5f));
	glm::mat4 asteroid3Rotate = glm::rotate(time * 0.01f, glm::vec3(0, 1, 0));
	glm::mat4 asteroid3Translate = glm::translate(glm::vec3(-2, 6, 11.6));


	glm::mat4 asteroid4Scale = glm::scale(glm::vec3(-1.f));
	glm::mat4 asteroid4Rotate = glm::rotate(time * 0.01f, glm::vec3(0, 1, 0));
	glm::mat4 asteroid4Translate = glm::translate(glm::vec3(0, 3, 12));
	glm::mat4 asteroid5Scale = glm::scale(glm::vec3(0.5f));
	glm::mat4 asteroid5Rotate = glm::rotate(time * 0.01f, glm::vec3(0, 1, 0));
	glm::mat4 asteroid5Translate = glm::translate(glm::vec3(5, 0, 12.4));
		//glm::vec3 translate = glm::vec3(float(groupPositionX) + relativeX, float(groupPositionY) + relativeY, 12.0f + relativeZ);
		drawObjectTexture(programTex, asteroidContext, asteroid1Scale * asteroid1Rotate * asteroid1Translate, texture::asteroid_grey, texture::asteroidNormal);
		drawObjectTexture(programTex, asteroidContext, asteroid2Scale * asteroid2Rotate * asteroid2Translate, texture::asteroid_red, texture::asteroidNormal);
		drawObjectTexture(programTex, asteroidContext, asteroid3Scale * asteroid3Rotate * asteroid3Translate, texture::sun, texture::asteroidNormal);
		drawObjectTexture(programTex, asteroidContext, asteroid4Scale * asteroid4Rotate * asteroid4Translate, texture::earth, texture::asteroidNormal);
		drawObjectTexture(programTex, asteroidContext, asteroid5Scale * asteroid5Rotate * asteroid5Translate, texture::aliensPlanet, texture::asteroidNormal);

		*/


	glm::vec3 spaceshipSide = glm::normalize(glm::cross(spaceshipDir, glm::vec3(0.f, 1.f, 0.f)));
	glm::vec3 spaceshipUp = glm::normalize(glm::cross(spaceshipSide, spaceshipDir));
	glm::mat4 specshipCameraRotrationMatrix = glm::mat4({
		spaceshipSide.x,spaceshipSide.y,spaceshipSide.z,0,
		spaceshipUp.x,spaceshipUp.y,spaceshipUp.z ,0,
		-spaceshipDir.x,-spaceshipDir.y,-spaceshipDir.z,0,
		0.,0.,0.,1.,
		});


	//drawObjectColor(shipContext,
	//	glm::translate(cameraPos + 1.5 * cameraDir + cameraUp * -0.5f) * inveseCameraRotrationMatrix * glm::eulerAngleY(glm::pi<float>()),
	//	glm::vec3(0.3, 0.3, 0.5)
	//	);
	drawObjectTexture(programShip, shipContext,
		glm::translate(spaceshipPos) * specshipCameraRotrationMatrix * glm::eulerAngleY(glm::pi<float>()) * glm::scale(glm::vec3(0.04f)),
		texture::ship, texture::shipNormal
	);

	//glUseProgram(0);
	glfwSwapBuffers(window);
}

void initAsteroidRendering() {
	// Генерация VAO
	glGenVertexArrays(1, &asteroidVAO);
	glBindVertexArray(asteroidVAO);

	// Генерация буфера вершин
	glGenBuffers(1, &asteroidVBO);
	glBindBuffer(GL_ARRAY_BUFFER, asteroidVBO);

	// Ваши вершины для астероида
	Vertex asteroidVertices[] = {
		{glm::vec3(-1.0f, -1.0f, 0.0f)},
		{glm::vec3(1.0f, -1.0f, 0.0f)},
		{glm::vec3(1.0f, 1.0f, 0.0f)},
		{glm::vec3(-1.0f, 1.0f, 0.0f)}
	};

	glBufferData(GL_ARRAY_BUFFER, sizeof(asteroidVertices), asteroidVertices, GL_STATIC_DRAW);

	// Настройка указателей атрибутов
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	// Генерация буфера индексов
	glGenBuffers(1, &asteroidEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, asteroidEBO);

	// Ваши индексы для астероида
	GLuint asteroidIndices[] = {
		0, 1, 2,
		2, 3, 0
	};

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(asteroidIndices), asteroidIndices, GL_STATIC_DRAW);

	// Сохранение количества индексов
	asteroidIndicesCount = sizeof(asteroidIndices) / sizeof(GLuint);
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

/*GLuint loadSkybox() {
	std::vector<std::string> skyboxFaces = {
"space_rt.png", "space_lf.png",
"space_up.png", "space_dn.png",
"space_ft.png", "space_bk.png"
	};
	GLuint cubemapTexture;
	glGenTextures(1, &cubemapTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

	int w, h;
	unsigned char* image;
	for (unsigned int i = 0; i < 6; i++) {
		std::string filepath = "skybox/" + skyboxFaces[i];
		image = SOIL_load_image(filepath.c_str(), &w, &h, 0, SOIL_LOAD_RGBA);
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image
		);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return cubemapTexture;
}*/


void init(GLFWwindow* window)
{
	//initAsteroidRendering();
	glGenTextures(1, &texture::cubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture::cubemap);
	int w, h;
	unsigned char* data;
	std::vector<std::string> filepaths = {
	 "./textures/skybox/lightblue/right.png",
	  "./textures/skybox/lightblue/left.png",
	  "./textures/skybox/lightblue/top.png",
	  "./textures/skybox/lightblue/bot.png",
	  "./textures/skybox/lightblue/front.png",
	  "./textures/skybox/lightblue/back.png",
	  /*/textures/skybox/red/bkg1_right1.png",
	  "./textures/skybox/red/bkg1_left2.png",
	  "./textures/skybox/red/bkg1_top3.png",
	  "./textures/skybox/red/bkg1_bottom4.png",
	  "./textures/skybox/red/bkg1_front5.png",
	  "./textures/skybox/red/bkg1_back6.png",*/

	};
	for (unsigned int i = 0; i < 6; i++)
	{
		unsigned char* image = SOIL_load_image(filepaths[i].c_str(), &w, &h, 0, SOIL_LOAD_RGBA);
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image
		);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glEnable(GL_DEPTH_TEST);
	program = shaderLoader.CreateProgram("shaders/shader_5_1.vert", "shaders/shader_5_1.frag");
	programTex = shaderLoader.CreateProgram("shaders/shader_5_1_tex_copy.vert", "shaders/shader_5_1_tex_copy.frag");
	programEarth = shaderLoader.CreateProgram("shaders/shader_earth.vert", "shaders/shader_earth.frag");
	programProcTex = shaderLoader.CreateProgram("shaders/shader_5_1_tex.vert", "shaders/shader_5_1_tex.frag");
	programSun = shaderLoader.CreateProgram("shaders/shader_5_sun.vert", "shaders/shader_5_sun.frag");
	programShip = shaderLoader.CreateProgram("shaders/shader_ship.vert", "shaders/shader_ship.frag");
	programSkybox = shaderLoader.CreateProgram("shaders/shader_skybox.vert", "shaders/shader_skybox.frag");
	//programAsteroid = shaderLoader.CreateProgram("shaders/shader_asteroid.vert", "shaders/shader_asteroid.frag");

	
	loadModelToContext("./models/sphere.obj", sphereContext);
	loadModelToContext("./models/SciFi_Fighter.obj", shipContext);
	//loadModelToContext("./models/SciFi_Fighter.obj", shipContext);
	loadModelToContext("./models/cube.obj", cubeMapContex);
	loadModelToContext("./models/asteroid.obj", asteroidContext);


	


	//new objects



	//texture::skybox = loadSkybox();

	texture::sun = Core::LoadTexture("textures/planets/8k_sun.jpg");

	texture::earth = Core::LoadTexture("textures/planets/8k_earth_daymap.jpg");
	texture::clouds = Core::LoadTexture("textures/planets/8k_earth_clouds.jpg");
	texture::earthNormal = Core::LoadTexture("textures/planets/earth2_normals.png");

	texture::ship = Core::LoadTexture("textures/spaceship/SF_Fighter-Albedo.jpg");
	texture::shipNormal = Core::LoadTexture("textures/spaceship/SF_Fighter_Normal.jpg");
	texture::rust = Core::LoadTexture("textures/spaceship/rust.jpg");
	texture::rustNormal = Core::LoadTexture("textures/spaceship/rust_normal.jpg");
	texture::shipScratches = Core::LoadTexture("textures/spaceship/scratches.png");
	
	texture::moon = Core::LoadTexture("textures/planets/8k_moon.jpg");
	texture::moonNormal = Core::LoadTexture("textures/planets/moon_normal.jpg");

	texture::mercury = Core::LoadTexture("textures/planets/mercury.png");
	texture::mercuryNormal = Core::LoadTexture("textures/planets/mercury_normal.jpg");

	texture::aliensPlanet = Core::LoadTexture("textures/planets/aliensPlanet.png");
	texture::aliensPlanetNormal = Core::LoadTexture("textures/planets/aliensPlanet_normal.jpg");
	texture::aliensPlanetClouds = Core::LoadTexture("textures/planets/aliensPlanet_clouds.jpg");

	texture::venus = Core::LoadTexture("textures/planets/venus.jpg");
	texture::venusNormal = Core::LoadTexture("textures/planets/venus_normal.jpg");

	texture::mars = Core::LoadTexture("textures/planets/mars.jpg");
	texture::marsNormal = Core::LoadTexture("textures/planets/mars_normal.jpg");

	texture::jupiter = Core::LoadTexture("textures/planets/jupiter.jpg");
	texture::jupiterNormal = Core::LoadTexture("textures/planets/jupiter_normal.jpg");
	
	texture::haumea = Core::LoadTexture("textures/planets/haumea.jpg");
	texture::haumeaNormal = Core::LoadTexture("textures/planets/haumea_normal.jpg");

	texture::asteroid_grey = Core::LoadTexture("textures/asteroids/4k_haumea_fictional.jpg");
	texture::asteroid_red = Core::LoadTexture("textures/asteroids/4k_makemake_fictional.jpg");
	texture::asteroidNormal = Core::LoadTexture("textures/asteroids/asteroid_normal.jpg");






}

void shutdown(GLFWwindow* window)
{
	shaderLoader.DeleteProgram(program);
}

//obsluga wejscia
void processInput(GLFWwindow* window)
{
	glm::vec3 spaceshipSide = glm::normalize(glm::cross(spaceshipDir, glm::vec3(0.f, 1.f, 0.f)));
	glm::vec3 spaceshipUp = glm::vec3(0.f, 1.f, 0.f);
	float angleSpeed = 4.f * deltaTime;
	float moveSpeed = 4.f * deltaTime;
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

	//cameraDir = glm::normalize(-cameraPos);

}

// funkcja jest glowna petla
void renderLoop(GLFWwindow* window) {
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		renderScene(window);
		glfwPollEvents();
	}
}
//}