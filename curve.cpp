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

vector<curve> genBasicCurve(vector<teamHistory> th, vec3 offset)
{
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
        curve nowCurve = curve(0);
        vertex nowVertex;
        vec3 teamColor = {rand()/float(RAND_MAX),rand()/float(RAND_MAX),rand()/float(RAND_MAX)};
        for (int timeIndex = 0; timeIndex < numberOfGames; timeIndex += 1)
        {
            
            nowVertex.location = vec4(offset.x,

                                     offset.y + float(timeIndex) / numberOfGames,

                                     offset.z + ((19-th[teamIndex].ranks[timeIndex]) / maxRank +
                                                 th[teamIndex].points[timeIndex] / maxPoint) /
                                                    2.0f,

                                     1);
            nowVertex.colors = teamColor;
            nowVertex.UVcoords = vec2(1,1);
            nowCurve.push_back(nowVertex);
        }
        curves[teamIndex] = nowCurve;
    }
    return curves;
}

vector<curve> squareModifier(const vector<curve> &basic, vec4 offset)
{
    vector<curve> newCurves = vector<curve>(0);
    

    for (int curveIndex = 0; curveIndex < basic.size(); curveIndex += 1)
    {
        curve nowCurve = curve(basic[curveIndex].size() * 2);
        for (int pointIndex = 0; pointIndex < basic[curveIndex].size(); pointIndex += 1)
        {
            nowCurve[2*pointIndex] = basic[curveIndex][pointIndex];
            nowCurve[2*pointIndex+1] = basic[curveIndex][pointIndex];

            nowCurve[2 * pointIndex].location -= offset * 0.5f;
            nowCurve[2*pointIndex+1].location += offset * 0.5f;
        }
        newCurves.push_back(nowCurve);
    }
    return newCurves;
}



curve subdivideSimple(const curve &basic, float power, int numberOfSubdivision){

}

vec4 getMeanNANBNormalToCNormalized(const vec4 &A,const vec4 &B ,const vec3 C){
    vec3 sumAB = normalize(B) + normalize(A);
    vec3 vec= (cross(sumAB,vec3(1,0,0)));
    float dist = distance(vec,vec3(0,0,0));
    vec /= dist*dist;//on rajoute ça pour l'inverse proportinalité des vecteurs normés.
    return 2.f * vec4(vec,1);
}

curve skinModifier(const curve &basic, float size)
{
    curve newCurve = curve(basic.size()*2);
    
    vec3 normalVector = cross(vec3(basic[0].location - basic[1].location), vec3(1.f, 0.f, 0.f));
    normalVector = normalize(normalVector);
    normalVector = getMeanNANBNormalToCNormalized(
        vec4(0.f),
        basic[0].location - basic[1].location,
        vec3(-1.f, 0.f, 0.f));

    newCurve[0] = basic[0];
    newCurve[1] = basic[1];

    newCurve[0].location +=  - vec4(normalVector * size * 0.5f, 0);
    newCurve[1].location += + vec4(normalVector * size * 0.5f, 0);
    for (int pointIndex = 1; pointIndex < basic.size(); pointIndex += 1)
    {
        normalVector = getMeanNANBNormalToCNormalized(
        basic[pointIndex].location - basic[pointIndex - 1].location,
        basic[pointIndex+1].location - basic[pointIndex].location,
        vec3(-1.f, 0.f, 0.f));

        newCurve[2 * pointIndex] = basic[pointIndex];
        newCurve[2*pointIndex].location += - vec4(normalVector * size * 0.5f, 0);


        newCurve[2 * pointIndex + 1] = basic[pointIndex];
        newCurve[2*pointIndex + 1].location += + vec4(normalVector * size * 0.5f, 0);
    }

    normalVector = getMeanNANBNormalToCNormalized(
        vec4(0.f),
        basic[basic.size()].location - basic[basic.size() - 1].location,
        vec3(-1.f, 0.f, 0.f));
    newCurve[(basic.size() - 1)*2  ] = basic[basic.size() -1];
    newCurve[(basic.size() - 1)*2+1] = basic[basic.size() -1];
    newCurve[(basic.size() - 1)*2  ].location += - vec4(normalVector * size * 0.5f, 0);
    newCurve[(basic.size() - 1)*2+1].location += + vec4(normalVector * size * 0.5f, 0);

    return newCurve;
}


vector<int> getVBOsSizes(const vector<curve> &curves)
{
    vector<int> VBOsizes = vector<int>(3);
    int totalNumberOfPoints = 0;
    for (int curveIndex = 0; curveIndex < curves.size(); curveIndex += 1)
    {
        totalNumberOfPoints += curves[curveIndex].size();
    }
    VBOsizes[0] = totalNumberOfPoints * 3;
    VBOsizes[1] = totalNumberOfPoints * 3;
    VBOsizes[2] = totalNumberOfPoints * 2;
    return VBOsizes;
}
vector<int> genVBOs(vector<curve> curves, GLfloat XYZcoords[], GLfloat UVcoords[], GLfloat colors[])
{
    vector<int> curveSize = vector<int>(curves.size());
    int totalNumberOfPoints = 0;
    for (int curveIndex = 0; curveIndex < curves.size(); curveIndex += 1)
    {
        curveSize[curveIndex] = curves[curveIndex].size();
    }

    unsigned int buffer3Index = 0;
    unsigned int buffer2Index = 0;
    for (int curveIndex = 0; curveIndex < curves.size(); curveIndex += 1)
    {
        for (int point = 0; point < curves[curveIndex].size(); point += 1)
        {
            XYZcoords[buffer3Index] = curves[curveIndex][point].location.x;
            colors[buffer3Index] = curves[curveIndex][point].colors.r;
            buffer3Index += 1;
            XYZcoords[buffer3Index] = curves[curveIndex][point].location.y;
            colors[buffer3Index] = curves[curveIndex][point].colors.g;
            buffer3Index += 1;
            XYZcoords[buffer3Index] = curves[curveIndex][point].location.z;
            colors[buffer3Index] = curves[curveIndex][point].colors.b;
            buffer3Index += 1;

            UVcoords[buffer2Index] = curves[curveIndex][point].UVcoords.x;
            buffer2Index += 1;
            UVcoords[buffer2Index] = curves[curveIndex][point].UVcoords.y;
            buffer2Index += 1;
        }
    }
    return curveSize;
}