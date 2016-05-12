#ifndef opengltest_oLoader_h
#define opengltest_oLoader_h

#include <glm/glm.hpp>
#include <vector>
#include "dFace.h"

class oLoader {

protected:
    bool loadOBJ(std::string path, std::vector<glm::vec3>& vertices);
    bool loadOBJ(std::string path, std::vector<glm::vec3>& vertices, std::vector<unsigned int>& elements, std::vector<dFace>& faces);

};


#endif
