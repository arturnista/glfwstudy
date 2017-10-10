#ifndef __GAME_OBJECT_HEADER__
#define __GAME_OBJECT_HEADER__ 1

#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library

#include <vector>
#include <math.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"
#include "TexturesManager.h"
#include "readFiles.h"
#include "common.h"

using namespace std;
using namespace glm;

class GameObject {
protected:
	GLuint VAO;
	int vertexCounter;
	string textureName;
	bool hasTexture;
	TexturesManager* texturesManager;

	vec3 color;

	vec3 size;
    vec3 position;
	vec3 rotation;
    mat4 model;

	Shader* shader;
public:
	GameObject (TexturesManager* tm);
	GameObject (TexturesManager* tm, string filename, float size, vec3 color);
    GameObject (TexturesManager* tm, string filename, vec3 size, vec3 color);

	GLuint getVAO();
	int getVertexCounter();

    virtual void update(GLFWwindow* window, float deltaTime);

	void setTextureName(string name);

    vec3 getPosition();
    float getPositionX();
    float getPositionY();
    float getPositionZ();
    void setPosition(vec3 position);
    void setPositionX(float value);
    void setPositionY(float value);
    void setPositionZ(float value);

    vec3 getSize();
	void setSize(float size);
    void setSize(vec3 size);

    void rotateX(float value);
    void rotateY(float value);
    void rotateZ(float value);

    virtual void render(Camera* camera, tLight light);
};

#endif