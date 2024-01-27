#include "glew.h"

#include <GLFW/glfw3.h>
#include "glm.hpp"
#include "ext.hpp"
#include <iostream>
#include <cmath>

#include "scene.hpp"



int main(int argc, char** argv)
{
	// inicjalizacja glfw
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	float width = 1280;
	float height = 720;

	// tworzenie okna za pomoca glfw
	GLFWwindow* window = glfwCreateWindow(width, height, "FirstWindow", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// ladowanie OpenGL za pomoca glew
	glewInit();
	aspectRatio = width / float(height);
	glViewport(0, 0, width, height);
	//glViewport(0, 0, 1280, 720);

	init(window);

	// uruchomienie glownej petli
	renderLoop(window);

	shutdown(window);
	glfwTerminate();
	return 0;
}
