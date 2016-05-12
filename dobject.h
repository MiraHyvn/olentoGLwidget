#ifndef opengltest_dobject_h
#define opengltest_dobject_h

#include <vector>
#include <iostream>
#include <time.h>
#include <algorithm>
#include "oLoader.h"
#include "dFace.h"
#include "dClock.h"

class dFacesConnected;
class dObject;


//===================================== dFacesConnected =================================//
//=======================================================================================//


class dFacesConnected {
    std::vector<unsigned int> faceIds;
    
public:
    
	void addFace(unsigned int Id);

	dFacesConnected();

	dFacesConnected(unsigned int vertexId,
		std::vector<unsigned int> elements,
		std::vector<glm::vec3> normals);
   
	glm::vec3 calculateVertexNormal(std::vector<dFace>& allFaces);
};

//======================================== dObject =========================================//
//==========================================================================================//
//                                                                                          //
// dObject is object for openGl. The goal is to make class that is easy read and handle     //
// with main openGl's loop.                                                                 //
// dObject needs to be able to calculate normals on the go. That is made in the subclasses  //
// dFace and dFaceConnected. These two calsses are reference classes with one               //
// or two functions.                                                                        //
//                                                                                          //
//==========================================================================================//

struct oRawDataT{
    int length;
    void* data;

    oRawDataT();
};


class dObject : public oLoader {
private:
    static int generateId();

public:
    dObject();
    dObject(std::string path);
    dObject(std::vector<glm::vec3> _vertices, std::vector<unsigned int> _elements);

    int id;
    bool loadFromFile(std::string path);

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> elements;
        
    oRawDataT getVertexData();
    oRawDataT getNormalData();
	oRawDataT getElementData();

	std::vector<dFace> faces;
	std::vector<glm::vec3> facePositions;

    void calculateAllNormals();
    void changeVertices(std::vector<glm::vec3>& new_vertices);
    void changeVerticesTowards(std::vector<glm::vec3>& aim_vertices, float multi);
    
    std::vector<dFacesConnected> vFacesConnected;

	void sortElementsByDistance(glm::vec3 cameraPos);
	void sortElementsByDistance();

    bool isReady();

private:
  
    void makeFaces();
    void makeFacesConnected();

	glm::vec3 cameraPosition;           //tämä on läpinäkyvyyttä varten
	bool faceIsCloser(int a, int b); //palauta tosi jos a on lähempänä kameraa kuin b

};

#endif
