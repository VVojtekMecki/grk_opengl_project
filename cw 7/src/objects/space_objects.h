#include <string>
#include <vector>
#include "../Shader_Loader.h"
#include "../Render_Utils.h"
#include "../Texture.h"

#ifndef SPACE_OBJECT_H
#define SPACE_OBJECT_H

class SpaceObject {
	public:
		virtual ~SpaceObject() = default;
		virtual void drawObjectTexture(glm::mat4 viewProjectionMatrix, glm::mat4 modelMatrix) const = 0;
		virtual std::string getName() const = 0;
		virtual GLuint getTexture() const = 0;
		virtual GLuint getNormals() const = 0;
		virtual GLuint getProgram() const = 0;
		virtual Core::RenderContext& getContext() const = 0;
		virtual glm::mat4 getModelMatrix() const = 0;
		virtual float getTime(float time) { return time; };
};

#endif

#ifndef PLANET_H
#define PLANET_H

class Planet : public SpaceObject {
	public:
		std::string name;
		GLuint texture;
		GLuint normals;
		GLuint program;
		Core::RenderContext& context;
		glm::mat4 modelMatrix;

	public:
		Planet(std::string name, GLuint program, Core::RenderContext& context, GLuint texture, GLuint normals)
			: program(program), context(context), texture(texture), normals(normals), name(name) {}
		void drawObjectTexture(glm::mat4 viewProjectionMatrix, glm::mat4 modelMatrix) const override {
			glUseProgram(program);
			glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
			glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);
			glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

			Core::SetActiveTexture(texture, "texture", program, 0);
			Core::SetActiveTexture(normals, "normalSampler", program, 1);

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

#ifndef CLOUDED_PLANET_H
#define CLOUDED_PLANET_H

class CloudedPlanet : public Planet {
private:
	GLuint clouds;

public:
	CloudedPlanet(std::string name, GLuint program, Core::RenderContext& context, GLuint texture, GLuint normals, GLuint clouds)
		: Planet(name, program, context, texture, normals), clouds(clouds) {}

	void drawObjectTexture(glm::mat4 viewProjectionMatrix, glm::mat4 modelMatrix) const override {
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
	GLuint getTexture() const override { return this->texture; };
	GLuint getNormals() const override { return this->normals; };
	GLuint getProgram() const override { return this->program; };
	Core::RenderContext& getContext() const override { return this->context; };
	glm::mat4 getModelMatrix() const override { return this->modelMatrix; };
};

#endif

#ifndef SUN_H
#define SUN_H

class Sun : public SpaceObject {
	public:
		std::string name;
		GLuint texture;
		GLuint program;
		Core::RenderContext& context;
		glm::mat4 modelMatrix;

	public: 
		Sun(std::string name, GLuint program, Core::RenderContext& context, GLuint texture)
		: program(program), context(context), texture(texture), name(name) {}

		void drawObjectTexture(glm::mat4 viewProjectionMatrix, glm::mat4 modelMatrix) const override {
			glUseProgram(program);
			glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
			glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);
			glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

			Core::SetActiveTexture(texture, "texture", program, 0);

			Core::DrawContext(context);
			glUseProgram(0);
		}
		GLuint getTexture() const override { return this->texture; };
		GLuint getNormals() const override { return NULL; };
		GLuint getProgram() const override { return this->program; };
		Core::RenderContext& getContext() const override { return this->context; };
		glm::mat4 getModelMatrix() const override { return this->modelMatrix; };
		std::string getName() const override { return this->name; };
};

#endif

#ifndef ASTEROID_H
#define ASTEROID_H

class Asteroid : public Planet {
private:
	int posX;
	int posY;
	int posZ;

public:
	Asteroid(std::string name, GLuint program, Core::RenderContext& context, GLuint texture, GLuint normals, int x, int y, int z)
		: Planet(name, program, context, texture, normals),  posX(x), posY(y), posZ(z) {}

	void drawObjectTexture(glm::mat4 viewProjectionMatrix, glm::mat4 modelMatrix) const override {
		glUseProgram(program);
		glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
		glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);
		glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

		Core::SetActiveTexture(texture, "texture", program, 0);
		Core::SetActiveTexture(normals, "normalSampler", program, 1);

		Core::DrawContext(context);
		glUseProgram(0);
	}

	auto getPosition() const {
		struct retVals {
			int x, y, z;
		};

		return retVals{ this->posX, this->posY, this->posZ };
	};
};

#endif