#include "glew.h"
#include <GLFW/glfw3.h>
#include "glm.hpp"
#include "ext.hpp"
#include <iostream>
#include <cmath>

#include "Shader_Loader.h"
#include "Render_Utils.h"
#include "Texture.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>
#include <..\..\cw 7\src\SOIL\SOIL.h>
#include <vector>
#include "objects/planets_list.cpp"
#include "objects/player_ship.cpp"
#include "objects/skybox.h"
#include "objects/asteroids_list.cpp"
#include "objects/details_list.cpp"
#include "objects/shipToRepairHeader.h"
//#include "../src/objects/Font.h"


#include "Box.cpp"


//GLuint quadVAO;

GLuint program;
GLuint programTex;
GLuint programTextDisplay;


Core::Shader_Loader shaderLoader;

glm::vec3 cameraPos = glm::vec3(-4.f, 0, 0);
glm::vec3 cameraDir = glm::vec3(1.f, 0.f, 1.f);

glm::mat4 shipModelMatrix;

glm::vec3 spaceshipPos = glm::vec3(12.f, 0, 0);
glm::vec3 spaceshipDir = glm::vec3(1.f, 0.f, 0.f);

GLuint instructionsTexture;

GLuint instructionsBoardTexture;
GLuint instructionsBoardTexture0;
GLuint instructionsBoardTexture1;
GLuint instructionsBoardTexture2;
GLuint instructionsBoardTexture3;
GLuint instructionsBoardTextureCongrats;

GLuint instructionsBoardNormal;

GLuint amountOfPickedItemsPaths[] = { instructionsBoardTexture0, instructionsBoardTexture1,instructionsBoardTexture2, instructionsBoardTexture3 };
//int amountOfPickedItemsTab = 0;
namespace detailsTextures {
	GLuint engineDetailTexture;
	GLuint engineDetailNormal;
	GLuint firstAidKit;
	GLuint firstAidKitNormal;
	GLuint crewMember;
	GLuint crewMemberNormal;

};


Core::RenderContext spaceshipEngineDetailContext;
Core::RenderContext firstAidBoxContext;
Core::RenderContext crewMemberContext;
Core::RenderContext instructionsBoardContext;

bool isPKeyPressed = false;
bool isRKeyPressed = false;
bool isIKeyPressed = false;
bool isTabKeyPressed = false;

bool canPickUpEngine = false;
bool canPickUpKit = false;

bool canPickUpCrew = false;

bool drawSpaceshipEngine = true;
bool drawSpaceshipKit = true;
bool drawSpaceshipCrew = true;
bool drawInstructions = true;
bool drawCongratulations = true;

bool isDragging = false;

bool isEnginePickedUp = false;
bool isKitPickedUp = false;
bool isCrewPickedUp = false;

bool isEngineCloseToShipRepair = false;
bool isKitCloseToShipRepair = false;
bool isCrewCloseToShipRepair = false;


glm::mat4 engineDetailMatrix;
glm::mat4 kitMatrix;
glm::mat4 crewMemberMatrix;

std::map<std::string, glm::mat4> detailsMatrixMap;


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

SpaceObjectsList spaceObjectsList(glfwGetTime(), createPerspectiveMatrix()* createCameraMatrix());

DetailsList detailslist(glfwGetTime(), createPerspectiveMatrix()* createCameraMatrix());

ShipToRepair shipToRepair;
//Details drawDetail;
AsteroidsList asteroidsList;

PlayerShip player;

Skybox skybox;


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


void renderScene(GLFWwindow* window)
{



	glClearColor(0.0f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 transformation;
	float timeGl = glfwGetTime();
	deltaTime = timeGl - lastFrameTime;
	deltaTime = glm::min(deltaTime, 0.1f);
	lastFrameTime = timeGl;
	


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

	for (Asteroid* obj : asteroidsList.objectsList) {
		glm::mat4 asteroid1Scale = glm::scale(glm::vec3(0.5f));
		glm::mat4 asteroid1Rotate = glm::rotate(timeGl * 0.05f, glm::vec3(0, 1, 0));
		auto pos = obj->getPosition();
		glm::mat4 asteroid1Translate = glm::translate(glm::vec3(pos.x, pos.y, pos.z));
		obj->drawObjectTexture(projectionMatrix, asteroid1Scale*asteroid1Rotate * asteroid1Translate);
	}


	glm::vec3 spaceshipSide = glm::normalize(glm::cross(spaceshipDir, glm::vec3(0.f, 1.f, 0.f)));
	glm::vec3 spaceshipUp = glm::normalize(glm::cross(spaceshipSide, spaceshipDir));
	glm::mat4 specshipCameraRotrationMatrix = glm::mat4({
		spaceshipSide.x,spaceshipSide.y,spaceshipSide.z,0,
		spaceshipUp.x,spaceshipUp.y,spaceshipUp.z ,0,
		-spaceshipDir.x,-spaceshipDir.y,-spaceshipDir.z,0,
		0.,0.,0.,1.,
		});

	shipModelMatrix = glm::translate(spaceshipPos) * specshipCameraRotrationMatrix * glm::eulerAngleY(glm::pi<float>()) * glm::scale(glm::vec3(0.04f));

	player.ship->drawObjectTexture(projectionMatrix, shipModelMatrix);




	std::map<std::string, glm::mat4> shipRepairMatrixMap = {
	  {"shipToRepair",glm::translate(glm::vec3(-27.5f, 4, 0)) * glm::scale(glm::vec3(0.15f))}
	};

	shipToRepair.repair->drawObjectTexture(projectionMatrix, shipRepairMatrixMap.at("shipToRepair"));

	

	//together for every detail

	glm::vec3 shipRepairTranslationVec = glm::vec3(shipRepairMatrixMap.at("shipToRepair")[3]);
	glm::vec3 shipTranslationVec = glm::vec3(shipModelMatrix[3]);


	engineDetailMatrix = glm::eulerAngleX(timeGl / 10) * glm::eulerAngleY(timeGl / 10) * glm::translate(glm::vec3(17.5f, 0, 0));

	//engine detail drag
	glm::vec3 engineDetailTranslationVec = glm::vec3(engineDetailMatrix[3]);

	glm::vec3 spaceshipPosCOPY = spaceshipPos;
	spaceshipPosCOPY.y -= 0.3;
	if (glm::distance(shipTranslationVec, engineDetailTranslationVec) < 1.0f && !isDragging)
	{
		canPickUpEngine = true;
	}
	if (isPKeyPressed && canPickUpEngine) {
		isEnginePickedUp = true;
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
		if (isRKeyPressed && isEnginePickedUp && isEngineCloseToShipRepair) {
			drawSpaceshipEngine = false;

			isPKeyPressed = false;
			isRKeyPressed = false;

			canPickUpEngine = false;
			isEnginePickedUp = false;

			isDragging = false;
			shipToRepair.repair->amountOfPickedUpObjects += 1;


		}
	}
	if (drawSpaceshipEngine) {
		drawObjectTexture(programTex, spaceshipEngineDetailContext, engineDetailMatrix * glm::scale(glm::vec3(0.15f)), detailsTextures::engineDetailTexture, detailsTextures::engineDetailNormal);
		//player.ship->amountOfPickedUpObjects += 1;
	}

	spaceshipPosCOPY = spaceshipPos;
	spaceshipPosCOPY.y -= 0.3;

	//start kit
	kitMatrix = glm::eulerAngleX(timeGl / 12.f) * glm::translate(glm::vec3(-12.7f, 5.f, 0)) * glm::eulerAngleY(timeGl / 12.f);
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
		if (isRKeyPressed) {
			drawSpaceshipKit = false;
			isPKeyPressed = false;
			isRKeyPressed = false;

			canPickUpKit = false;
			isKitPickedUp = false;
			isDragging = false;
			shipToRepair.repair->amountOfPickedUpObjects += 1;


		}
	}
	if (drawSpaceshipKit) {
		drawObjectTexture(programTex, firstAidBoxContext, kitMatrix * glm::scale(glm::vec3(0.06f)), detailsTextures::firstAidKit, detailsTextures::firstAidKitNormal);
	}



	spaceshipPosCOPY = spaceshipPos;
	spaceshipPosCOPY.y -= 0.6;
	//crew member
	crewMemberMatrix = glm::eulerAngleX(timeGl / 15) * glm::translate(glm::vec3(3.5f, -7.f, 0)) * glm::eulerAngleY(timeGl / 15);
	glm::vec3 crewDetailTranslationVec = glm::vec3(crewMemberMatrix[3]);
	if (glm::distance(shipTranslationVec, crewDetailTranslationVec) < 1.0f && !isDragging)
	{
		canPickUpCrew = true;

	}
	if (isPKeyPressed && canPickUpCrew) {
		isCrewPickedUp = true;
		isDragging = true;
		crewMemberMatrix = glm::translate(spaceshipPosCOPY);
		if (glm::distance(shipTranslationVec, shipRepairTranslationVec) < 3.f && !isEnginePickedUp && !isKitPickedUp) {
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
			shipToRepair.repair->amountOfPickedUpObjects += 1;
		}
	}
	if (drawSpaceshipCrew) {
		drawObjectTexture(programTex, crewMemberContext, crewMemberMatrix * glm::scale(glm::vec3(0.05f)), detailsTextures::crewMember, detailsTextures::crewMemberNormal);
		

	}


	if (shipToRepair.repair->amountOfPickedUpObjects == 3) {
		shipToRepair.repair->indexNormal = 1;
		drawObjectTexture(programTex, instructionsBoardContext, glm::rotate(glm::translate(glm::vec3(0.f, 10.f, 0)), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f))* glm::rotate(glm::translate(glm::vec3(0.f, 0, 0)), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f))* glm::scale(glm::vec3(2.f)), instructionsBoardTextureCongrats, instructionsBoardNormal);


	}

	glm::mat4 blackboardRotationMatrix;
	glm::vec3 blackboardRotationTranslVec = blackboardRotationMatrix[3];
	if ((glm::distance(shipTranslationVec, blackboardRotationTranslVec) < 3.0f)|| isIKeyPressed) {
		drawInstructions = false;
	}
	if (drawInstructions) {
		glm::mat4 blackboardRotationY = glm::rotate(glm::translate(glm::vec3(14.f, -0.5f, 0)), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 blackboardRotationX = glm::rotate(glm::translate(glm::vec3(0.f, 0, 0)), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		 blackboardRotationMatrix = blackboardRotationY * blackboardRotationX;

		drawObjectTexture(programTex, instructionsBoardContext, blackboardRotationMatrix * glm::scale(glm::vec3(1.4f)), instructionsBoardTexture, instructionsBoardNormal);
	}

	if (isTabKeyPressed) {
		drawObjectTexture(programTex, instructionsBoardContext, glm::rotate(glm::translate(glm::vec3(0.f, 10.f, 0)), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::rotate(glm::translate(glm::vec3(0.f, 0, 0)), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) * glm::scale(glm::vec3(2.f)), amountOfPickedItemsPaths[shipToRepair.repair->amountOfPickedUpObjects], instructionsBoardNormal);

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
	//initSquare();
	//instructionsTexture= Core::LoadTexture("textures/grid.png");
	
	////////////////////////////

	

	skybox.init();

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glEnable(GL_DEPTH_TEST);
	program = shaderLoader.CreateProgram("shaders/shader_5_1.vert", "shaders/shader_5_1.frag");
	programTex = shaderLoader.CreateProgram("shaders/shader_5_1_tex_copy.vert", "shaders/shader_5_1_tex_copy.frag");

	programTextDisplay = shaderLoader.CreateProgram("shaders/textShader.vert", "shaders/textShader.frag");
	
	detailsTextures::engineDetailTexture = Core::LoadTexture("textures/details/airplane_engine_BaseColor.png");
	detailsTextures::engineDetailNormal = Core::LoadTexture("textures/details/airplane_engine_Normal.png");

	detailsTextures::firstAidKit = Core::LoadTexture("textures/details/first_aid_box_base.png");
	detailsTextures::firstAidKitNormal = Core::LoadTexture("textures/details/first_aid_box_base_normal.png");

	detailsTextures::crewMember = Core::LoadTexture("textures/details/Alien_BaseColor.png");
	detailsTextures::crewMemberNormal = Core::LoadTexture("textures/details/Alien_Normal.png");

	instructionsBoardTexture = Core::LoadTexture("textures/text/BlackboardBake.png");

	instructionsBoardTexture0 = Core::LoadTexture("textures/text/Blackboard0of3.png");
	instructionsBoardTexture1 = Core::LoadTexture("textures/text/Blackboard1of3.png");
	instructionsBoardTexture2 = Core::LoadTexture("textures/text/Blackboard2of3.png");
	instructionsBoardTexture3 = Core::LoadTexture("textures/text/Blackboard3of3.png");
	
	instructionsBoardTextureCongrats = Core::LoadTexture("textures/text/Blackboard3of3Destroy.png");

	instructionsBoardNormal = Core::LoadTexture("textures/text/BlackboardNormal.png");


	loadModelToContext("./models/airplane-engine-detail.obj", spaceshipEngineDetailContext);
	loadModelToContext("./models/Firstaidbox.obj", firstAidBoxContext);
	loadModelToContext("./models/crew_member.obj", crewMemberContext);

	loadModelToContext("./models/Chalkboard.fbx", instructionsBoardContext);

	spaceObjectsList.init();
	asteroidsList.init();
	player.init();
	shipToRepair.init();
	
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


	if ((glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) && (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS))
	{
		spaceshipPos += spaceshipDir * moveSpeed * 5;
	}


	if ((glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) && (canPickUpEngine || canPickUpKit || canPickUpCrew))
	{
		isPKeyPressed = true;
	}

	if ((glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) && (isKitPickedUp || isEnginePickedUp || isCrewPickedUp) && (isKitCloseToShipRepair || isEngineCloseToShipRepair || isCrewCloseToShipRepair))
	{
		isRKeyPressed = true;

	}

	if ((glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) )
	{
		isIKeyPressed = true;

	}
	if ((glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS))
	{
		isTabKeyPressed = true;

	}else 
		isTabKeyPressed = false;


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