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
    vec3 sumAB = normalize(cross(B,C)) + normalize(cross(A,C));
    vec3 vec= sumAB;
    float dist = distance(vec,vec3(0,0,0));
    //cout<<vec.x<<';'<<vec.y<<';'<<vec.z<<endl;
    vec /= dist*dist;//on rajoute ça pour l'inverse proportinalité des vecteurs normés.
    //cout<<vec.x<<';'<<vec.y<<';'<<vec.z<<endl;
    return 2.f * vec3(vec);
}


vector<curve> genBasicCurves(vector<teamHistory> th, vec3 offset)
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

vector<curve> genNonCrossingCurves(vector<teamHistory> th, vec3 offset, float backOffset)
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
        nowVertex.colors = teamColor;
        nowVertex.UVcoords = vec2(1,1);
        float height1 = 0;
        float height2= 0;
        for (int timeIndex = 0; timeIndex < numberOfGames; timeIndex += 1)
        {
            height1 = ((19-th[teamIndex].ranks[timeIndex]) / maxRank +
                                                 th[teamIndex].points[timeIndex] / maxPoint) /
                                                    2.0f;
            
            height2 = timeIndex+1 < numberOfGames ? ((19-th[teamIndex].ranks[timeIndex+1]) / maxRank +
                                                 th[teamIndex].points[timeIndex+1] / maxPoint) /
                                                    2.0f : height1;
            nowVertex.location = vec3(offset.x,
                                     offset.y + float(2*timeIndex) / numberOfGames,
                                     offset.z + height1);
            nowCurve.push_back(nowVertex);

            nowVertex.location.y += 1.0f/numberOfGames;
            nowCurve.push_back(nowVertex);

            nowVertex.location.y += 0.5f/numberOfGames;
            nowVertex.location.z = offset.z + (height1+height2)/2.0;
            nowVertex.location.x = offset.x + (height1 < height2 ? 1 : -1)*backOffset;
            nowCurve.push_back(nowVertex);
            nowVertex.location.x = offset.x;
        }
        curves[teamIndex] = nowCurve;
    }
    return curves;
}


vector<curve> genWithTangentCurves(vector<teamHistory> th, vec3 offset, float backOffset)
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
        nowVertex.colors = teamColor;
        nowVertex.UVcoords = vec2(1,1);
        nowVertex.tangentCoord = nowVertex.location;
        float height1 = 0;
        float height2= 0;
        for (int timeIndex = 0; timeIndex < numberOfGames; timeIndex += 1)
        {
            //vertex d'état stable (droit)
            height1 = ((19-th[teamIndex].ranks[timeIndex]) / maxRank +
                                                 th[teamIndex].points[timeIndex] / maxPoint) /
                                                    2.0f;
            
            height2 = timeIndex+1 < numberOfGames ? ((19-th[teamIndex].ranks[timeIndex+1]) / maxRank +
                                                 th[teamIndex].points[timeIndex+1] / maxPoint) /
                                                    2.0f : height1;
            nowVertex.location = vec3(offset.x,
                                     offset.y + float(3*timeIndex) / numberOfGames,
                                     offset.z + height1);
            nowVertex.tangentCoord = nowVertex.location + vec3(0.f,0.01,0);
            nowCurve.push_back(nowVertex);

            nowVertex.location.y += 1.0f/numberOfGames;
            nowVertex.tangentCoord = nowVertex.location + vec3(0,0.01,0);// tangente de changement d'état.
            nowCurve.push_back(nowVertex);
            if(th[teamIndex].position[timeIndex] == th[teamIndex].position[(timeIndex+1)%(numberOfGames-1)]){
                continue;
            }
            nowVertex.location.y += 1.0f/numberOfGames;

            nowVertex.location.z = offset.z + (height1+height2)*0.5;
            nowVertex.location.x = offset.x + backOffset*(th[teamIndex].position[timeIndex+1]-th[teamIndex].position[timeIndex]);
            nowVertex.tangentCoord = nowVertex.location;
            

            
            nowVertex.tangentCoord.y += 0.01f;
            nowVertex.tangentCoord.z += 0.5f * (height2 - height1);
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

void cylinderAdd(curve &toModify, circleData c1, circleData c2, int res){
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
        toReturn[segmentIndex*2].colors *= (sin(angleC1)+cos(angleC1)+1)/3;
        toReturn[segmentIndex*2+1] = c2.Center;
        toReturn[segmentIndex*2+1].location += cos(angleC2) * c2.cos;
        toReturn[segmentIndex*2+1].location += sin(angleC2) * c2.sin;
        toReturn[segmentIndex*2+1].colors *= (sin(angleC2)+cos(angleC2)+1)/3;
        angleC1 += angleIncrementC1;
        angleC2 += angleIncrementC2;
    }
    toReturn[toReturn.size()-2] = c1.Center;
    toReturn[toReturn.size()-2].colors = vec3(0);
    toReturn[toReturn.size()-1] = c2.Center;
    toReturn[toReturn.size()-1].colors = vec3(0);
    for(int i = 0; i < toReturn.size(); i += 1){
        toModify.push_back(toReturn[i]);
    }
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
    vec3 normalCosVector = vec3(0,0,size/2);
    vec3 normalSinVector = vec3(size/2,0,0);
    toReturn[0].Center = skeleton[0];
    toReturn[0].cos = normalCosVector;
    toReturn[0].sin = normalSinVector;
    toReturn[0].angleDegrees = 180;

    for (int pointIndex =1; pointIndex < skeleton.size()-1; pointIndex +=1){
        normalCosVector = getMeanNANBNormalToCNormalized(
            skeleton[pointIndex].location - skeleton[pointIndex-1].location,
            skeleton[pointIndex+1].location - skeleton[pointIndex].location,
            vec3(-1.f, 0.f, 0.f));
        normalSinVector = getMeanNANBNormalToCNormalized(
            skeleton[pointIndex].location - skeleton[pointIndex-1].location,
            skeleton[pointIndex+1].location - skeleton[pointIndex].location,
            vec3(0.f, 0.f, 1.f));
        toReturn[pointIndex].Center = skeleton[pointIndex];
        toReturn[pointIndex].cos = normalCosVector * size;
        toReturn[pointIndex].sin = normalSinVector * size;
        toReturn[pointIndex].angleDegrees = 180;
    }
    normalCosVector = vec3(0,0,size/2);
    normalSinVector = vec3(size/2,0,0);

    toReturn[skeleton.size()-1].Center = skeleton[skeleton.size() -1];
    toReturn[skeleton.size()-1].cos = normalCosVector;
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
    curve nowCurve = curve(0);
    curve nowCylinder = curve(0);
    vector<circleData> circleDataNow;
    for (int curveIndex = 0; curveIndex < basic.size(); curveIndex+= 1){
        circleDataNow = genSkinCircles(basic[curveIndex],size);
        nowCurve = cylinder(circleDataNow[0],circleDataNow[1],res);
        for (int pointIndex =0; pointIndex < basic[curveIndex].size()-1; pointIndex +=1){
            cylinderAdd(nowCurve,circleDataNow[pointIndex],circleDataNow[pointIndex+1],res);
        }
        toReturn.push_back(nowCurve);
    }
    return toReturn;
}


vertex getCurveVertex(const curve Curve, int index){
    if (index < 0 && index < Curve.size()){
        return Curve[index];
    }else{
        return {vec3(0.),vec3(0.),vec2(0.)};
    }
}

curve subdivideSimple(const curve &basic, int numberOfSubdivision){
    curve newCurve = curve(0);
    vertex vertexNow;
    for (int vertexIndex = 0; vertexIndex < basic.size()-1; vertexIndex += 1){
        for (float i =0; i < 1; i+= 1.0/numberOfSubdivision){
            vertexNow = (i * basic[vertexIndex+1] + (1-i) * basic[vertexIndex]);
            newCurve.push_back(vertexNow);
        }
    }
    return newCurve;
}

curve subdivideSmooth(const curve &basic, int numberOfSubdivision){//on utilise les courbes de beziers, problème avec les courbe ne se reroisant pas.
    curve newCurve = curve(0);
    vertex vertexNow;
    
    vertex ctrVertex1;
    vertex ctrVertex2;
    float counterI = 1;
    float offset = 0.01;
    for (int vertexIndex = 0; vertexIndex < basic.size()-1; vertexIndex += 1){
        if(basic[vertexIndex+1].location.z == basic[vertexIndex].location.z ){
                newCurve.push_back(basic[vertexIndex]);
                continue;
        }
        ctrVertex1 = basic[vertexIndex+1];
        ctrVertex2 = basic[vertexIndex];

        ctrVertex1.location.y = 0.5 * basic[vertexIndex+1].location.y + 0.5*basic[vertexIndex].location.y;
        ctrVertex2.location.y = 0.5 * basic[vertexIndex+1].location.y + 0.5*basic[vertexIndex].location.y;

        ctrVertex2.UVcoords.y *= 0.5;
        for (float i =0; i < 1; i+= 1.0/numberOfSubdivision){
            counterI = 1-i;


            vertexNow = i*i*i * basic[vertexIndex+1] +
                        3 * i *i * counterI * ctrVertex1 +
                        3 * i * counterI * counterI * ctrVertex2 + 
                        counterI * counterI * counterI * basic[vertexIndex];
            
            /*cout<<i<<","<<counterI<<","<<i*i*i+
                        3 * i *i * counterI+
                        3 * i * counterI * counterI+ 
                        counterI * counterI * counterI<<
                        "\n{x,y,z} = {"
                        <<vertexNow.location.x<<','
                        <<vertexNow.location.y<<','
                        <<vertexNow.location.z<<'}'
                        <<endl;*/
            
            newCurve.push_back(vertexNow);
        }
    }
    return newCurve;
}


curve subdivideSmoothTangents(const curve &basic, int numberOfSubdivision){//on utilise les courbes de beziers, problème avec les courbe ne se reroisant pas.
    curve newCurve = curve(0);
    vertex vertexNow;
    
    vertex ctrVertex1;
    vertex ctrVertex2;
    float counterI = 1;
    float offset = 0.01;
    float incrI;
    for (int vertexIndex = 0; vertexIndex < basic.size()-1; vertexIndex += 1){
        incrI = ((basic[vertexIndex+1].location.z != basic[vertexIndex].location.z) ? 1 : 0);
        
        
        ctrVertex1 = basic[vertexIndex+1];
        ctrVertex1.location = 2.f * ctrVertex1.location - ctrVertex1.tangentCoord;
        ctrVertex2 = basic[vertexIndex];
        ctrVertex2.location = ctrVertex2.tangentCoord; 

        
        for (float i =0; i < 1; i+= 1.0/(numberOfSubdivision*incrI)){
            counterI = 1-i;


            vertexNow = i*i*i * basic[vertexIndex+1] +
                        3 * i *i * counterI * ctrVertex1 +
                        3 * i * counterI * counterI * ctrVertex2 + 
                        counterI * counterI * counterI * basic[vertexIndex];
            
            newCurve.push_back(vertexNow);
        }
    }
    return newCurve;
}


vector<curve> subdivideSimpleModifier(const vector<curve> &basic, int numberOfSubdivisions){
    vector<curve> toReturn = vector<curve>(0);
    for (int i = 0; i < basic.size(); i += 1){
        toReturn.push_back(subdivideSimple(basic[i],numberOfSubdivisions));
    }
    return toReturn;
}

vector<curve> subdivideSmoothModifier(const vector<curve> &basic, int numberOfSubdivisions){
    vector<curve> toReturn = vector<curve>(0);
    for (int i = 0; i < basic.size(); i += 1){
        toReturn.push_back(subdivideSmooth(basic[i],numberOfSubdivisions));
    }
    return toReturn;
}

vector<curve> subdivideTangentsModifier(const vector<curve> &basic, int numberOfSubdivisions){
    vector<curve> toReturn = vector<curve>(0);
    for (int i = 0; i < basic.size(); i += 1){
        toReturn.push_back(subdivideSmoothTangents(basic[i],numberOfSubdivisions));
    }
    return toReturn;
}

curve skinModifierOne(const curve &basic, float size)
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

vector<curve> skinModifier(const vector<curve> &basic, float size){
    vector<curve> toReturn = vector<curve>(0);
    for (int i = 0; i < basic.size(); i += 1){
        toReturn.push_back(skinModifierOne(basic[i],size));
    }
    return toReturn;
    
}

void pushHalfCylinder(vertex orthCircleA1, vertex orthCircleA2, vertex orthCircleB1,
 vertex orthCircleB2,vec3 offsetPosA, vec3 offsetPosB, int N,float maxAngleDegrees, curve &toAddOn){
    cout<<"pushHalfCylinder"<<endl;
    float angle = 0;
    float angleIncrement = M_PI* maxAngleDegrees/(180.f * N);
    vertex vertexNow;
    for (int segment = 0; segment <= N; segment+=1){
        
        toAddOn.push_back(glm::sin(angle) * orthCircleA1 + glm::cos(angle) * orthCircleA2);
        toAddOn[toAddOn.size()-1].location += vec3(offsetPosA);
        //cout<<(glm::sin(angle) * orthCircleA1 + glm::cos(angle) * orthCircleA2).location.x<<','<<(glm::sin(angle) * orthCircleA1 + glm::cos(angle) * orthCircleA2).location.y<<
        //','<<(glm::sin(angle) * orthCircleA1 + glm::cos(angle) * orthCircleA2).location.z<<endl;
        toAddOn.push_back(glm::sin(angle) * orthCircleB1 + glm::cos(angle) * orthCircleB2);
        toAddOn[toAddOn.size()-1].colors *= glm::sin(angle);
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
    vector<int> VBOsizes = vector<int>(4);
    int totalNumberOfPoints = 0;
    for (int curveIndex = 0; curveIndex < curves.size(); curveIndex += 1)
    {
        totalNumberOfPoints += curves[curveIndex].size();
    }
    VBOsizes[0] = totalNumberOfPoints * 3;
    VBOsizes[1] = totalNumberOfPoints * 3;
    VBOsizes[2] = totalNumberOfPoints * 2;
    VBOsizes[3] = totalNumberOfPoints;
    return VBOsizes;
}
vector<int> genVBOs(vector<curve> curves, GLfloat XYZcoords[], GLfloat UVcoords[], GLfloat colors[], GLint teamIndex[])
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
            teamIndex[point] = curveIndex;
            
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