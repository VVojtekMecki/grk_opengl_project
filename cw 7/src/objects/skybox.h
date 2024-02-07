#include <glew.h>
#include "../Shader_Loader.h"
#include "../Render_Utils.h"
#include "../Texture.h"
#include <..\..\cw 7\src\SOIL\SOIL.h>

class Skybox {
private:
	GLuint programSkybox;

	Core::Shader_Loader shaderLoader;

	Core::RenderContext cubeMapContex;

	GLuint cubemapTexture;
	GLuint skyboxTexture;

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
	Skybox() {}

	void init() {
		programSkybox = shaderLoader.CreateProgram("shaders/shader_skybox.vert", "shaders/shader_skybox.frag");

		loadModelToContext("./models/cube.obj", cubeMapContex);

		glGenTextures(1, &cubemapTexture);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		int w, h;
		std::vector<std::string> filepaths = {
		 /* "./textures/skybox/lightblue/right.png",
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
	}

	void renderSkybox(glm::mat4 transformation) {
		glUseProgram(programSkybox);
		glUniformMatrix4fv(glGetUniformLocation(programSkybox, "transformation"), 1, GL_FALSE, (float*)&transformation);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		Core::DrawContext(cubeMapContex);
	}
};