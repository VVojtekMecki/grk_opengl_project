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
	GLuint rust;
	GLuint moon;
	GLuint ship;
	GLuint sun;
	GLuint grid;
	GLuint asteroidNormal;
	GLuint cubemap;
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

Core::Shader_Loader shaderLoader;

Core::RenderContext shipContext;
Core::RenderContext sphereContext;
Core::RenderContext cubeMapContex;

glm::vec3 cameraPos = glm::vec3(-4.f, 0, 0);
glm::vec3 cameraDir = glm::vec3(1.f, 0.f, 0.f);

glm::vec3 spaceshipPos = glm::vec3(-4.f, 0, 0);
glm::vec3 spaceshipDir = glm::vec3(1.f, 0.f, 0.f);
GLuint VAO,VBO;

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

void drawObjectColor(Core::RenderContext& context, glm::mat4 modelMatrix, glm::vec3 color) {

	glUseProgram(program);
	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
	glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
	glUniform3f(glGetUniformLocation(program, "color"), color.x, color.y, color.z);
	glUniform3f(glGetUniformLocation(program, "lightPos"), 0,0,0);
	Core::DrawContext(context);

}
void drawObjectTexture(GLuint program, Core::RenderContext& context, glm::mat4 modelMatrix, GLuint textureID, GLuint normalmapId) {
	glUseProgram(program);
	//glUseProgram(skyboxProgram);
	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
	glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
	//glUniform3f(glGetUniformLocation(program, "lightPos"), 0, 0, 0);
	//Core::SetActiveTexture(textureID, "colorTexture", programTex, 0);
	if (textureID == texture::sun) {
		Core::SetActiveTexture(textureID, "tex", program, 0);
	}
	else if (textureID == texture::earth) {
		Core::SetActiveTexture(texture::earth, "earth", program, 0);
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
		Core::SetActiveTexture(texture::asteroidNormal, "asteroid", program, 2);
		Core::SetActiveTexture(normalmapId, "normalSampler", program, 3);


	}
	Core::DrawContext(context);
	glUseProgram(0);
}

void renderScene(GLFWwindow* window)
{

	glClearColor(0.0f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 transformation;
	float time = glfwGetTime();
	deltaTime = time - lastFrameTime;
	deltaTime = glm::min(deltaTime, 0.1f);
	lastFrameTime = time;


	glUseProgram(programSkybox);
	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix() * glm::translate(cameraPos);
	transformation = viewProjectionMatrix;
	glUniformMatrix4fv(glGetUniformLocation(programSkybox, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture::cubemap);
	Core::DrawContext(cubeMapContex);
	glClear(GL_DEPTH_BUFFER_BIT);
	//glUniform1i(glGetUniformLocation(skyboxProgram, "skybox"), 0);

	drawObjectTexture(programSun, sphereContext, glm::mat4(), texture::sun, texture::sun);

	drawObjectTexture(programEarth, sphereContext, glm::eulerAngleY(time / 3) * glm::translate(glm::vec3(4.f, 0, 0)) * glm::scale(glm::vec3(0.3f)), texture::earth, texture::earthNormal);

	drawObjectTexture(programTex, sphereContext,
		glm::eulerAngleY(time / 3) * glm::translate(glm::vec3(4.f, 0, 0)) * glm::eulerAngleY(time) * glm::translate(glm::vec3(1.f, 0, 0)) * glm::scale(glm::vec3(0.1f)), texture::moon, texture::moonNormal);

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
	
	loadModelToContext("./models/sphere.obj", sphereContext);
	loadModelToContext("./models/SciFi_Fighter.fbx", shipContext);
	loadModelToContext("./models/cube.obj", cubeMapContex);
	//texture::skybox = loadSkybox();
	texture::earth = Core::LoadTexture("textures/planets/8k_earth_daymap.jpg");
	texture::ship = Core::LoadTexture("textures/spaceship/spaceship.png");
	texture::moon = Core::LoadTexture("textures/planets/8k_moon.jpg");
	texture::rust = Core::LoadTexture("textures/spaceship/rust.png");
	texture::earthNormal = Core::LoadTexture("textures/planets/earth2_normals.png");
	texture::shipNormal = Core::LoadTexture("textures/spaceship/spaceship_normal.jpg");
	texture::moonNormal = Core::LoadTexture("textures/planets/moon_normal.jpg");
	texture::grid = Core::LoadTexture("textures/grid.png");
	texture::asteroidNormal = Core::LoadTexture("textures/spaceship/scratches.png");
	texture::clouds = Core::LoadTexture("textures/planets/8k_earth_clouds.jpg");
	texture::sun = Core::LoadTexture("textures/planets/8k_sun.jpg");
	texture::rustNormal = Core::LoadTexture("textures/spaceship/rust_normal.jpg");
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