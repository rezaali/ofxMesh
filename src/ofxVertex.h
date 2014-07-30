#ifndef OFX_VERTEX
#define OFX_VERTEX

#include <vector>
#include "ofVec3f.h"
#include "ofColor.h"

using namespace std;

class ofxFace;
class ofxEdge;

class ofxVertex
{
public:
    ofxVertex(ofVec3f _pos, int _id);
    ofxVertex(ofxVertex *ref);
    ~ofxVertex();
    void clear();
    
    void addEdge(ofxEdge *edge); 
    void addFace(ofxFace *face); 

    void removeEdge(ofxEdge *edge);
    ofxEdge *getEdge(int index);
    ofxFace *getFace(int index);
    
    vector<ofxVertex *> getNeighbors();     //Oh you fancy huh?
    ofVec3f getNeighborsAveragePos();     
    ofxVertex * getClosestNeighborFrom(ofVec3f _pos);
    
    int getNumEdges();
    int getNumFaces();
    int getID();
    void setID(int _id);
    void setPos(ofVec3f _pos);
    ofVec3f &getPos();
    ofVec3f &getNormal();
    ofVec3f &calculateNormal();

    void setTexcoord(ofVec2f _texcoord);
    ofVec2f &getTexcoord();
    
    ofColor &getColor();
    void setColor(ofColor _color); 
    
    int id;
    ofVec3f pos;
    ofVec3f normal;
    ofVec2f texcoord;
    ofColor color;
    vector<ofxFace *> faces;    //Faces that use this vert
    vector<ofxEdge *> edges;    //Edges that use this vert
};

#endif
