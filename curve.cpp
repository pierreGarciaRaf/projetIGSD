#include <vector>
#include <glm/glm.hpp>

using namespace std;
using namespace glm;

#include <OpenGL/gl.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

#include "curve.hpp"
#include "fileReader.hpp"


vector<curve> genCurveSkeletons(vector<teamHistory> th,vec3 offset){
    float maxRank = 0;
    float maxPoint = 0;
    for (int i = 0; i < th.size(); i += 1)
    {
        if (maxRank < th[i].maxRank)
        {
            maxRank = th[i].maxRank;
        }
        if (maxPoint < th[i].maxPoint)
        {
            maxPoint = th[i].maxPoint;
        }
    }
    int N = th.size();
    int numberOfGames = th[0].points.size();
    vector<curve> curves = vector<curve>(N);
    for (int teamIndex = 0; teamIndex < N; teamIndex++)
    {
        for (int timeIndex = 0; timeIndex < numberOfGames; timeIndex +=1){
            curves[teamIndex].xYZSCoords.push_back(vec4(
                offset.x+(th[teamIndex].ranks[timeIndex]/maxRank +
                th[teamIndex].points[timeIndex]/maxPoint)/2.0f,
                offset.y + float(timeIndex)/numberOfGames,
                offset.z,
                1));
            }
    }
}


vector<int> genBasicVBOs(vector<curve> curves,  GLfloat *XYZcoords[], GLfloat *UVcoords[], GLfloat *colors[]){
    vector<int> VBOsizes = vector<int>(curves.size());
    int totalNumberOfPoints = 0;
    for (int curveIndex = 0; curveIndex < curves.size(); curveIndex+= 1){
        VBOsizes[curveIndex] = curves[curveIndex].xYZSCoords.size();
        totalNumberOfPoints += VBOsizes[curveIndex];
    }

    *XYZcoords = new GLfloat[totalNumberOfPoints*3];
    *UVcoords = new GLfloat[totalNumberOfPoints*2];
    *colors = new GLfloat[totalNumberOfPoints*3];

    for (int curveIndex = 0; curveIndex < curves.size(); curveIndex+= 1){
        for (int point = 0; point < curves[curveIndex].xYZSCoords.size(); point+= 1){

            *XYZcoords[3*point  ] = curves[curveIndex].xYZSCoords[point].x;
            *XYZcoords[3*point+1] = curves[curveIndex].xYZSCoords[point].y;
            *XYZcoords[3*point+2] = curves[curveIndex].xYZSCoords[point].z;

            *UVcoords[2*point  ] = curves[curveIndex].uVcoords[point].x;
            *UVcoords[2*point+1] = curves[curveIndex].uVcoords[point].y;

            *colors[3*point  ] = curves[curveIndex].colors[point].r;
            *colors[3*point+1] = curves[curveIndex].colors[point].g;
            *colors[3*point+2] = curves[curveIndex].colors[point].b;
        }
    }

}