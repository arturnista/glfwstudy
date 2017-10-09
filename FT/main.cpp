#define _SCL_SECURE_NO_WARNINGS
#pragma warning(disable:4996)

#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library
#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <math.h>
#include <time.h>       /* time */

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"
#include "Lamp.h"
#include "Bunny.h"


using namespace std;
using namespace glm;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);

int main() {
  	srand (time(NULL));
	// start GL context and O/S window using the GLFW helper library
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return 1;
	}

	glEnable(GL_CULL_FACE);
	const float INITIAL_SCREEN_WIDTH = 800;
	const float INITIAL_SCREEN_HEIGHT = 600;

	GLFWwindow* window = glfwCreateWindow(INITIAL_SCREEN_WIDTH, INITIAL_SCREEN_HEIGHT, "Hello Triangle", NULL, NULL);
	if (!window) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, cursor_pos_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	// tell GL to only draw onto a pixel if the shape is closer to the viewer
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"

	vec3 lightPosition = vec3(5.0f, 5.0f, 0.0f);

	int GAME_OBJECTS_COUNTER = 2;
	Bunny* bunnyObject = new Bunny();
	bunnyObject->setPosition(vec3(0, 0, 0));
	Lamp* lampObject = new Lamp();
	lampObject->setPosition(lightPosition);

	Shader* shader = new Shader("lighting");

	float rotateSpeed = .1;
	float distanceX = 0.0f;
	float distanceY = 0.0f;
	float distanceZ = 3.0f;

	double lastMouseX = 0.0f;
	double lastMouseY = 0.0f;

	bool isFirstMouse = true;

	float deltaTime = 0.0f;	// Time between current frame and last frame
	float lastFrame = 0.0f; // Time of last frame

    Camera* camera = new Camera(window);

	float pitch = 0;
	float yaw = 0;

	float mouseSens = 1.3f;
    tLight light = {
        vec3(1.0f, 1.0f, 1.0f),
        lightPosition
    };
	while (!glfwWindowShouldClose(window)) {
		// Updates the screen size
		int width, height;
		glfwGetWindowSize(window, &width, &height);
		float SCREEN_WIDTH = width;
		float SCREEN_HEIGHT = height;

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

		// Compute the delta time
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Get the cursor position
		double mouseX, mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);

		// Compute the mouse position

		if (isFirstMouse) {
			lastMouseY = mouseY;
			lastMouseX = mouseX;
			isFirstMouse = false;
		}
		float mouseOffsetX = (mouseX - lastMouseX) * deltaTime * mouseSens;
		float mouseOffsetY = (lastMouseY - mouseY) * deltaTime * mouseSens;
		lastMouseY = mouseY;
		lastMouseX = mouseX;
		// Uses the screen center as reference
		//float mouseOffsetX = deltaTime * mouseSens * (SCREEN_WIDTH / 2 - mouseX);
		//float mouseOffsetY = deltaTime * mouseSens * (SCREEN_HEIGHT / 2 - mouseY);

		pitch += mouseOffsetY;
		yaw += mouseOffsetX;
		if(pitch > 89.0f) pitch =  89.0f;
		if(pitch < -89.0f) pitch = -89.0f;

		vec3 lookingDirection = vec3(
			cos(radians(pitch)) * cos(radians(yaw)),
			sin(radians(pitch)),
			cos(radians(pitch)) * sin(radians(yaw))
		);

		camera->setFront( normalize(lookingDirection) );
        vec3 cameraFront = camera->getFront();
		float cameraSpeed = 10.0f * deltaTime;
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			camera->setPosition( camera->getPosition() + (cameraSpeed * cameraFront) );
		} else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			camera->setPosition( camera->getPosition() - (cameraSpeed * cameraFront) );
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			camera->setPosition( camera->getPosition() - (normalize(cross(cameraFront, camera->getUp())) * cameraSpeed) );
		} else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			camera->setPosition( camera->getPosition() + (normalize(cross(cameraFront, camera->getUp())) * cameraSpeed) );
		}

		/*
			Objects update
		*/
        bunnyObject->update(window, deltaTime);
		lampObject->update(window, deltaTime);

		bunnyObject->render(shader, camera, light);
		lampObject->render(shader, camera, light);

		// Reset the vertex bind
		glBindVertexArray(0);

		// update other events like input handling
		glfwPollEvents();
		// put the stuff we've been drawing onto the display
		glfwSwapBuffers(window);

		// Set the cursor to the middle of the screen
		//glfwSetCursorPos(window, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
	}

	// close GL context and any other GLFW resources
	glfwTerminate();
	return 0;
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if ((key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q) && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}

void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos) {
	// cout << xpos << '\n';
}
