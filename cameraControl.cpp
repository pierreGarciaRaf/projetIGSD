#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

using namespace glm;
#include "cameraControl.hpp"
mat4 navigationCamera(float angleZ, float angleY, float  distance){
    return lookAt(
            vec3(distance*(cos(angleZ)+cos(angleY)),distance*sin(angleZ),distance*sin(angleY)), // where is the camara
            vec3(0, 0, 0),                           //where it looks
            vec3(0, 0, 1)                            // head is up
        );

}
