#ifndef OFX_EDGE
#define OFX_EDGE

#include "ofVec3f.h"
#include <vector>
using namespace std;

class ofxVertex;
class ofxFace;

class ofxEdge
{
public:
    ofxEdge(ofxVertex *vertex0, ofxVertex *vertex1, int _id);
    ofxEdge(ofxEdge *ref);
    ~ofxEdge();
    void clearVerts();
    void clearFaces();
    void clear();
    
    void addVertex(ofxVertex *vertex);
    
    void setID(int _id); 
    int getID();
    int id;

    ofVec3f centroid;
    ofVec3f &calculateCentroid();
    ofVec3f &getCentroid();

    ofxEdge *splitEdge(ofxVertex *splitVert); //Returns the other edge
    
    void addFace(ofxFace *face);
    
    ofxVertex *getVertex(int index);
    ofxFace *getFace(int index);
    
    vector<ofxVertex *> verts;  //Two Verts that make the edge
    vector<ofxFace *> faces;    //Faces that share this edge
};

#endif
