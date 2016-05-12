#include "oLoader.h"
#include <iostream>
using namespace std;

bool oLoader::loadOBJ(
	std::string path,
	std::vector<glm::vec3>& vertices)
{

    //std::cerr << "Loading OBJ file " << path.c_str() << "... ";

	FILE * file = fopen(path.c_str(), "r");
	if (file == NULL){
        //cerr << "Couldn't open file" << path.c_str() << endl;
		return false;
	}

    if(!vertices.empty() )
        vertices.clear();

	while (1){

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader

		if (strcmp(lineHeader, "v") == 0){

			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			vertices.push_back(vertex);

		}
		else{
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}
	}


    //std::cerr << "ok\n";
	return true;
}

bool oLoader::loadOBJ(
	std::string path,
	std::vector<glm::vec3>& vertices,
	std::vector<unsigned int>& elements,
	std::vector<dFace>& faces)
{

    //std::cout << "Loading OBJ file: " << path.c_str() << " ... " ;

    FILE * file = fopen(path.c_str(), "r");
	if (file == NULL){
        //cerr << "\n   Couldn't open file!\nLoadOBJ failed\n";
		return false;
	}

    if(!vertices.empty() )
        vertices.clear();
    if(!elements.empty() )
        elements.clear();
    if(!faces.empty() )
        faces.clear();


	while (1){

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader

		if (strcmp(lineHeader, "v") == 0){

			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			vertices.push_back(vertex);

		}
		else if (strcmp(lineHeader, "f") == 0){

			unsigned int vertexIndex[3], normalIndex[3];

			int matches = fscanf(file, "%d//%d %d//%d %d//%d\n", &vertexIndex[0], &normalIndex[0], &vertexIndex[1], &normalIndex[1], &vertexIndex[2], &normalIndex[2]);

			if (matches != 6){
                //cerr << ("\n   Couldn't parse file!\nLoadOBJ failed\n");
				return false;

			}

			vertexIndex[0] -= 1;
			vertexIndex[1] -= 1;
			vertexIndex[2] -= 1;

			// use these to make Face
			faces.push_back(vertexIndex);

			elements.push_back(vertexIndex[0]);
			elements.push_back(vertexIndex[1]);
			elements.push_back(vertexIndex[2]);

		}
		else{
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}
	}
    //cerr << "ok\n";
	return true;
}

