#ifndef OFX_FACE
#define OFX_FACE

#include "ofRectangle.h"
#include <vector>
using namespace std;

class ofxVertex;
class ofxEdge;

class ofxFace
{
public:
    ofxFace(int _id);
    ~ofxFace();
    
    int id;
    int getID();

    void sortPoints();
    void clearVerts();
    void clearEdges();
    void clear();
    void deleteVerts();
    void deleteEdges();
    
    ofxFace *clone();
    
    int getNumVerts();
    int getNumEdges();
    ofRectangle getBoundingRect();
    
    void addEdge(ofxEdge *edge); 
    void addVertex(ofxVertex *vertex); 

    ofxVertex *getVertex(int index);
    ofxEdge *getEdge(int index);
    
    vector<ofxEdge *> edges;        //All edges that make up this edge
    vector<ofxVertex *> verts;      //All verts that make the face
    
    ofVec3f centroid;
    ofVec3f &calculateCentroid();
    ofVec3f &getCentroid();
    
    ofVec3f normal;
    ofVec3f &calculateNormal();
    ofVec3f &getNormal();
};

#endif
