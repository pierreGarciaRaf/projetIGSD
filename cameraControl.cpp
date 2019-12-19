#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

using namespace glm;
#include "cameraControl.hpp"
#define PI_OVER_2_LIMIT  1.570794 //reduced for limits problem/
mat4 navigationCamera(vec3 &cad){

    if(cad.x < 0){
        cad.x = 0;
    }
    
    if (cad.y > PI_OVER_2_LIMIT){//M_PI_2 = M_PI/2
        cad.y = PI_OVER_2_LIMIT;
    }

    if (cad.y < -PI_OVER_2_LIMIT){
        cad.y = -PI_OVER_2_LIMIT;
    }
    return lookAt(
            cad.x * vec3((cos(cad.z)),sin(cad.z),1)*
            vec3((cos(cad.y)),cos(cad.y),sin(cad.y)), // projection longitude latitude.
            vec3(0, 0, 0),                           //where it looks
            vec3(0, 0, 1)                            // head is up
        );

}
