#version 330 core

smooth in vec3 colort;
out vec4 color;

void main(){
  color = vec4(1-colort.r, 1-colort.g, 1-color.b, 1.0);
}