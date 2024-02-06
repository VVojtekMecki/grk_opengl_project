#include <stdlib.h>
#include <time.h>
#include <glew.h>
#include "glm.hpp"
#include "glew.h"
#include <GLFW/glfw3.h>
#include "ext.hpp"
#include "space_objects.h"

class AsteroidsList {

private:
	int amountAsteroids = 400;
	int positionX, positionY, positionZ = 0;
	
	GLuint asteroidGreyTexture;
	GLuint asteroidRedTexture;
	GLuint asteroidNormal;

	GLuint programTex;

	Core::Shader_Loader shaderLoader;
	Core::RenderContext asteroidContext;

	GLuint getRandomAsteroidTexture() {
		int randomIndex = std::rand() % 2;
		return (randomIndex == 0) ? asteroidGreyTexture : asteroidRedTexture;

	}

	int getRandomNumber(int min, int max) {
		int randomNumber = rand() % (max - min + 1) + min;
		return randomNumber;
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

public:
	std::vector<Asteroid*> objectsList;

	AsteroidsList() {}

	void init() {
		asteroidGreyTexture = Core::LoadTexture("textures/asteroids/4k_haumea_fictional.jpg");
		asteroidRedTexture = Core::LoadTexture("textures/asteroids/4k_makemake_fictional.jpg");
		asteroidNormal = Core::LoadTexture("textures/asteroids/asteroid_normal.jpg");

		programTex = shaderLoader.CreateProgram("shaders/shader_5_1_tex_copy.vert", "shaders/shader_5_1_tex_copy.frag");

		loadModelToContext("./models/asteroid.obj", asteroidContext);

		srand(static_cast<unsigned>(time(0)));
		for (int asteroid = 0; asteroid < amountAsteroids; asteroid++) {
			positionX = getRandomNumber(-65, 65);
			positionY = getRandomNumber(-35, 35);
			positionZ = getRandomNumber(-35, 35);

			bool truePositionZ = false;
			if (positionY > -16 && positionY < 16) {
				while (truePositionZ != true) {
					if (positionZ < -8 || positionZ > 8) {
						truePositionZ = true;
						break;
					}
					positionZ = getRandomNumber(-35, 35);
				}
			}

			Asteroid* object = new Asteroid(std::to_string(asteroid), programTex, asteroidContext, getRandomAsteroidTexture(), asteroidNormal, positionX, positionY, positionZ);

			objectsList.push_back(object);

		};
	}
	
};