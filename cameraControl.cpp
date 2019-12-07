#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

using namespace glm;
#include "cameraControl.hpp"
mat4 navigationCamera(vec3 cameraAnglesDistance){
    
    float angleZ = cameraAnglesDistance.z;
    float angleY = cameraAnglesDistance.y;
    float distance = cameraAnglesDistance.x;
    return lookAt(
            vec3(distance*(cos(angleZ)),distance*sin(angleZ),1)* // where is the camara
            vec3(distance*(cos(angleY)),1,distance*sin(angleY)), // where is the camara
            vec3(0, 0, 0),                           //where it looks
            vec3(0, 0, 1)                            // head is up
        );

}
