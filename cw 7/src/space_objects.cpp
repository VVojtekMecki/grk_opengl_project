#include "glew.h"
#include <GLFW/glfw3.h>
#include "glm.hpp"
#include "ext.hpp"
#include <iostream>
#include <cmath>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>
#include <..\..\cw 7\src\SOIL\SOIL.h>
#include <vector>
#include "Shader_Loader.h"
#include "Render_Utils.h"
#include "Texture.h"

class SpaceObject {
public:
	virtual ~SpaceObject() = default;
	virtual void drawObjectTexture(glm::mat4 viewProjectionMatrix) const = 0;
	/*GLuint texture;
	GLuint normals;
	GLuint program;
	Core::RenderContext& context;
	glm::mat4 modelMatrix;*/
	/*virtual float getX() const = 0;
	virtual float getY() const = 0;
	virtual float getZ() const = 0;*/
};


class Planet : public SpaceObject {
public:
	GLuint texture;
	GLuint normals;
	GLuint program;
	Core::RenderContext& context;
	glm::mat4 modelMatrix;

public:
	Planet(GLuint program, Core::RenderContext& context, glm::mat4 modelMatrix, GLuint texture, GLuint normals) 
		: program(program), context(context), modelMatrix(modelMatrix), texture(texture), normals(normals) {}
	void drawObjectTexture(glm::mat4 viewProjectionMatrix) const override {
		glUseProgram(program);
		glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
		glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);
		glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

		Core::SetActiveTexture(texture, "texture", program, 0);
		Core::SetActiveTexture(normals, "normalSampler", program, 1);

		Core::DrawContext(context);
		glUseProgram(0);
	}
	//float getX() const override { return x; }
	//float getY() const override { return y; }
	//float getZ() const override { return z; }
};

class Earth : public Planet {

	GLuint clouds = Core::LoadTexture("textures/planets/8k_earth_clouds.jpg");

	void drawObjectTexture(glm::mat4 viewProjectionMatrix) const override {
		glUseProgram(program);
		glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
		glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);
		glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

		Core::SetActiveTexture(texture, "earth", program, 0);
		Core::SetActiveTexture(clouds, "clouds", program, 1);
		Core::SetActiveTexture(normals, "normalSampler", program, 2);

		Core::DrawContext(context);
		glUseProgram(0);
	}
};

