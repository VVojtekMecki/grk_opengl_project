#include "space_objects.h"
#include <map>
#include <GLFW/glfw3.h>

class SpaceObjectProperties {
public:
	std::string name;
	SpaceObject* object;
	glm::mat4 viewProjectionMatrix;

	SpaceObjectProperties(std::string name, SpaceObject* obj)
		: name(name), object(obj) {}

};

class SpaceObjectsList {
private:
	GLuint emptyTexture;
	GLuint emptyNormals;

	GLuint earthTexture;
	GLuint earthNormalTexture;
	GLuint earthCloudsTexture;

	GLuint mercuryTexture;
	GLuint mercuryNormalTexture;

	GLuint aliensPlanetTexture;
	GLuint aliensPlanetNormalTexture;
	GLuint aliensPlanetCloudsTexture;

	GLuint venusTexture;
	GLuint venusNormalTexture;

	GLuint marsTexture;
	GLuint marsNormalTexture;

	GLuint jupiterTexture;
	GLuint jupiterNormalTexture;

	GLuint haumeaTexture;
	GLuint haumeaNormalTexture;

	GLuint shipTexture;
	GLuint shipNormalTexture;

	GLuint moonTexture;
	GLuint moonNormalTexture;


	GLuint programPBREarth;
	GLuint programCloudsAnimation;
	GLuint programPlanetAnimatedClouds;

	Core::Shader_Loader shaderLoader;
	Core::RenderContext sphereContext;

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
	float time;

	glm::mat4 viewProjectionMatrix;
	std::vector<SpaceObjectProperties> spaceObjectsList;

	SpaceObjectsList(float time, glm::mat4 viewProjectionMatrix) : time(time), viewProjectionMatrix(viewProjectionMatrix) {}

	void init() {
		programPBREarth = shaderLoader.CreateProgram("shaders/PBRforEarth.vert", "shaders/PBRforEarth.frag");
		programCloudsAnimation = shaderLoader.CreateProgram("shaders/clouds_noise.vert", "shaders/clouds_noise.frag");
		programPlanetAnimatedClouds = shaderLoader.CreateProgram("shaders/PlanetWithAnimatedClouds.vert", "shaders/PlanetWithAnimatedClouds.frag");


		loadModelToContext("./models/sphere.obj", sphereContext);


		earthTexture = Core::LoadTexture("textures/planets/green_planet.png");
		earthCloudsTexture = Core::LoadTexture("textures/planets/8k_earth_clouds.jpg");
		earthNormalTexture = Core::LoadTexture("textures/planets/earth2_normals.png");

		shipTexture = Core::LoadTexture("textures/spaceship/spaceship.jpg");
		shipNormalTexture = Core::LoadTexture("textures/spaceship/SF_Fighter_Normal.jpg");

		moonTexture = Core::LoadTexture("textures/planets/8k_moon.jpg");
		moonNormalTexture = Core::LoadTexture("textures/planets/moon_normal.jpg");

		mercuryTexture = Core::LoadTexture("textures/planets/mercury.png");
		mercuryNormalTexture = Core::LoadTexture("textures/planets/mercury_normal.jpg");

		aliensPlanetTexture = Core::LoadTexture("textures/planets/aliensPlanet.png");
		aliensPlanetNormalTexture = Core::LoadTexture("textures/planets/aliensPlanet_normal.jpg");
		aliensPlanetCloudsTexture = Core::LoadTexture("textures/planets/aliensPlanet_clouds.jpg");

		venusTexture = Core::LoadTexture("textures/planets/venus.jpg");
		venusNormalTexture = Core::LoadTexture("textures/planets/venus_normal.jpg");

		marsTexture = Core::LoadTexture("textures/planets/mars.jpg");
		marsNormalTexture = Core::LoadTexture("textures/planets/mars_normal.jpg");

		jupiterTexture = Core::LoadTexture("textures/planets/jupiter.jpg");
		jupiterNormalTexture = Core::LoadTexture("textures/planets/jupiter_normal.jpg");

		haumeaTexture = Core::LoadTexture("textures/planets/haumea.jpg");
		haumeaNormalTexture = Core::LoadTexture("textures/planets/haumea_normal.jpg");


		//CloudedPlanet* earth = new CloudedPlanet("earth", programPBREarth, sphereContext, SpaceObjectsList::earthTexture, SpaceObjectsList::earthNormalTexture, SpaceObjectsList::earthCloudsTexture);
		CloudsAnimatedPlanet* earth = new CloudsAnimatedPlanet("earth", programPlanetAnimatedClouds, sphereContext, SpaceObjectsList::earthTexture, SpaceObjectsList::earthNormalTexture);
		spaceObjectsList.push_back(SpaceObjectProperties("earth", earth));

		CloudedPlanet* moon = new CloudedPlanet("moon", programPBREarth, sphereContext, SpaceObjectsList::moonTexture, SpaceObjectsList::moonNormalTexture, SpaceObjectsList::moonTexture);
		spaceObjectsList.push_back(SpaceObjectProperties("moon", moon));

		CloudedPlanet* secondMoon = new CloudedPlanet("secondMoon", programPBREarth, sphereContext, SpaceObjectsList::moonTexture, SpaceObjectsList::moonNormalTexture, SpaceObjectsList::moonTexture);
		spaceObjectsList.push_back(SpaceObjectProperties("secondMoon", secondMoon));

		CloudedPlanet* mars = new CloudedPlanet("mars", programPBREarth, sphereContext, SpaceObjectsList::marsTexture, SpaceObjectsList::marsNormalTexture, SpaceObjectsList::marsTexture);
		spaceObjectsList.push_back(SpaceObjectProperties("mars", mars));

		CloudsAnimatedPlanet* aliensPlanet = new CloudsAnimatedPlanet("aliensPlanet", programPBREarth, sphereContext, SpaceObjectsList::aliensPlanetTexture, SpaceObjectsList::aliensPlanetNormalTexture);
		spaceObjectsList.push_back(SpaceObjectProperties("aliensPlanet", aliensPlanet));

		CloudedPlanet* venus = new CloudedPlanet("venus", programPBREarth, sphereContext, SpaceObjectsList::venusTexture, SpaceObjectsList::venusNormalTexture, SpaceObjectsList::venusTexture);
		spaceObjectsList.push_back(SpaceObjectProperties("venus", venus));

		CloudedPlanet* humea = new CloudedPlanet("humea", programPBREarth, sphereContext, SpaceObjectsList::haumeaTexture, SpaceObjectsList::haumeaNormalTexture, SpaceObjectsList::haumeaTexture);
		spaceObjectsList.push_back(SpaceObjectProperties("humea", humea));

		CloudedPlanet* mercury = new CloudedPlanet("mercury", programPBREarth, sphereContext, SpaceObjectsList::mercuryTexture, SpaceObjectsList::mercuryNormalTexture, SpaceObjectsList::mercuryTexture);
		spaceObjectsList.push_back(SpaceObjectProperties("mercury", mercury));

	}

	void updateTime() {
		this->time = glfwGetTime();
	}
};