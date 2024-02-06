#include "ship.h"

class PlayerShip {
private:
	GLuint shipTexture;
	GLuint shipNormalTexture;
	GLuint programShip;
	GLuint programPBR;


	Core::Shader_Loader shaderLoader;
	Core::RenderContext shipContext;


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
	Ship* ship;

	PlayerShip() {};

	void init() {
		programShip = shaderLoader.CreateProgram("shaders/shader_ship.vert", "shaders/shader_ship.frag");
		programPBR = shaderLoader.CreateProgram("shaders/PBR.vert", "shaders/PBR.frag");

		loadModelToContext("./models/SciFi_Fighter.obj", shipContext);

		shipTexture = Core::LoadTexture("textures/spaceship/spaceship.jpg");
		shipNormalTexture = Core::LoadTexture("textures/spaceship/SF_Fighter_Normal.jpg");
		this->ship = new Ship("player_ship", programPBR, shipContext, shipTexture, shipNormalTexture);
		this->ship->init();
	};
};