/**
 * Contains the general structure of points coordinates for the teams.
 * the three first coordinates are the usual ones, x, y & z.
 * the last one corresponds to the weight of this point.
 **/
struct curve{
    vector<vec4> xYZSCoords;
    vector<vec2> uVcoords;
    vector<vec3> colors;
};

/**
 * from a teamhistory vector generates a basic curve (simple strings of points.)
 **/
vector<curve> genBasicCurve(vector<teamHistory> th);

/**
 * converts the vector to a VAO :
 *  @return vector<int>:
 *              the triangle strips sizes
 * @param curves:
 *              the curves it needs to convert (separation in VAO will be done using this)
 * @param XYZcoords:
 *              the VBO of coordinates it needs to create.
 * @param UVcoords:
 *              the VBO for texture it needs to create. (optional)
 * @param colors:
 *              the VBO for vertex colors it needs to create.
 * 
 **/


vector<int> genBasicVBOs(vector<curve> curves,  GLfloat *XYZcoords, GLfloat *UVcoords, GLfloat *colors);
