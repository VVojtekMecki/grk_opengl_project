#include "space_objects.h"
class SunObject {
private:
	GLuint programSun;

	GLuint sunTexture;

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
	Sun* object;

	SunObject() {}

	void init() {
		programSun = shaderLoader.CreateProgram("shaders/shader_sun.vert", "shaders/shader_sun.frag");

		loadModelToContext("./models/sphere.obj", sphereContext);

		sunTexture = Core::LoadTexture("textures/planets/lava_sun.png");

		this->object = new Sun("sun", programSun, sphereContext, sunTexture);
	}
};