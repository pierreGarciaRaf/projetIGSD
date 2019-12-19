#ifndef CURVE
#define CURVE

/**
 * Contains the general structure of points coordinates for the teams.
 * the three first coordinates are the usual ones, x, y & z.
 * the last one corresponds to the weight of this point.
 * UV & colors are pretty evident.
 **/
struct vertex{
    vec3 location;
    vec3 colors;
    vec2 UVcoords;
};


using curve = vector<vertex>;

/**
 * from a teamhistory vector generates a basic curve (simple strings of points.)
 * @param th teamhistory vector initialized with fileReader functions.
 * @param offset where is the origin of the curve.
 **/
vector<curve> genBasicCurves(vector<teamHistory> th, vec3 offset);


vector<curve> squareModifier(const vector<curve> &basic, vec3 offset);

vector<curve> squareCylinderModifier(const vector<curve> &basic, vec3 offset,int res);

curve skinModifier(const curve &basic, float size);

vector<curve> skinCylinderModifier(const vector<curve> &basic,float size, int res);

curve cylinderModifier(const curve &basic, float size, int edgeNumber);

vector<int> getVBOsSizes(const vector<curve> &curves);


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
vector<int> genVBOs(vector<curve> curves,  GLfloat XYZcoords[], GLfloat UVcoords[], GLfloat colors[]);
#endif