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

Core::Shader_Loader shaderLoader;

class SpaceObject {
public:
    virtual ~SpaceObject() = default;
    virtual float getX() const = 0;
    virtual float getY() const = 0;
    virtual float getZ() const = 0;
};


class Planet : public SpaceObject {
private:
    float x, y, z;
    GLuint texture;
    GLuint program;
    Core::RenderContext& context;

public:
    Planet(float x, float y, float z) : x(x), y(y), z(z) {}

    float getX() const override { return x; }
    float getY() const override { return y; }
    float getZ() const override { return z; }
};

