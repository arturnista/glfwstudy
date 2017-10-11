#ifndef __RESOURCES_MANAGER_HEADER__
#define __RESOURCES_MANAGER_HEADER__ 1

#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library

#include <iostream>
#include <vector>
#include <map>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "common.h"
#include "readFiles.h"
#include "./vendor/stb_image.h"

class ResourcesManager {
private:
    std::map<std::string, unsigned int> textureMap;
    std::map<std::string, tObject> objectMap;

    unsigned int processTexture(std::string filename);
    tObject processObjectFile(std::string filename);

public:
    ResourcesManager ();

    unsigned int getTexture(std::string textureName);
    tObject getObject(std::string objectName);
};

#endif