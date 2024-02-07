#pragma once
#include "repairment.h"

class ShipToRepair {
public:

	GLuint shipToRepairTexture;
	GLuint shipToRepairNormalRepaired;
	GLuint shipToRepairNormalDamaged;

	GLuint programShipToRepair;
	//const char* const shipToRepairTexturePaths[5] = { "./textures/spaceship/.png", "./textures/planets/mercury.png"};

	Core::Shader_Loader shaderLoader;
	Core::RenderContext shipToRepairContext;

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
	Repairment* repair;


	ShipToRepair() {};


	void init() {
		programShipToRepair = shaderLoader.CreateProgram("shaders/shader_5_1_tex_copy.vert", "shaders/shader_5_1_tex_copy.frag");

		loadModelToContext("./models/UFO.obj", shipToRepairContext);

		shipToRepairTexture = Core::LoadTexture("textures/spaceship/ufo_main_ship_to_repair.png");


		shipToRepairNormalDamaged = Core::LoadTexture("textures/planets/asteroid_normal.jpg");
		shipToRepairNormalRepaired = Core::LoadTexture("textures/planets/ufo_normal.png");
		

		this->repair = new Repairment("shipToRepair", programShipToRepair, shipToRepairContext, shipToRepairTexture, shipToRepairNormalDamaged, shipToRepairNormalRepaired);
	}


};