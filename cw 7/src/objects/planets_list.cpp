#include "space_objects.cpp"
#include <map>

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

	GLuint sunTexture;

	GLuint programSun;
	GLuint programTex;
	GLuint programEarth;

	Core::Shader_Loader shaderLoader;
	Core::RenderContext shipContext;
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
		programTex = shaderLoader.CreateProgram("shaders/shader_5_1_tex_copy.vert", "shaders/shader_5_1_tex_copy.frag");
		programEarth = shaderLoader.CreateProgram("shaders/shader_earth.vert", "shaders/shader_earth.frag");
		programSun = shaderLoader.CreateProgram("shaders/shader_5_sun.vert", "shaders/shader_5_sun.frag");

		loadModelToContext("./models/sphere.obj", sphereContext);
		loadModelToContext("./models/SciFi_Fighter.obj", shipContext);

		sunTexture = Core::LoadTexture("textures/planets/8k_sun.jpg");

		earthTexture = Core::LoadTexture("textures/planets/8k_earth_daymap.jpg");
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

		Sun* sun = new Sun("sun", programSun, sphereContext, SpaceObjectsList::sunTexture);
		spaceObjectsList.push_back(SpaceObjectProperties("sun", sun));

		CloudedPlanet* earth = new CloudedPlanet("earth", programEarth, sphereContext, SpaceObjectsList::earthTexture, SpaceObjectsList::earthNormalTexture, SpaceObjectsList::earthCloudsTexture);
		spaceObjectsList.push_back(SpaceObjectProperties("earth", earth));

		Planet* moon = new Planet("moon", programTex, sphereContext, SpaceObjectsList::moonTexture, SpaceObjectsList::moonNormalTexture);
		spaceObjectsList.push_back(SpaceObjectProperties("moon", moon));

		Planet* mars = new Planet("mars", programTex, sphereContext, SpaceObjectsList::marsTexture, SpaceObjectsList::marsNormalTexture);
		spaceObjectsList.push_back(SpaceObjectProperties("mars", mars));

		CloudedPlanet* aliensPlanet = new CloudedPlanet("aliensPlanet", programEarth, sphereContext, SpaceObjectsList::aliensPlanetTexture, SpaceObjectsList::aliensPlanetNormalTexture, SpaceObjectsList::aliensPlanetCloudsTexture);
		spaceObjectsList.push_back(SpaceObjectProperties("aliensPlanet", aliensPlanet));

		Planet* venus = new Planet("venus", programTex, sphereContext, SpaceObjectsList::venusTexture, SpaceObjectsList::venusNormalTexture);
		spaceObjectsList.push_back(SpaceObjectProperties("venus", venus));

		Planet* humea = new Planet("humea", programTex, sphereContext, SpaceObjectsList::haumeaTexture, SpaceObjectsList::haumeaNormalTexture);
		spaceObjectsList.push_back(SpaceObjectProperties("humea", humea));

		Planet* mercury = new Planet("mercury", programTex, sphereContext, SpaceObjectsList::mercuryTexture, SpaceObjectsList::mercuryNormalTexture);
		spaceObjectsList.push_back(SpaceObjectProperties("mercury", mercury));
	}

	void updateTime(float newTime) {
		this->time = newTime;
	}
};