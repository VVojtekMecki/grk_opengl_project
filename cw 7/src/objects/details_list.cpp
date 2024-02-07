#include "space_objects.h"
#include <map>


class DetailsProperties {
public:
	std::string name;
	Details* detail;
	glm::mat4 viewProjectionMatrix;

	DetailsProperties(std::string name, Details* obj)
		: name(name), detail(obj) {}

};


class DetailsList {
	GLuint engineDetailTexture;
	GLuint engineDetailNormal;

	GLuint firstAidKit;
	GLuint firstAidKitNormal;

	GLuint crewMember;
	GLuint crewMemberNormal;

	GLuint programTex;

	Core::Shader_Loader shaderLoader;

	Core::RenderContext spaceshipEngineDetailContext;
	Core::RenderContext firstAidBoxContext;
	Core::RenderContext crewMemberContext;

	/*bool isShifWtPressed = false;
	bool isPKeyPressed = false;
	bool canPickUpEngine = false;
	bool canPickUpKit = false;

	bool canPickUpCrew = false;
	bool isRKeyPressed = false;

	bool drawSpaceshipEngine = true;
	bool drawSpaceshipKit = true;
	bool drawSpaceshipCrew = true;

	bool isDragging = false;

	bool isEnginePickedUp = false;
	bool isKitPickedUp = false;
	bool isCrewPickedUp = false;

	bool isEngineCloseToShipRepair = false;
	bool isKitCloseToShipRepair = false;
	bool isCrewCloseToShipRepair = false;*/

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
	std::vector<DetailsProperties> detailsList;


	DetailsList(float time, glm::mat4 viewProjectionMatrix) : time(time), viewProjectionMatrix(viewProjectionMatrix) {}

	void init() {

		programTex = shaderLoader.CreateProgram("shaders/shader_5_1_tex_copy.vert", "shaders/shader_5_1_tex_copy.frag");
		
		engineDetailTexture = Core::LoadTexture("textures/details/airplane_engine_BaseColor.png");
		engineDetailNormal = Core::LoadTexture("textures/details/airplane_engine_Normal.png");

		firstAidKit = Core::LoadTexture("textures/details/first_aid_box_base.png");
		firstAidKitNormal = Core::LoadTexture("textures/details/first_aid_box_base_normal.png");

		crewMember = Core::LoadTexture("textures/details/Alien_BaseColor.png");
		crewMemberNormal = Core::LoadTexture("textures/details/Alien_Normal.png");

		loadModelToContext("./models/airplane-engine-detail.obj", spaceshipEngineDetailContext);
		loadModelToContext("./models/Firstaidbox.obj", firstAidBoxContext);
		loadModelToContext("./models/crew_member.obj", crewMemberContext);


		Details* engineDetail = new Details("engineDetail", programTex, spaceshipEngineDetailContext,  engineDetailTexture, engineDetailNormal);
		Details* kitDetail = new Details("kitDetail", programTex, firstAidBoxContext, firstAidKit, firstAidKitNormal);
		Details* crewMemberDetail = new Details("crewMemberDetail", programTex, crewMemberContext, crewMember, crewMemberNormal);

		detailsList.push_back(DetailsProperties("engineDetail", engineDetail));
		detailsList.push_back(DetailsProperties("kitDetail", kitDetail));
		detailsList.push_back(DetailsProperties("crewMemberDetail", crewMemberDetail));



	}
	void updateTime(float newTime) {
		this->time = newTime;
	}
};

