#include <vector>
#include <string>
#include <iostream>


#include <glm/glm.hpp>
#include <OpenGL/gl.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;


#include "fileReader.hpp"
#include "curve.hpp"


vector<curve> genBasicCurve(vector<teamHistory> th,vec3 offset){
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
        vector<vec3> nowColors = vector<vec3>(0);
        vector<vec4> nowCoords = vector<vec4>(0);
        for (int timeIndex = 0; timeIndex < numberOfGames; timeIndex +=1){
            nowCoords.push_back(vec4(offset.x,

                offset.y + float(timeIndex)/numberOfGames,

                offset.z + (th[teamIndex].ranks[timeIndex]/maxRank +
                th[teamIndex].points[timeIndex]/maxPoint)/2.0f,
                
                1));
            nowColors.push_back(vec3(1,1,1));
            }
        curves[teamIndex].xYZSCoords = nowCoords;
        curves[teamIndex].uVcoords = vector<vec2>(numberOfGames);
        curves[teamIndex].colors = nowColors;
    }
    return curves;
}

vector<curve> squareModifier(const vector<curve> &basic, vec4 offset){
    vector<curve> newCurves = vector<curve>(0);
    for (int curveIndex = 0; curveIndex < basic.size(); curveIndex+=1){
        
        vector<vec4> newPoints = vector<vec4>(basic[curveIndex].xYZSCoords.size()*2);
        vector<vec3> newColors = vector<vec3>(basic[curveIndex].xYZSCoords.size()*2);
        vector<vec2> newUvs = vector<vec2>(basic[curveIndex].uVcoords.size()*2);
        for (int pointIndex = 0; pointIndex < basic[curveIndex].xYZSCoords.size(); pointIndex +=1){
            newPoints[2*pointIndex] = basic[curveIndex].xYZSCoords[pointIndex]-offset*0.5f;
            newPoints[2*pointIndex+1] = basic[curveIndex].xYZSCoords[pointIndex]+offset*0.5f;

            newColors[2*pointIndex] = basic[curveIndex].colors[pointIndex];
            newColors[2*pointIndex+1] = basic[curveIndex].colors[pointIndex];

            newUvs[2*pointIndex] = basic[curveIndex].uVcoords[pointIndex];
            newUvs[2*pointIndex+1] = basic[curveIndex].uVcoords[pointIndex];
        }
        curve newCurve = curve{newPoints,newUvs,newColors};
        newCurves.push_back(newCurve);
    }
    return newCurves;
}


vector<int> getVBOsSizes(const vector<curve> &curves){
    vector<int> VBOsizes = vector<int>(3);
    int totalNumberOfPoints = 0;
    for (int curveIndex = 0; curveIndex < curves.size(); curveIndex+= 1){
        totalNumberOfPoints += curves[curveIndex].xYZSCoords.size();
    }
    VBOsizes[0] = totalNumberOfPoints * 3;
    VBOsizes[1] = totalNumberOfPoints * 3;
    VBOsizes[2] = totalNumberOfPoints * 2;
    return VBOsizes;
}
vector<int> genVBOs(vector<curve> curves,  GLfloat XYZcoords[], GLfloat UVcoords[], GLfloat colors[]){
    vector<int> curveSize = vector<int>(curves.size());
    int totalNumberOfPoints = 0;
    for (int curveIndex = 0; curveIndex < curves.size(); curveIndex+= 1){
        curveSize[curveIndex] = curves[curveIndex].xYZSCoords.size();
    }

    unsigned int buffer3Index = 0;
    unsigned int buffer2Index = 0;
    for (int curveIndex = 0; curveIndex < curves.size(); curveIndex+= 1){
        for (int point = 0; point < curves[curveIndex].xYZSCoords.size(); point+= 1){
            XYZcoords[buffer3Index] = curves[curveIndex].xYZSCoords[point].x;
            colors[buffer3Index] = curves[curveIndex].colors[point].r;
            buffer3Index+=1;
            XYZcoords[buffer3Index] = curves[curveIndex].xYZSCoords[point].y;
            colors[buffer3Index] = curves[curveIndex].colors[point].g;
            buffer3Index+=1;
            XYZcoords[buffer3Index] = curves[curveIndex].xYZSCoords[point].z;
            colors[buffer3Index] = curves[curveIndex].colors[point].b;
            buffer3Index+=1;

            UVcoords[buffer2Index] = curves[curveIndex].uVcoords[point].x;
            buffer2Index+=1;
            UVcoords[buffer2Index] = curves[curveIndex].uVcoords[point].y;
            buffer2Index+=1;

        }
    }
    return curveSize;
}