#pragma once
#include "space_objects.h"
#ifndef SHIPTOREPAIR_H
#define SHIPTOREPAIR_H


class Repairment {
public:
	std::string name;
	GLuint texture;
	GLuint normalRepaired;
	GLuint normalsDamaged;

	GLuint program;
	Core::RenderContext& context;
	glm::mat4 modelMatrix;

	int amountOfPickedUpObjects = 0;

	 GLuint repairShipNormalsPaths[3] = { normalsDamaged, normalRepaired };
	int indexNormal = 0;

public:
	Repairment(std::string name, GLuint program, Core::RenderContext& context, GLuint texture, GLuint normalsDamaged, GLuint normalRepaired)
		: program(program), context(context), texture(texture), normalRepaired(normalRepaired), normalsDamaged(normalsDamaged), name(name) {}




	void drawObjectTexture(glm::mat4 viewProjectionMatrix, glm::mat4 modelMatrix) {

		glUseProgram(program);
		glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
		glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);
		glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
		Core::SetActiveTexture(texture, "texture", program, 0);


		Core::SetActiveTexture(repairShipNormalsPaths[indexNormal], "normalSampler", program, 1);
		
	

		Core::DrawContext(context);
		glUseProgram(0);
	}
	/*GLuint getTexture() const override { return this->texture; };
	GLuint getNormals() const override { return this->normalRepaired; };
	GLuint getNormalsDamaged() const { return this->normalsDamaged; };

	GLuint getProgram() const override { return this->program; };
	Core::RenderContext& getContext() const override { return this->context; };
	glm::mat4 getModelMatrix() const override { return this->modelMatrix; };
	std::string getName() const override { return this->name; };*/

	


};
#endif // !SHIPTOREPAIR_H
