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




	GLuint ship;
	GLuint rust;

	GLuint shipRepair;
	GLuint shipRepairNormal;
	GLuint engineDetail;
	GLuint engineDetailNormal;
	GLuint firstAidKit;
	GLuint firstAidKitNormal;
	GLuint crewMember;
	GLuint crewMemberNormal;


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

Core::Shader_Loader shaderLoader;

Core::RenderContext shipContext;
Core::RenderContext sphereContext;
Core::RenderContext cubeMapContex;
Core::RenderContext spaceshipEngineDetailContext;
Core::RenderContext shipToRepairContext;
Core::RenderContext firstAidBoxContext;
Core::RenderContext crewMemberContext;


glm::vec3 cameraPos = glm::vec3(-4.f, 0, 0);
glm::vec3 cameraDir = glm::vec3(1.f, 0.f, 0.f);

glm::vec3 spaceshipPos = glm::vec3(-4.f, 0, 0);
glm::vec3 spaceshipDir = glm::vec3(1.f, 0.f, 0.f);
GLuint VAO, VBO;

bool isShifWtPressed=false;
bool isPKeyPressed = false;
bool canPickUpEngine = false;
bool canPickUpKit = false;
bool canPickUpCrew = false;
bool isRKeyPressed = false;
bool drawSpaceshipEngine = true;
bool drawSpaceshipKit = true;
bool drawSpaceshipCrew = true;


bool isDragging = false;

bool isObjectPickedUp=false;
bool isKitPickedUp =false;
bool isCrewPickedUp =false;

bool isEngineCloseToShipRepair = false;
bool isKitCloseToShipRepair = false;
bool isCrewCloseToShipRepair = false;



float aspectRatio = 1.f;

float lastFrameTime = 0.0f;
float deltaTime = 0.0f;

glm::mat4 engineDetailMatrix;
glm::mat4 shipRepairMatrix;
glm::mat4 shipMatrix;
glm::mat4 kitMatrix;
glm::mat4 crewMemberMatrix;



glm::mat4 createCameraMatrix()
{
	glm::vec3 cameraSide = glm::normalize(glm::cross(cameraDir, glm::vec3(0.f, 1.f, 0.f)));
	glm::vec3 cameraUp = glm::normalize(glm::cross(cameraSide, cameraDir));
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
		0.,0.,(f + n) / (n - f),2 * f * n / (n - f),
		0.,0.,-1.,0.,
		});


	perspectiveMatrix = glm::transpose(perspectiveMatrix);

	return perspectiveMatrix;
}

void drawObjectColor(Core::RenderContext& context, glm::mat4 modelMatrix, glm::vec3 color) {

	//glUseProgram(program);
	//glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
	//glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
	//glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);
	//glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
	//glUniform3f(glGetUniformLocation(program, "color"), color.x, color.y, color.z);
	//glUniform3f(glGetUniformLocation(program, "lightPos"), 0, 0, 0);
	//Core::DrawContext(context);
	std::cout << "debug" << std::endl;

}
void drawObjectTexture(GLuint program, Core::RenderContext& context, glm::mat4 modelMatrix, GLuint textureID, GLuint normalmapId) {
	glUseProgram(program);
	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
	glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	if (textureID == texture::sun) {
		Core::SetActiveTexture(texture::sun, "tex", program, 0);
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




	/*glm::mat4 sunMatrix = glm::mat4();
	glm::mat4 earthMatrix = glm::eulerAngleY(time / 3) * glm::translate(glm::vec3(10.f, 0, 0));
	glm::mat4 moonMatrix = glm::eulerAngleY(time / 3) * glm::translate(glm::vec3(10.f, 0, 0)) * glm::eulerAngleY(time) * glm::translate(glm::vec3(3.f, 0, 0));
	glm::mat4 marsMatrix = glm::eulerAngleY((time + 6) / 3) * glm::translate(glm::vec3(15.f, 0, 0));
	glm::mat4 aliensPlanetMatrix = glm::eulerAngleY(time / 3.3f) * glm::translate(glm::vec3(20.f, 0, 0));
	glm::mat4 venusMatrix = glm::eulerAngleY(time / 4) * glm::translate(glm::vec3(25.f, 0, 0));
	glm::mat4 haumeaMatrix = glm::eulerAngleY(time / 5) * glm::translate(glm::vec3(30.f, 0, 0));
	glm::mat4 mercuryMatrix = glm::eulerAngleY(time / 7) * glm::translate(glm::vec3(35.f, 0, 0));
	*/
	 engineDetailMatrix = glm::eulerAngleX(time / 10) * glm::eulerAngleY(time / 10) * glm::translate(glm::vec3(17.5f, 0, 0));
	 shipRepairMatrix = glm::translate(glm::vec3(27.5f, 4, 0));

	//ship
	glm::vec3 spaceshipSide = glm::normalize(glm::cross(spaceshipDir, glm::vec3(0.f, 1.f, 0.f)));
	glm::vec3 spaceshipUp = glm::normalize(glm::cross(spaceshipSide, spaceshipDir));
	glm::mat4 specshipCameraRotrationMatrix = glm::mat4({
		spaceshipSide.x,spaceshipSide.y,spaceshipSide.z,0,
		spaceshipUp.x,spaceshipUp.y,spaceshipUp.z ,0,
		-spaceshipDir.x,-spaceshipDir.y,-spaceshipDir.z,0,
		0.,0.,0.,1.,
		});
	shipMatrix = glm::translate(spaceshipPos) * specshipCameraRotrationMatrix * glm::eulerAngleY(glm::pi<float>());
	//end ship


	
	


	drawObjectTexture(programSun, sphereContext, glm::mat4() * glm::scale(glm::vec3(4.f)), texture::sun, texture::sun);
	//earth
	drawObjectTexture(programEarth, sphereContext, glm::eulerAngleY(time / 3) * glm::translate(glm::vec3(10.f, 0, 0)) * glm::scale(glm::vec3(1.8f)), texture::earth, texture::earthNormal);
	//moon
	drawObjectTexture(programTex, sphereContext,
		glm::eulerAngleY(time / 3) * glm::translate(glm::vec3(10.f, 0, 0)) * glm::eulerAngleY(time) * glm::translate(glm::vec3(3.f, 0, 0)) * glm::scale(glm::vec3(0.6f)), texture::moon, texture::moonNormal);
	//mars
	drawObjectTexture(programTex, sphereContext, glm::eulerAngleY((time + 6) / 3) * glm::translate(glm::vec3(15.f, 0, 0)) * glm::scale(glm::vec3(0.7f)), texture::mars, texture::marsNormal);
	//aliens planet
	drawObjectTexture(programEarth, sphereContext, glm::eulerAngleY(time / 3.3f) * glm::translate(glm::vec3(20.f, 0, 0)) * glm::scale(glm::vec3(1.5f)), texture::aliensPlanet, texture::aliensPlanetNormal);
	//venus
	drawObjectTexture(programTex, sphereContext, glm::eulerAngleY(time / 4) * glm::translate(glm::vec3(25.f, 0, 0)) * glm::scale(glm::vec3(0.8f)), texture::venus, texture::venusNormal);
	//haumea
	drawObjectTexture(programTex, sphereContext, glm::eulerAngleY(time / 5) * glm::translate(glm::vec3(30.f, 0, 0)) * glm::scale(glm::vec3(2.f)), texture::haumea, texture::haumeaNormal);
	//mercury
	drawObjectTexture(programTex, sphereContext, glm::eulerAngleY(time / 7) * glm::translate(glm::vec3(35.f, 0, 0)) * glm::scale(glm::vec3(0.79f)), texture::mercury, texture::mercuryNormal);


	//ship to repair
	drawObjectTexture(programTex, shipToRepairContext, shipRepairMatrix * glm::scale(glm::vec3(0.1f)), texture::shipRepair, texture::shipRepairNormal);

	
	//ship
	drawObjectTexture(programShip, shipContext,
		shipMatrix* glm::scale(glm::vec3(0.04f)),
		texture::ship, texture::shipNormal
	);




	//together for every detail

	glm::vec3 shipRepairTranslationVec = glm::vec3(shipRepairMatrix[3]);
	glm::vec3 shipTranslationVec = glm::vec3(shipMatrix[3]);
	glm::vec3 spaceshipPosCOPY = spaceshipPos;
	spaceshipPosCOPY.y -= 0.7;
	

	//engine detail drag
	glm::vec3 engineDetailTranslationVec = glm::vec3(engineDetailMatrix[3]);
		if (glm::distance(shipTranslationVec, engineDetailTranslationVec) < 1.0f && !isDragging)
		{
			canPickUpEngine = true;
		}
	if (isPKeyPressed && canPickUpEngine) {
		isObjectPickedUp = true;
		isDragging = true;

		engineDetailMatrix = glm::translate(spaceshipPosCOPY);
		float dist = glm::distance(shipTranslationVec, shipRepairTranslationVec);

		if (glm::distance(shipTranslationVec, shipRepairTranslationVec) < 3.f) {
			isEngineCloseToShipRepair = true;
		}
		else {
			isEngineCloseToShipRepair = false;
		}
		//isObjectPickedUp - about engine
		if (isRKeyPressed && isObjectPickedUp && isEngineCloseToShipRepair) {
			drawSpaceshipEngine = false;

			isPKeyPressed = false;
			isRKeyPressed = false;

			canPickUpEngine = false;
			isObjectPickedUp = false;

			isDragging = false;


		}
	}
	if (drawSpaceshipEngine) {
		drawObjectTexture(programTex, spaceshipEngineDetailContext, engineDetailMatrix * glm::scale(glm::vec3(0.15f)), texture::engineDetail, texture::engineDetailNormal);
	}


	//start kit
	kitMatrix = glm::eulerAngleX(time / 12.f) * glm::translate(glm::vec3(12.7f, 5.f, 0)) * glm::eulerAngleY(time / 12.f);
	glm::vec3 kitDetailTranslationVec = glm::vec3(kitMatrix[3]);
	if (glm::distance(shipTranslationVec, kitDetailTranslationVec) < 1.0f && !isDragging)
	{
		canPickUpKit = true;
	}
	if (isPKeyPressed && canPickUpKit) {
		isKitPickedUp = true;
		isDragging = true;
		kitMatrix = glm::translate(spaceshipPosCOPY);
		if (glm::distance(shipTranslationVec, shipRepairTranslationVec) < 3.f) {
			isKitCloseToShipRepair = true;
		}
		else {
			isKitCloseToShipRepair = false;
		}
		if (isRKeyPressed ) {
			drawSpaceshipKit = false;
			isPKeyPressed = false;
			isRKeyPressed = false;

			canPickUpKit = false;
			isKitPickedUp = false;
			isDragging = false;


		}
	}
	if (drawSpaceshipKit) {
		drawObjectTexture(programTex, firstAidBoxContext, kitMatrix * glm::scale(glm::vec3(0.06f)), texture::firstAidKit, texture::firstAidKitNormal);
	}
	



	//crew member
	crewMemberMatrix = glm::eulerAngleX(time / 15) * glm::translate(glm::vec3(3.5f, -7.f, 0)) * glm::eulerAngleY(time / 15);
	glm::vec3 crewDetailTranslationVec = glm::vec3(crewMemberMatrix[3]);
	if (glm::distance(shipTranslationVec, crewDetailTranslationVec) < 1.0f&& !isDragging)
	{
		canPickUpCrew = true;
		
	}
	if (isPKeyPressed && canPickUpCrew) {
		isCrewPickedUp = true;
		isDragging = true;
		crewMemberMatrix = glm::translate(spaceshipPosCOPY);
		if (glm::distance(shipTranslationVec, shipRepairTranslationVec) < 3.f && !isObjectPickedUp && !isKitPickedUp) {
			isCrewCloseToShipRepair = true;
		}
		else {
			isCrewCloseToShipRepair = false;
		}
		if (isRKeyPressed) {
			drawSpaceshipCrew = false;
			isPKeyPressed = false;
			isRKeyPressed = false;

			canPickUpCrew = false;
			isCrewPickedUp = false;
			isDragging = false;

		}
	}
	if (drawSpaceshipCrew) {
		drawObjectTexture(programTex, crewMemberContext, crewMemberMatrix*glm::scale(glm::vec3(0.05f)), texture::crewMember, texture::crewMemberNormal);
	}


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
	const aiScene * scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}
	context.initFromAssimpMesh(scene->mMeshes[0]);
}

void init(GLFWwindow* window)
{

	glGenTextures(1, &texture::cubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture::cubemap);
	int w, h;
	unsigned char* data;
	std::vector<std::string> filepaths = {
	 /*"./textures/skybox/lightblue/right.png",
	  "./textures/skybox/lightblue/left.png",
	  "./textures/skybox/lightblue/top.png",
	  "./textures/skybox/lightblue/bot.png",
	  "./textures/skybox/lightblue/front.png",
	  "./textures/skybox/lightblue/back.png",*/

		"./textures/skybox/red/bkg1_right1.png",
    "./textures/skybox/red/bkg1_left2.png",
    "./textures/skybox/red/bkg1_top3.png",
    "./textures/skybox/red/bkg1_bottom4.png",
    "./textures/skybox/red/bkg1_front5.png",
    "./textures/skybox/red/bkg1_back6.png",
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
	loadModelToContext("./models/SciFi_Fighter.obj", shipContext);
	//loadModelToContext("./models/SciFi_Fighter.obj", shipContext);
	loadModelToContext("./models/cube.obj", cubeMapContex);

	loadModelToContext("./models/airplane-engine-detail.obj", spaceshipEngineDetailContext);
	loadModelToContext("./models/UFO.obj", shipToRepairContext);
	loadModelToContext("./models/Firstaidbox.obj", firstAidBoxContext);
	loadModelToContext("./models/crew_member.obj", crewMemberContext);




	//new objects



	//texture::skybox = loadSkybox();

	texture::sun = Core::LoadTexture("textures/planets/8k_sun.jpg");

	texture::earth = Core::LoadTexture("textures/planets/8k_earth_daymap.jpg");
	texture::clouds = Core::LoadTexture("textures/planets/8k_earth_clouds.jpg");
	texture::earthNormal = Core::LoadTexture("textures/planets/earth2_normals.png");

	texture::ship = Core::LoadTexture("textures/spaceship/spaceship.jpg");
	texture::shipNormal = Core::LoadTexture("textures/spaceship/SF_Fighter_Normal.jpg");
	texture::rust = Core::LoadTexture("textures/spaceship/rust.png");
	texture::rustNormal = Core::LoadTexture("textures/spaceship/rust_normal.jpg");
	texture::shipScratches = Core::LoadTexture("textures/spaceship/scratches.png");


	texture::shipRepair = Core::LoadTexture("textures/spaceship/ufo_main_ship_to_repair.png");
	texture::shipRepairNormal = Core::LoadTexture("textures/spaceship/ufo_normal.png");

	texture::engineDetail = Core::LoadTexture("textures/details/airplane_engine_BaseColor.png");
	texture::engineDetailNormal = Core::LoadTexture("textures/details/airplane_engine_Normal.png");

	texture::firstAidKit = Core::LoadTexture("textures/details/first_aid_box_base.png");
	texture::firstAidKitNormal = Core::LoadTexture("textures/details/first_aid_box_base_normal.png");
	
	texture::crewMember = Core::LoadTexture("textures/details/Alien_BaseColor.png");
	texture::crewMemberNormal = Core::LoadTexture("textures/details/Alien_Normal.png");



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


	if ((glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) && (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS))
	{
		spaceshipPos += spaceshipDir * moveSpeed * 5;
		isShifWtPressed = true;
	}
	


	if ((glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)&&(canPickUpEngine||canPickUpKit||canPickUpCrew))
	{
		isPKeyPressed = true;
		//std::cout << "iside input: P    canPickUpEngine " << canPickUpEngine << "	canPickUpKit " << canPickUpKit<< "	canPickUpCrew " << canPickUpCrew << std::endl;
	}
	
	if ((glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) && (isKitPickedUp||isObjectPickedUp||isCrewPickedUp) && (isKitCloseToShipRepair || isEngineCloseToShipRepair||isCrewCloseToShipRepair))
	{
		isRKeyPressed = true;
		//std::cout << "iside input: R   " << isRKeyPressed << std::endl;

	}
	

	cameraPos = spaceshipPos - 1.5 * spaceshipDir + glm::vec3(0, 1, 0) * 0.5f;
	cameraDir = spaceshipDir;

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