#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

using namespace glm;
#include "cameraControl.hpp"
mat4 navigationCamera(vec3 &cad){

    if(cad.x < 0){
        cad.x = 0;
    }
    
    if (cad.y > M_PI_2){//M_PI_2 = M_PI/2
        cad.y = M_PI_2;
    }

    if (cad.y < -M_PI_2){
        cad.y = -M_PI_2;
    }
    return lookAt(
            cad.x * vec3((cos(cad.z)),sin(cad.z),1)*
            vec3((cos(cad.y)),cos(cad.y),sin(cad.y)), // projection longitude latitude.
            vec3(0, 0, 0),                           //where it looks
            vec3(0, 0, 1)                            // head is up
        );

}
