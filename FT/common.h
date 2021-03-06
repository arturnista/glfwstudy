#ifndef __COMMON_HEADER__
#define __COMMON_HEADER__ 1

#include <iostream>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <math.h>
#include <string>
#include <vector>

typedef unsigned long tHash;

typedef struct {
	glm::vec3 color;
	glm::vec3 position;
} tLight;

struct tObject {
	std::vector<GLfloat> pointsVector;
	std::vector<GLuint> indexVector;
	std::vector<GLfloat> normalVector;
	std::vector<GLfloat> textureVector;
	bool hasTexture;
	int itemsPerPoint;
	GLfloat *points;
	int pointsCounter;
	GLuint *vertices;
	int verticesCounter;
	std::string textureName;
};
typedef struct tObject tObject;

float distance(glm::vec3 pos1, glm::vec3 pos2);

tHash hashNumber(long v);

tHash hashVec3(glm::vec3 v);

#endif
