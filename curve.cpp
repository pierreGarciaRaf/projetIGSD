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
struct circleData{
    vertex Center;
    vec3 cos;
    vec3 sin;
    float angleDegrees;
};

vertex operator +(const vertex& x, const vertex& y) {
    return vertex{x.location + y.location,x.colors + y.colors,x.UVcoords + y.UVcoords};
}

vertex operator *(const float& x, const vertex& y) {
    return vertex{x* y.location,x* y.colors,x* y.UVcoords};
}

vertex operator -(const vertex& x, const vertex& y) {
    return x + -1.f * y;
}



vec3 getMeanNANBNormalToCNormalized(const vec3 &A,const vec3 &B ,const vec3 C){
    vec3 sumAB = normalize(B) + normalize(A);
    vec3 vec= {0,sumAB.z,-sumAB.y};
    float dist = distance(vec,vec3(0,0,0));
    cout<<vec.x<<';'<<vec.y<<';'<<vec.z<<endl;
    vec /= dist*dist;//on rajoute ça pour l'inverse proportinalité des vecteurs normés.
    cout<<vec.x<<';'<<vec.y<<';'<<vec.z<<endl;
    return 2.f * vec3(vec);
}


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
            
            nowVertex.location = vec3(offset.x,
                                     offset.y + float(2*timeIndex) / numberOfGames,
                                     offset.z + ((19-th[teamIndex].ranks[timeIndex]) / maxRank +
                                                 th[teamIndex].points[timeIndex] / maxPoint) /
                                                    2.0f);
            nowVertex.colors = teamColor;
            nowVertex.UVcoords = vec2(1,1);
            nowCurve.push_back(nowVertex);

            nowVertex.location = vec3(offset.x,
                                     offset.y + float(2*timeIndex+1) / numberOfGames,
                                     offset.z + ((19-th[teamIndex].ranks[timeIndex]) / maxRank +
                                                 th[teamIndex].points[timeIndex] / maxPoint) /
                                                    2.0f);
            nowVertex.colors = teamColor;
            nowVertex.UVcoords = vec2(1,1);
            nowCurve.push_back(nowVertex);
        }
        curves[teamIndex] = nowCurve;
    }
    return curves;
}

vector<curve> squareModifier(const vector<curve> &basic, vec3 offset)
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




float angleIncrement(circleData const &c, int res){
    return M_PI * c.angleDegrees /(180 * res);
}

curve cylinder(circleData c1, circleData c2, int res){
    curve toReturn = curve(res * 2 + 6);
    toReturn[0] = c1.Center;
    toReturn[1] = c2.Center;
    float angleC1 = 0;
    float angleC2 = 0;
    float angleIncrementC1 = angleIncrement(c1,res);
    float angleIncrementC2 = angleIncrement(c2, res);
    for (int segmentIndex = 1; segmentIndex <= res+1; segmentIndex += 1){
        toReturn[segmentIndex*2] = c1.Center;
        toReturn[segmentIndex*2].location += cos(angleC1) * c1.cos;
        toReturn[segmentIndex*2].location += sin(angleC1) * c1.sin;
        toReturn[segmentIndex*2+1] = c2.Center;
        toReturn[segmentIndex*2+1].location += cos(angleC2) * c2.cos;
        toReturn[segmentIndex*2+1].location += sin(angleC2) * c2.sin;
        angleC1 += angleIncrementC1;
        angleC2 += angleIncrementC2;
    }
    toReturn[toReturn.size()-2] = c1.Center;
    toReturn[toReturn.size()-1] = c2.Center;
    return toReturn;
}

vector<circleData> genStraightCircles(const curve &skeleton, vec3 offset){
    vector<circleData> toReturn = vector<circleData>(skeleton.size());
    for (int pointIndex =0; pointIndex < skeleton.size(); pointIndex +=1){
        toReturn[pointIndex].Center = skeleton[pointIndex];
        
        toReturn[pointIndex].angleDegrees = 180;
        toReturn[pointIndex].cos = offset;
        toReturn[pointIndex].sin = vec3(1,0,0) * distance(vec3(0),offset);
    }
    return toReturn;
}

vector<circleData> genSkinCircles(const curve &skeleton, float size){
    vector<circleData> toReturn = vector<circleData>(skeleton.size());
    vec3 normalVector = vec3(0,0,-size/2);
    toReturn[0].Center = skeleton[0];
    toReturn[0].cos = normalVector;
    toReturn[0].sin = vec3(size/2,0,0);
    toReturn[0].angleDegrees = 180;

    for (int pointIndex =1; pointIndex < skeleton.size()-1; pointIndex +=1){
        normalVector = getMeanNANBNormalToCNormalized(
            skeleton[pointIndex].location - skeleton[pointIndex-1].location,
            skeleton[pointIndex+1].location - skeleton[pointIndex].location,
            vec3(-1.f, 0.f, 0.f));
        toReturn[pointIndex].Center = skeleton[pointIndex];
        toReturn[pointIndex].cos = normalVector * size;
        toReturn[pointIndex].sin = vec3(1,0,0) * size;
        toReturn[pointIndex].angleDegrees = 180;
    }
    normalVector = vec3(0,0,-size/2);

    toReturn[skeleton.size()-1].Center = skeleton[skeleton.size() -1];
    toReturn[skeleton.size()-1].cos = normalVector;
    toReturn[skeleton.size()-1].sin = vec3(size/2,0,0);
    toReturn[skeleton.size()-1].angleDegrees = 180;
    return toReturn;

}


vector<curve> squareCylinderModifier(const vector<curve> &basic,vec3 offset, int res){
    vector<curve> toReturn = vector<curve>(0);
    vector<circleData> circleDataNow;
    for (int curveIndex = 0; curveIndex < basic.size(); curveIndex+= 1){
        circleDataNow = genStraightCircles(basic[curveIndex],offset);
        for (int pointIndex =0; pointIndex < basic[curveIndex].size()-1; pointIndex +=1){
            toReturn.push_back(cylinder(circleDataNow[pointIndex],circleDataNow[pointIndex+1],res));
        }
    }
    return toReturn;
}

vector<curve> skinCylinderModifier(const vector<curve> &basic,float size, int res){
    vector<curve> toReturn = vector<curve>(0);
    vector<circleData> circleDataNow;
    for (int curveIndex = 0; curveIndex < basic.size(); curveIndex+= 1){
        circleDataNow = genSkinCircles(basic[curveIndex],size);
        for (int pointIndex =0; pointIndex < basic[curveIndex].size()-1; pointIndex +=1){
            toReturn.push_back(cylinder(circleDataNow[pointIndex],circleDataNow[pointIndex+1],res));
        }
    }
    return toReturn;
}



curve subdivideSimple(const curve &basic, float power, int numberOfSubdivision){

}

curve skinModifier(const curve &basic, float size)
{
    curve newCurve = curve(basic.size()*2);
    
    vec3 normalVector;
    normalVector = getMeanNANBNormalToCNormalized(
        vec3(0.f),
        basic[0].location - basic[1].location,
        vec3(-1.f, 0.f, 0.f));

    newCurve[0] = basic[0];
    newCurve[1] = basic[1];

    newCurve[0].location +=  - vec3(normalVector * size);
    newCurve[1].location += + vec3(normalVector * size);
    for (int pointIndex = 1; pointIndex < basic.size(); pointIndex += 1)
    {
        normalVector = getMeanNANBNormalToCNormalized(
        basic[pointIndex].location - basic[pointIndex - 1].location,
        basic[pointIndex+1].location - basic[pointIndex].location,
        vec3(-1.f, 0.f, 0.f));

        newCurve[2 * pointIndex] = basic[pointIndex];
        newCurve[2*pointIndex].location += - vec3(normalVector * size);


        newCurve[2 * pointIndex + 1] = basic[pointIndex];
        newCurve[2*pointIndex + 1].location += + vec3(normalVector * size);
    }

    normalVector = getMeanNANBNormalToCNormalized(
        vec3(0.f),
        basic[basic.size()-1].location - basic[basic.size() - 2].location,
        vec3(-1.f, 0.f, 0.f));
    newCurve[(basic.size() - 1)*2  ] = basic[basic.size() -1];
    newCurve[(basic.size() - 1)*2+1] = basic[basic.size() -1];
    newCurve[(basic.size() - 1)*2  ].location += - vec3(normalVector * size);
    newCurve[(basic.size() - 1)*2+1].location += + vec3(normalVector * size);

    return newCurve;
}

void pushHalfCylinder(vertex orthCircleA1, vertex orthCircleA2, vertex orthCircleB1,
 vertex orthCircleB2,vec3 offsetPosA, vec3 offsetPosB, int N,float maxAngleDegrees, curve &toAddOn){
    float angle = 0;
    float angleIncrement = M_PI* maxAngleDegrees/(180.f * N);
    vertex vertexNow;
    for (int segment = 0; segment <= N; segment+=1){
        
        toAddOn.push_back(glm::sin(angle) * orthCircleA1 + glm::cos(angle) * orthCircleA2);
        toAddOn[toAddOn.size()-1].location += vec3(offsetPosA);
        cout<<(glm::sin(angle) * orthCircleA1 + glm::cos(angle) * orthCircleA2).location.x<<','<<(glm::sin(angle) * orthCircleA1 + glm::cos(angle) * orthCircleA2).location.y<<
        ','<<(glm::sin(angle) * orthCircleA1 + glm::cos(angle) * orthCircleA2).location.z<<endl;
        toAddOn.push_back(glm::sin(angle) * orthCircleB1 + glm::cos(angle) * orthCircleB2);
        toAddOn[toAddOn.size()-1].location += vec3(offsetPosB);
        angle += angleIncrement;
    }
}

void pushHalfCylinder(const curve &basic, curve &toPushOn, float size, int index, int N, float maxAngle){
    vec3 lastNormalVector = getMeanNANBNormalToCNormalized(
        basic[index-1].location - basic[index].location,
        basic[index].location - basic[index+1].location,
        vec3(-1.f, 0.f, 0.f));
    vec3 newNormalVector = getMeanNANBNormalToCNormalized(
        basic[index].location - basic[index+1].location,
        basic[index+1].location - basic[index+2].location,
        vec3(-1.f, 0.f, 0.f));
    cout<<"--------------------------"<<basic[index].location.y<<endl;
    cout<<"--------------------------"<<basic[index].location.z<<endl;

    vertex lastPerpendicular;
    lastPerpendicular.location = vec3{1,0,0} *size;
    lastPerpendicular.UVcoords = basic[index].UVcoords;
    lastPerpendicular.colors = basic[index].colors;


    vertex newPerpendicular;
    newPerpendicular.location = vec3{1,0,0}*size;
    newPerpendicular.UVcoords = basic[index+1].UVcoords;
    newPerpendicular.colors = basic[index+1].colors;


    vertex lastNormal;
    lastNormal = basic[index];
    lastNormal.location = vec3(lastNormalVector)*size;

    vertex newNormal;
    newNormal = basic[index+1];
    newNormal.location = vec3(newNormalVector)*size;

    pushHalfCylinder(lastNormal,lastPerpendicular,newNormal,lastNormal,
    vec3(basic[index].location), vec3(basic[index+1].location),5, 180, toPushOn);
}

curve cylinderModifier(const curve &basic, float size, int edgeNumber){
    curve cylinderedCurve = curve(0);
    for(int pointIndex = 1; pointIndex < basic.size()-2; pointIndex+=1){
        pushHalfCylinder(basic,cylinderedCurve,size,pointIndex,edgeNumber,180);
    }
    return cylinderedCurve;
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