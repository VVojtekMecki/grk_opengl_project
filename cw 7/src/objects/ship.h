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

	void drawObjectTexture(glm::mat4 viewProjectionMatrix, glm::mat4 modelMatrix) const override {
		glUseProgram(program);
		glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
		glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);
		glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

		Core::SetActiveTexture(texture, "ship", program, 0);
		Core::SetActiveTexture(rust, "rust", program, 1);
		Core::SetActiveTexture(shipScratches, "asteroid", program, 2);
		Core::SetActiveTexture(normals, "normalSampler", program, 3);

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
