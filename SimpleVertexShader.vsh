#version 330 core

//Entrees
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexColor_modelspace;
layout(location = 2) in int team_index;

// Sortie
smooth out vec3 colort;

// Params
uniform mat4 modelMatrix;
uniform mat4 modelForTeamToMove;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform int teamToMoveIndex;

void main(){
  if(teamToMoveIndex < 0 || teamToMoveIndex >=20){
    gl_Position = projectionMatrix * viewMatrix * modelMatrix         * vec4(vertexPosition_modelspace, 1.0);
    colort = vertexColor_modelspace;

  }
  else if (team_index == teamToMoveIndex){
    gl_Position = projectionMatrix * viewMatrix * modelForTeamToMove  * vec4(vertexPosition_modelspace, 1.0);
    colort = vertexColor_modelspace;

  }else{
    gl_Position = projectionMatrix * viewMatrix * modelMatrix         * vec4(vertexPosition_modelspace, 1.0);
    colort = vertexColor_modelspace * 0.5;
  }
  
}