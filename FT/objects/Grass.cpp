#include "Grass.h"

Grass::Grass(ResourcesManager* tm) : GameObject(tm, GO_TYPE_GROUND, "grass_normal", vec3(1.0f), vec3(1.0f)) {
    
}

Grass::Grass(ResourcesManager* tm, float size) : GameObject(tm, GO_TYPE_GROUND, "grass_normal", vec3(size), vec3(1.0f)) {
    
}

void Grass::update(GLFWwindow* window, float deltaTime) {
}
