#include "ResourcesManager.h"

ResourcesManager::ResourcesManager() {
    tJson configData = readConfigFile();

    // Iterate thru objects
    tJson objects = configData["objects"];
    for (auto& obj : tJson::iterator_wrapper(configData["objects"])) {
        objectMap[obj.key()] = this->processObjectFile(obj.value());
    }

    this->light = {
        glm::vec3(configData["light"]["color"]["r"], configData["light"]["color"]["g"], configData["light"]["color"]["b"]), // Color
        glm::vec3(configData["light"]["position"]["x"], configData["light"]["position"]["y"], configData["light"]["position"]["z"]) // Position
    };
    this->configData = configData;
}

unsigned int ResourcesManager::processTexture(std::string filename) {
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char *textureData = stbi_load(filename.c_str(), &width, &height, &nrChannels, STBI_default);
    if (textureData) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture " << filename << std::endl;
    }
    stbi_image_free(textureData);


    cout << "Texture: " << filename << "\t";
    cout << "Size (" << width << ", " << height << ")" << '\n';
    return texture;
}

tObject ResourcesManager::processObjectFile(std::string filename) {
	std::vector<GLfloat> pointsVector = {};
	std::vector<GLuint> indexVector = {};
	std::vector<GLfloat> normalVector = {};
	std::vector<GLfloat> textureVector = {};
    std::string materialName = "";
    std::string textureName;

    readObjFile(filename, pointsVector, indexVector, normalVector, textureVector, materialName);
    if(materialName.length() > 0) {
        materialName = "./assets/materials/" + materialName;
        readMtlFile(materialName, textureName);
        if(textureMap[textureName] == 0) {
            textureMap[textureName] = this->processTexture("./assets/textures/" + textureName);
        }
    }

    cout << "Object: " << filename << "\t";
    std::cout << "texture" << " = " << textureName << '\t';
    cout << "vertex count" << " = " << pointsVector.size() << '\t';
    cout << "face count" << " = " << indexVector.size() << '\n';

    return processObject(pointsVector, indexVector, normalVector, textureVector, textureName);
}

tObject ResourcesManager::combineObjects(tObject object1, tObject object2, glm::vec3 offset) {
    std::vector<GLfloat> pointsVector = {};
    pointsVector.reserve( object1.pointsVector.size() + object2.pointsVector.size() );
    pointsVector.insert( pointsVector.end(), object1.pointsVector.begin(), object1.pointsVector.end() );
    pointsVector.insert( pointsVector.end(), object2.pointsVector.begin(), object2.pointsVector.end() );

    for (size_t i = object1.pointsVector.size(); i < pointsVector.size(); i += 3) {
		pointsVector.at(i) += offset.x;
		pointsVector.at(i + 1) += offset.y;
		pointsVector.at(i + 2) += offset.z;
    }

    std::vector<GLuint> indexVector = {};
    indexVector.reserve( object1.indexVector.size() + object2.indexVector.size() );
    indexVector.insert( indexVector.end(), object1.indexVector.begin(), object1.indexVector.end() );
    indexVector.insert( indexVector.end(), object2.indexVector.begin(), object2.indexVector.end() );

    int firstObjPointsInc = object1.pointsVector.size() / 3;
	for (size_t i = object1.indexVector.size(); i < indexVector.size(); i++) {
        indexVector.at(i) += firstObjPointsInc;
    }

    std::vector<GLfloat> normalVector = {};
    normalVector.reserve( object1.normalVector.size() + object2.normalVector.size() );
    normalVector.insert( normalVector.end(), object1.normalVector.begin(), object1.normalVector.end() );
    normalVector.insert( normalVector.end(), object2.normalVector.begin(), object2.normalVector.end() );

    std::vector<GLfloat> textureVector = {};
    textureVector.reserve( object1.textureVector.size() + object2.textureVector.size() );
    textureVector.insert( textureVector.end(), object1.textureVector.begin(), object1.textureVector.end() );
    textureVector.insert( textureVector.end(), object2.textureVector.begin(), object2.textureVector.end() );
    // std::cout << "Tex " << object1.textureName << '\n';
    return processObject(pointsVector, indexVector, normalVector, textureVector, object1.textureName);
}

tObject ResourcesManager::processObject( vector<GLfloat>& pointsVector, vector<GLuint>& indexVector, vector<GLfloat>& normalVector, vector<GLfloat>& textureVector, std::string textureName ) {

    bool hasTexture = textureVector.size() > 0;

    int textureCounter = (pointsVector.size() / 3) * 2;
    int pointsCounter = pointsVector.size() * 2 + textureCounter;
    int verticesCounter = indexVector.size();

    int itemsPerPoint = 8;

	GLfloat *points = new GLfloat[pointsCounter];

	int counter = 0;
	int texCounter = 0;
	for (size_t i = 0; i < pointsCounter; i += itemsPerPoint) {
		// Position
		points[i + 0] = pointsVector.at(counter);
		points[i + 1] = pointsVector.at(counter + 1);
		points[i + 2] = pointsVector.at(counter + 2);

		if(normalVector.size() > 0 && counter + 3 < normalVector.size()) {
			points[i + 3] = normalVector.at(counter);
			points[i + 4] = normalVector.at(counter + 1);
			points[i + 5] = normalVector.at(counter + 2);
		} else {
            points[i + 3] = 1.0f;
			points[i + 4] = 0.0f;
			points[i + 5] = 0.0f;
		}

		if(hasTexture && texCounter + 1 < textureVector.size()) {
			points[i + 6] = textureVector.at(texCounter);
			points[i + 7] = textureVector.at(texCounter + 1);
		} else {
			points[i + 6] = 0.0f;
			points[i + 7] = 0.0f;
		}
		texCounter += 2;
		counter += 3;
	}

	GLuint *vertices = new GLuint[verticesCounter];
	for (size_t i = 0; i < verticesCounter; i++) vertices[i] = indexVector.at(i) - 1;

    return {
    	pointsVector,
    	indexVector,
    	normalVector,
    	textureVector,
    	hasTexture,
        itemsPerPoint,
    	points,
    	pointsCounter,
    	vertices,
    	verticesCounter,
        textureName
    };
}

tObject ResourcesManager::getObject(std::string objectName) {
    return this->objectMap[objectName];
}

unsigned int ResourcesManager::getTexture(std::string textureName) {
    return this->textureMap[textureName];
}

tLight ResourcesManager::getLight() {
    return this->light;
}

tJson ResourcesManager::getConfigData() {
    return this->configData;
}
