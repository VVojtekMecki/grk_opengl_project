#include "space_objects.cpp"
#include "scene.hpp"

Core::Shader_Loader shaderLoader;
Core::RenderContext shipContext;
Core::RenderContext sphereContext;

class SpaceObjectProperties {
public:
	std::string name;
	SpaceObject* object;
	glm::mat4 viewProjectionMatrix;

	SpaceObjectProperties(std::string name, SpaceObject* obj, glm::mat4 viewProjectionMatrix)
		: name(name), object(obj), viewProjectionMatrix(viewProjectionMatrix) {}

	~SpaceObjectProperties() {
		delete object;
	}
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

	GLuint program;
	GLuint programSun;
	GLuint programTex;
	GLuint programEarth;
	GLuint programProcTex;
	GLuint programShip;

public:
	float time;
	glm::mat4 viewProjectionMatrix;
	std::vector<SpaceObjectProperties> spaceObjectsList;

	SpaceObjectsList(float time, glm::mat4 viewProjectionMatrix) : time(time), viewProjectionMatrix(viewProjectionMatrix) {

		program = shaderLoader.CreateProgram("shaders/shader_5_1.vert", "shaders/shader_5_1.frag");
		programTex = shaderLoader.CreateProgram("shaders/shader_5_1_tex_copy.vert", "shaders/shader_5_1_tex_copy.frag");
		programEarth = shaderLoader.CreateProgram("shaders/shader_earth.vert", "shaders/shader_earth.frag");
		programProcTex = shaderLoader.CreateProgram("shaders/shader_5_1_tex.vert", "shaders/shader_5_1_tex.frag");
		programSun = shaderLoader.CreateProgram("shaders/shader_5_sun.vert", "shaders/shader_5_sun.frag");
		programShip = shaderLoader.CreateProgram("shaders/shader_ship.vert", "shaders/shader_ship.frag");

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

		Sun* sun = new Sun(programSun, sphereContext, glm::mat4() * glm::scale(glm::vec3(4.f)), SpaceObjectsList::sunTexture);
		spaceObjectsList.push_back(SpaceObjectProperties("Sun", sun, this->viewProjectionMatrix));

		CloudedPlanet* earth = new CloudedPlanet(programEarth, sphereContext, glm::eulerAngleY(time / 3) * glm::translate(glm::vec3(10.f, 0, 0)) * glm::scale(glm::vec3(1.8f)), SpaceObjectsList::earthTexture, SpaceObjectsList::earthNormalTexture, SpaceObjectsList::earthCloudsTexture);
		spaceObjectsList.push_back(SpaceObjectProperties("Earth", earth, this->viewProjectionMatrix));

		Planet* moon = new Planet(programTex, sphereContext,
			glm::eulerAngleY(time / 3) * glm::translate(glm::vec3(10.f, 0, 0)) * glm::eulerAngleY(time) * glm::translate(glm::vec3(3.f, 0, 0)) * glm::scale(glm::vec3(0.6f)), SpaceObjectsList::moonTexture, SpaceObjectsList::moonNormalTexture);
		spaceObjectsList.push_back(SpaceObjectProperties("Moon", moon, this->viewProjectionMatrix));

		Planet* mars = new Planet(programTex, sphereContext, glm::eulerAngleY((time + 6) / 3) * glm::translate(glm::vec3(15.f, 0, 0)) * glm::scale(glm::vec3(0.7f)), SpaceObjectsList::marsTexture, SpaceObjectsList::marsNormalTexture);
		spaceObjectsList.push_back(SpaceObjectProperties("Mars", mars, this->viewProjectionMatrix));

		CloudedPlanet* aliensPlanet = new CloudedPlanet(programEarth, sphereContext, glm::eulerAngleY(time / 3.3f) * glm::translate(glm::vec3(20.f, 0, 0)) * glm::scale(glm::vec3(1.5f)), SpaceObjectsList::aliensPlanetTexture, SpaceObjectsList::aliensPlanetNormalTexture, SpaceObjectsList::aliensPlanetCloudsTexture);
		spaceObjectsList.push_back(SpaceObjectProperties("AliensPlanet", aliensPlanet, this->viewProjectionMatrix));

		Planet* venus = new Planet(programTex, sphereContext, glm::eulerAngleY(time / 4) * glm::translate(glm::vec3(25.f, 0, 0)) * glm::scale(glm::vec3(0.8f)), SpaceObjectsList::venusTexture, SpaceObjectsList::venusNormalTexture);
		spaceObjectsList.push_back(SpaceObjectProperties("Venus", venus, this->viewProjectionMatrix));

		Planet* humea = new Planet(programTex, sphereContext, glm::eulerAngleY(time / 5) * glm::translate(glm::vec3(30.f, 0, 0)) * glm::scale(glm::vec3(2.f)), SpaceObjectsList::haumeaTexture, SpaceObjectsList::haumeaNormalTexture);
		spaceObjectsList.push_back(SpaceObjectProperties("Humea", humea, this->viewProjectionMatrix));

		Planet* mercury = new Planet(programTex, sphereContext, glm::eulerAngleY(time / 7) * glm::translate(glm::vec3(35.f, 0, 0)) * glm::scale(glm::vec3(0.79f)), SpaceObjectsList::mercuryTexture, SpaceObjectsList::mercuryNormalTexture);
		spaceObjectsList.push_back(SpaceObjectProperties("Mercury", mercury, this->viewProjectionMatrix));

	}
};