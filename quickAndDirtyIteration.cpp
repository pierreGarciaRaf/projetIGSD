// sous mac
// >g++ -I/u/usr/local/include/ -lglfw -lGLEW main4.cpp -framework OpenGL  -omain4
// >./main4

// sous linux
// >g++ -I/usr/local/include/ -I/public/ig/glm/ -c main4.cpp  -omain4.o
// >g++ -I/usr/local main4.o -lglfw  -lGLEW  -lGL  -omain4
// >./main4

// Inclut les en-têtes standards
#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
using namespace std;

#include <stdlib.h>
#include <string.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

//my includes
#include "fileReader.hpp"
#include "curve.hpp"

#include "cameraControl.hpp"

bool wasPressingQ;
bool wasPressingD;
#define GLM_FORCE_RADIANS

string dataPath;

// stocke les variables uniformes qui seront communes a tous les vertex dessines
// dans une variable globale CAR
// C'est dans la fonction loadShaders que nous pouvos recupere les bonnes valeurs de pointeur (une fois le shader compile/linke)
// c'est dans le main que nous pouvons donne les bonnes valeurs "au bout du pointeur" pour que les shaders les recoivent
GLint uniform_proj, uniform_view, uniform_model;

GLuint LoadShaders(const char *vertex_file_path, const char *fragment_file_path)
{

    // Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    // Read the Vertex Shader code from the file
    std::string VertexShaderCode;
    std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
    if (VertexShaderStream.is_open())
    {
        std::string Line = "";
        while (getline(VertexShaderStream, Line))
            VertexShaderCode += "\n" + Line;
        VertexShaderStream.close();
    }
    else
    {
        printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
        getchar();
        return 0;
    }

    // Read the Fragment Shader code from the file
    std::string FragmentShaderCode;
    std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
    if (FragmentShaderStream.is_open())
    {
        std::string Line = "";
        while (getline(FragmentShaderStream, Line))
            FragmentShaderCode += "\n" + Line;
        FragmentShaderStream.close();
    }

    GLint Result = GL_FALSE;
    int InfoLogLength;

    // Compile Vertex Shader
    printf("Compiling shader : %s\n", vertex_file_path);
    char const *VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
    glCompileShader(VertexShaderID);

    // Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0)
    {
        std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
        printf("%s\n", &VertexShaderErrorMessage[0]);
    }

    // Compile Fragment Shader
    printf("Compiling shader : %s\n", fragment_file_path);
    char const *FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
    glCompileShader(FragmentShaderID);

    // Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0)
    {
        std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
        printf("%s\n", &FragmentShaderErrorMessage[0]);
    }

    // Link the program
    printf("Linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    uniform_proj = glGetUniformLocation(ProgramID, "projectionMatrix");
    uniform_view = glGetUniformLocation(ProgramID, "viewMatrix");
    uniform_model = glGetUniformLocation(ProgramID, "modelMatrix");

    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0)
    {
        std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
        glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
        printf("%s\n", &ProgramErrorMessage[0]);
    }

    glDetachShader(ProgramID, VertexShaderID);
    glDetachShader(ProgramID, FragmentShaderID);

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    return ProgramID;
}


void setup()
{
    dataPath = "DataProjet2019/data/";
}

int main()
{
    setup();
    vector<teamHistory> teamData = readData(dataPath + "rankspts.csv");
    
    // Initialise GLFW
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    const int N = teamData.size();
    const int numberOfGames = teamData[0].ranks.size();
    
    
    vector<curve> curves=genWithTangentCurves(teamData, vec3(0,-1.f,-0.5f),0.01);
    curves = subdivideTangentsModifier(curves,5);
    curves = skinCylinderModifier(curves,0.01,4);
    

    vector<int> VBOsizes = getVBOsSizes(curves);
    GLfloat g_vertex_buffer_data[VBOsizes[0]];
    GLfloat g_vertex_color_data[VBOsizes[1]];
    GLfloat g_vertex_UV_data[VBOsizes[2]];
    vector<int> numberOfPointPerTeam  = genVBOs(curves, g_vertex_buffer_data,
                                    g_vertex_UV_data,
                                    g_vertex_color_data);
    
    
    
    int nVertex =0;
    for (int i =0; i < numberOfPointPerTeam.size(); i += 1){
        nVertex+=numberOfPointPerTeam[i];
    }


    glfwWindowHint(GLFW_SAMPLES, 4);               // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // On veut OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // Pour rendre MacOS heureux ; ne devrait pas être nécessaire
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // On ne veut pas l'ancien OpenGL

    // Ouvre une fenêtre et crée son contexte OpenGl
    GLFWwindow *window; // (Dans le code source qui accompagne, cette variable est globale)
    window = glfwCreateWindow(1024, 768, "Pierre T  BO (merci Elie, c'est tres gentil).", NULL, NULL);
    if (window == NULL)
    {
        fprintf(stderr,
        "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window); // Initialise GLEW
    glewExperimental = true;        // Nécessaire dans le profil de base
    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }

    // modern OpenGL do not have a default VAO anymore. Even if we don't want to use it
    // we have a create and bind one before playing with buffers !
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // This will identify our vertex buffer
    GLuint vertexbuffer;
    // Generate 1 buffer, put the resulting identifier in vertexbuffer
    glGenBuffers(1, &vertexbuffer);

    // The following commands will talk about our 'vertexbuffer' buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    // Only allocqte memory. Do not send yet our vertices to OpenGL.
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data) + sizeof(g_vertex_color_data), 0, GL_STATIC_DRAW);

    // send vertices in the first part of the buffer
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(g_vertex_buffer_data), g_vertex_buffer_data);

    // send vertices in the second part of the buffer
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), sizeof(g_vertex_color_data), g_vertex_color_data);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Assure que l'on peut capturer la touche d'échappement enfoncée ci-dessous
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    GLuint programID = LoadShaders("SimpleVertexShader.vsh", "SimpleFragmentShader.fsh");

    float angle = 0.0f;
    float angleIncrement = 0;
    double xpos = 0;
    double ypos = 0;
    double lastXpos = 0;
    double lastYpos = 0;
    double incrXpos = 0;
    double incrYpos = 0;
    float posY;
    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    glClearColor(1,1,1,1);
    glEnable(GL_DEPTH_TEST);  
    vec3 cameraAnglesDistance = {1,0,0};
    vec3 lastCameraAnglesDistance = {1,0,0};
    double lastTime = glfwGetTime();
    int nbFrames = 0;
    double delta = 0;
    int lastX=0;
    int lastY=0;
    do
    {
        double currentTime = glfwGetTime();
        nbFrames++;
        if ( currentTime- lastTime >= 1.0 ){ // If last prinf() was more than 1 sec ago
             // printf and reset timer
            delta = (currentTime -lastTime)/nbFrames;

            printf("%f ms/frame\n", 1000.0/double(nbFrames));
            nbFrames = 0;
            lastTime += 1.0;
        }
        // clear before every draw 1
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use our shader
        glUseProgram(programID);

        glm::mat4 projectionMatrix = glm::perspective(45.0f, 1024.0f / 768.0f, 0.1f, 200.0f);
        glm::mat4 viewMatrix = navigationCamera(cameraAnglesDistance);
        mat4 modelMatrix = glm::mat4(1.0);
        modelMatrix = translate(modelMatrix, vec3(0, 0, posY));
        glUniformMatrix4fv(uniform_proj, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
        glUniformMatrix4fv(uniform_view, 1, GL_FALSE, glm::value_ptr(viewMatrix));
        glUniformMatrix4fv(uniform_model, 1, GL_FALSE, glm::value_ptr(modelMatrix));

        // 1rst attribute buffer : vertices
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
            0,        // attribute 0. No particular reason for 0, but must match the layout in the shader.
            3,        // size
            GL_FLOAT, // type
            GL_FALSE, // normalized?
            0,        // stride
            (void *)0 // array buffer offset
        );
        glEnableVertexAttribArray(0);

        glVertexAttribPointer( // same thing for the colors
            1,
            3,
            GL_FLOAT,
            GL_FALSE,
            0,
            (void *)sizeof(g_vertex_buffer_data));
        glEnableVertexAttribArray(1);
        
        int firstVertex = 0;
        for (int triangleStripIndex = 0; triangleStripIndex < numberOfPointPerTeam.size(); triangleStripIndex +=1){
            glDrawArrays(GL_TRIANGLE_STRIP, firstVertex, numberOfPointPerTeam[triangleStripIndex]); 
            firstVertex += numberOfPointPerTeam[triangleStripIndex];
        }
         
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();


        /**
         * Camera movements
         * 
         **/
        if(glfwGetKey(window, GLFW_KEY_W)==GLFW_PRESS){
            cameraAnglesDistance.x -= delta * 2;
        }
        if(glfwGetKey(window, GLFW_KEY_S)==GLFW_PRESS){
            cameraAnglesDistance.x += delta * 2;
        }
        if(glfwGetKey(window, GLFW_KEY_D)==GLFW_PRESS){
            cameraAnglesDistance.z += delta * 2;
        }
        if(glfwGetKey(window, GLFW_KEY_Q)==GLFW_PRESS){
            cameraAnglesDistance.z -= delta * 2;
        }
        if(glfwGetKey(window, GLFW_KEY_SPACE)==GLFW_PRESS){
            cameraAnglesDistance.y += delta * 2;
        }
        if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)==GLFW_PRESS){
            cameraAnglesDistance.y -= delta * 2;
        }
        
        glfwGetCursorPos(window, &xpos, &ypos);
        
        if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1)){
            cameraAnglesDistance.z = lastCameraAnglesDistance.z + (xpos-lastX)/400;
            cameraAnglesDistance.y = lastCameraAnglesDistance.y + (ypos-lastY)/350;
        }else{
            lastX = xpos;
            lastY = ypos;
            lastCameraAnglesDistance = cameraAnglesDistance;
        }
        
        

    } // Vérifie si on a appuyé sur la touche échap (ESC) ou si la fenêtre a été fermée
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0);
}

