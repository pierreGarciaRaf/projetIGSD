#version 330 core

smooth in vec3 colort;
out vec4 color;

void main(){
  color = vec4(colort.r, colort.g, color.b, 1.0);
}