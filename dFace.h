//
//  dFace.h
//  opengltest
//
//  Created by Ismo Torvinen on 27.3.2016.
//  Copyright (c) 2016 Ismo Torvinen. All rights reserved.
//

#include <glm/glm.hpp>
#include <vector>

#ifndef opengltest_dFace_h
#define opengltest_dFace_h

//========================================= dFace =======================================//
//=======================================================================================//

class dFace 
{
public:

	std::vector<unsigned int> vertsIds; //facen verteksit indekseinä dObjectin verteksidataan
    
    glm::vec3 faceNormal;
    
    dFace(std::vector<unsigned int> _vertsIds) : vertsIds(_vertsIds) {}
    
	dFace(unsigned int* _vertsIds) : vertsIds(_vertsIds,_vertsIds + 3) {}
    

    void calculateFaceNormal(std::vector<glm::vec3>& allVerts)
    {
        glm::vec3 verts[3] = { allVerts[vertsIds[0]], allVerts[vertsIds[1]], allVerts[vertsIds[2]] };
        
        glm::vec3 vect1;
        glm::vec3 vect2;
        
        vect1 = verts[1] - verts[0];
        vect2 = verts[2] - verts[0];
        
        faceNormal = glm::cross(vect1, vect2);
    }
};


#endif
