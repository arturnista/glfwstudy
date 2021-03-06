#include "readFiles.h"

const char* readFile(string filename) {
	string fullFileData;
	ifstream openedFile(filename.c_str());
	if (openedFile.is_open()) {
		string line;
		while (getline(openedFile, line)) {
			fullFileData += line + '\n';
		}
		openedFile.close();
	}

	// Copy the string
	char * writable = new char[fullFileData.size() + 1];
	std::copy(fullFileData.begin(), fullFileData.end(), writable);
	writable[fullFileData.size()] = '\0';

	return writable;
}

vec3 splitLine(string line) {
	string::size_type sz;
	string strNumber = "";
	int idx = 0;
	vec3 result;

	bool digitFounded = false;

	for (size_t i = 2; i < line.length(); i++) {
		char character = line.at(i);
		if(!digitFounded) {
			if(character == ' ') continue;
			digitFounded = true;
		}

		if(character != ' ') {
			strNumber += character;
		} else  {
			float number = stof(strNumber, &sz);
			strNumber = "";
			if(idx == 0) result.x = number;
			else if(idx == 1) result.y = number;
			else if(idx == 2) result.z = number;
			idx++;
		}
	}
	if(idx == 2) {
		float finalNumber = stof(strNumber, &sz);
		result.z = finalNumber;
	}
	return result;
}

gameObject readObjectFile(string filename, float size, vec3 offset) {
	std::vector<GLfloat> pointsVector = {};
    std::vector<GLuint> indexVector = {};

	ifstream openedFile(filename.c_str());
	if (openedFile.is_open()) {
		string line;
		while (getline(openedFile, line)) {
			char first = '#';
			if(line.length() > 0) first = line.at(0);

			if(first == 'v') {
				vec3 point = splitLine(line);
				pointsVector.push_back(point.x + offset.x);
				pointsVector.push_back(point.y + offset.y);
				pointsVector.push_back(point.z + offset.z);
			}
			if(first == 'f') {
				vec3 index = splitLine(line);
				indexVector.push_back(index.x);
				indexVector.push_back(index.y);
				indexVector.push_back(index.z);
			}
		}
		openedFile.close();
	}

	int pointsCounter = pointsVector.size() * 2;
	int vertexCounter = indexVector.size();

	cout << filename << "\t";
	cout << "vertex count " << " = " << pointsCounter << " -- ";
	cout << "face count " << " = " << vertexCounter << '\n';

	GLfloat *points = new GLfloat[pointsCounter];
	glm::vec3 color = glm::vec3(
		static_cast <float> (rand()) / static_cast <float> (RAND_MAX),
		static_cast <float> (rand()) / static_cast <float> (RAND_MAX),
		static_cast <float> (rand()) / static_cast <float> (RAND_MAX)
	);
	int counter = 0;
	for (size_t i = 0; i < pointsCounter; i += 6) {
		points[i] = pointsVector.at(counter++);
		points[i + 1] = pointsVector.at(counter++);
		points[i + 2] = pointsVector.at(counter++);
		// counter += 3;

		points[i + 3] = color.x; // R
		points[i + 4] = color.y; // G
		points[i + 5] = color.z; // B
	}
	// copy(pointsVector.begin(), pointsVector.end(), points);
	// if(size != 1) for (size_t i = 0; i < pointsCounter; i++) points[i] = points[i] * size;

	GLuint *indexArray = new GLuint[vertexCounter];
	copy(indexVector.begin(), indexVector.end(), indexArray);
	for (size_t i = 0; i < vertexCounter; i++) indexArray[i] = indexArray[i] - 1;

	GLuint VBO = 0;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, pointsCounter * sizeof(GLfloat), points, GL_STATIC_DRAW);
	// With colors
	// glBufferData(GL_ARRAY_BUFFER, 8 * 9 * sizeof(GLfloat), points, GL_STATIC_DRAW);

	GLuint VAO = 0;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Positions without colors
	// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
	// glEnableVertexAttribArray(0);

	// Positions
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);

	// Colors
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	GLuint EBO = 0;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * vertexCounter, indexArray, GL_STATIC_DRAW);

	return {
		VAO,
		vertexCounter,
		size,
	};
}

gameObject readObjectFile(string filename, float size) {
	return readObjectFile(filename, size, vec3(0.0f, 0.0f, 0.0f));
}
