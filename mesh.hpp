#ifndef MESH
#define MESH
#include lib.hpp

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct Texture {
    unsigned int id;
    char type;
};


class Mesh {
    public:
        /*  Mesh Data  */
        vector<Vertex> vertices;
        vector<unsigned int> indices;  
        vector<Texture> textures;
        /*  Functions  */
        Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);
        void Draw(Shader shader);
    private:
        /*  Render data  */
        unsigned int VAO, VBO, EBO;
        /*  Functions    */
        void setupMesh();
};


#endif