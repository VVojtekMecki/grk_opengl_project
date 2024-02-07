#include "space_objects.h"

#ifndef SHIP_H
#define SHIP_H

class Ship{

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
		rust = Core::LoadTexture("textures/spaceship/rust.jpg");
		shipScratches = Core::LoadTexture("textures/spaceship/scratches.jpg");
	}

	void drawWithPBR(glm::mat4 viewProjectionMatrix, glm::mat4 modelMatrix, float roughness, float metallic,
		glm::vec3 lightColor, float lightPower, glm::vec3 cameraPos, glm::vec3 startPlanetPos, glm::vec3 spotlightPos, glm::vec3 spotlightConeDir) {
		glUseProgram(program);
		glm::vec3 sunColor = glm::vec3(0.7f, 0.7f, 0.7f)/*   *5  */;

		glm::vec3 pointlightPos = glm::vec3(0, 0, 0);
		glm::vec3 pointlightColor = glm::vec3(0.9, 0.9, 0.9);
		glm::vec3 spotlightColor = glm::vec3(0.4, 0.4, 0.9) /* * 3  */;
		float spotlightPhi = 3.14 / 4;
		//glm::vec3 sunPosition = startPlanetPos * glm::vec4(1.f, 0.0f, 0.f, 1.f);
		glm::vec3 sunPos = glm::vec3();
		glm::vec3 sunDir = glm::normalize(startPlanetPos);

		Core::SetActiveTexture(texture, "texture1", program, 0);
		Core::SetActiveTexture(shipScratches, "texture2", program, 1);
		Core::SetActiveTexture(rust, "texture3", program, 2);

		Core::SetActiveTexture(normals, "textureNormal", program, 3);
		glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
		glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);
		glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

		glUniform1f(glGetUniformLocation(program, "exposition"), lightPower);

		glUniform1f(glGetUniformLocation(program, "roughness"), roughness);
		glUniform1f(glGetUniformLocation(program, "metallic"), metallic);

		glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);

		glUniform3f(glGetUniformLocation(program, "sunDir"), sunDir.x, sunDir.y, sunDir.z);
		glUniform3f(glGetUniformLocation(program, "sunColor"), sunColor.x, sunColor.y, sunColor.z);

		glUniform3f(glGetUniformLocation(program, "lightPos"), 0, 0, 0);
		glUniform3f(glGetUniformLocation(program, "lightColor"), lightColor.x, lightColor.y, lightColor.z);

		Core::DrawContext(context);
		glUseProgram(0);
	}
};

#endif