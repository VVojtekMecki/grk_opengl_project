#include "space_objects.h"

#ifndef SHIP_H
#define SHIP_H

class Ship : public SpaceObject {

	GLuint rust;
	GLuint shipScratches;

public:
	std::string name;
	GLuint texture;
	GLuint normals;
	GLuint program;
	Core::RenderContext& context;
	glm::mat4 modelMatrix;

public:
	Ship(std::string name, GLuint program, Core::RenderContext& context, GLuint texture, GLuint normals)
		: program(program), context(context), texture(texture), normals(normals), name(name) {}

	void init() {
		rust = Core::LoadTexture("textures/spaceship/rust.png");
		shipScratches = Core::LoadTexture("textures/spaceship/scratches.png");
	}
	void drawWithPBR(glm::mat4 viewProjectionMatrix, glm::mat4 modelMatrix, float roughness, float metallic,
		glm::vec3 lightColor, float lightPower, glm::vec3 cameraPos, glm::mat4 startPlanetPos, glm::vec3 spotlightPos, glm::vec3 spotlightConeDir) const  {
		glUseProgram(program);
		glm::vec3 sunPos = glm::vec3();
		glm::vec3 sunColor = glm::vec3(0.9f, 0.9f, 0.7f)/*   *5  */;
		glm::vec3 pointlightPos = glm::vec3(0, 2, 0);
		glm::vec3 pointlightColor = glm::vec3(0.9, 0.6, 0.6);
		glm::vec3 spotlightColor = glm::vec3(0.4, 0.4, 0.9) /* * 3  */;
		float spotlightPhi = 3.14 / 4;
		glm::vec3 sunPosition = startPlanetPos * glm::vec4(1.f, 0.0f, 0.f, 1.f);
		glm::vec3 sunDir = glm::normalize(-sunPosition);
		Core::SetActiveTexture(texture, "colorTexture", program, 0);
		glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
		glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);
		glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

		glUniform1f(glGetUniformLocation(program, "exposition"), lightPower);

		glUniform1f(glGetUniformLocation(program, "roughness"), roughness);
		glUniform1f(glGetUniformLocation(program, "metallic"), metallic);

		glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);

		glUniform3f(glGetUniformLocation(program, "sunDir"), sunDir.x, sunDir.y, sunDir.z);
		glUniform3f(glGetUniformLocation(program, "sunColor"), lightColor.x, lightColor.y, lightColor.z);

		glUniform3f(glGetUniformLocation(program, "lightPos"), sunPos.x, sunPos.y, sunPos.z);
		glUniform3f(glGetUniformLocation(program, "lightColor"), lightColor.x, lightColor.y, lightColor.z);

		Core::DrawContext(context);
		glUseProgram(0);
	}
	void drawObjectTexture(glm::mat4 viewProjectionMatrix, glm::mat4 modelMatrix) const  {
		glUseProgram(program);
		glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
		glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);
		glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

		Core::SetActiveTexture(texture, "ship", program, 0);
		Core::SetActiveTexture(rust, "rust", program, 1);
		Core::SetActiveTexture(shipScratches, "scratches", program, 2);
		Core::SetActiveTexture(normals, "normalSampler", program, 3);

		Core::DrawContext(context);
		glUseProgram(0);
	}
	void drawObjectPBR(glm::mat4 viewProjectionMatrix, glm::mat4 modelMatrix, float roughness, float metallic,
		glm::vec3 lightColor, float lightPower, glm::vec3 cameraPos, glm::mat4 startPlanetPos, glm::vec3 spotlightPos, glm::vec3 spotlightConeDir) const override {
		glUseProgram(program);
		glm::vec3 sunDir = glm::vec3(1.f, 0.f, 1.f);
		glm::vec3 sunPos = glm::vec3(20.f, 0.f, 20.f);
		Core::SetActiveTexture(texture, "colorTexture", program, 0);
		glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
		glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);
		glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

		glUniform1f(glGetUniformLocation(program, "exposition"), lightPower);

		glUniform1f(glGetUniformLocation(program, "roughness"), roughness);
		glUniform1f(glGetUniformLocation(program, "metallic"), metallic);

		glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);

		glUniform3f(glGetUniformLocation(program, "sunDir"), sunDir.x, sunDir.y, sunDir.z);
		glUniform3f(glGetUniformLocation(program, "sunColor"), lightColor.x, lightColor.y, lightColor.z);

		glUniform3f(glGetUniformLocation(program, "lightPos"), sunPos.x, sunPos.y, sunPos.z);
		glUniform3f(glGetUniformLocation(program, "lightColor"), lightColor.x, lightColor.y, lightColor.z);

		Core::DrawContext(context);
		glUseProgram(0);
	}
	GLuint getTexture() const override { return this->texture; };
	GLuint getNormals() const override { return this->normals; };
	GLuint getProgram() const override { return this->program; };
	Core::RenderContext& getContext() const override { return this->context; };
	glm::mat4 getModelMatrix() const override { return this->modelMatrix; };
	std::string getName() const override { return this->name; };
};

#endif
