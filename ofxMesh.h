#ifndef OFX_MESH
#define OFX_MESH

#include <vector>
#include <map>
#include "ofMesh.h"
#include "ofRectangle.h"

class ofxVertex;
class ofxFace;
class ofxEdge;

class ofxMesh
{
public:
    ofxMesh();
    ~ofxMesh();
    
    void init(); 
    void clear();
    
    void calculateBoundingBox(); 
    ofVec3f &calculateCentroid();
    ofVec3f &getCentroid();
    void center();
    float getWidth();
    float getHeight();
    float getDepth();
    void scale(float _x, float _y, float _z);
    void translate(float _x, float _y, float _z);
    
    void setScaleFactor(float _scaleFactor);
    void loadFromMesh(ofMesh &mesh);
    void loadFromObj(string path);
    void computeVertexNormals(ofxMesh *mesh);
    

    ofRectangle getBoundingRect();
    ofPoint getMinPoint();
    ofPoint getMaxPoint();

    ofxVertex *getOppositeVertex(ofxEdge *e, ofxFace *f);
    bool areEdgesOrdered(ofxFace *face);
    bool areEdgesOrdered(vector<ofxEdge*> _edges);
    void orderEdges(ofxFace *f);
    vector<ofxEdge *> orderEdges(vector<ofxEdge *> _edges);
    
    ofMesh generateMesh();
    ofxMesh *clone();
    ofxMesh *smooth(int level = 1);
    
    void draw(); //Assumes a Triangulated Mesh
    void drawVerticesNumbers();
    void drawVertices();
    void drawEdges();
    void drawFaces();
    void drawFaceNormals(float scale = 1.0);
    void drawVertexNormals(float scale = 1.0);
    void drawFace(int index);
    void drawBoundingBox(); 
    
    int getNumVertices();
    int getNumEdges();
    int getNumFaces();
    
    ofxVertex *getVertex(int index);
    ofxFace *getFace(int index);
    ofxEdge *getEdge(int index);
    
    ofxEdge *getEdge(ofIndexType i0, ofIndexType i1);
    vector<ofxVertex *> verts;
    vector<ofxFace *> faces;
    vector<ofxEdge *> edges;
    
    float scaleFactor;
    
    float meshWidth;
    float meshHeight;
    float meshDepth;
    
    ofVec3f centroid;
    
    ofxEdge *splitEdge(ofxEdge *e, ofxVertex *ep);
    
private:
    map<string, ofxEdge *> edgeMap; 
};

#endif