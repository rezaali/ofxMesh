#include "ofxEdge.h"
#include "ofxVertex.h"
#include "ofMath.h"

ofxEdge::ofxEdge(ofxVertex *vertex0, ofxVertex *vertex1, int _id)
{
    id = _id;
    vertex0->addEdge(this);
    vertex1->addEdge(this);
    addVertex(vertex0);
    addVertex(vertex1);
    calculateCentroid();
}

void ofxEdge::addVertex(ofxVertex *vertex)
{
    verts.push_back(vertex);
}

ofxEdge::~ofxEdge()
{
    clear(); 
}

void ofxEdge::clearFaces()
{
    faces.clear();
}

void ofxEdge::clearVerts()
{
    verts.clear();
}

void ofxEdge::clear()
{
    clearVerts();
    clearFaces();
}

int ofxEdge::getID()
{
    return id;
}

void ofxEdge::addFace(ofxFace *face)
{    
    faces.push_back(face);    
}

ofxVertex *ofxEdge::getVertex(int index)
{
    return verts[index%verts.size()]; 
}

ofxFace *ofxEdge::getFace(int index)
{
    return faces[index%faces.size()];
}

ofVec3f &ofxEdge::calculateCentroid()
{
    centroid = ofVec3f(0,0,0);
    for(vector<ofxVertex *>::iterator it = verts.begin(); it != verts.end(); ++it)
    {
        ofxVertex *v = (*it);
        if(v != NULL)
        {
            centroid += v->getPos();
        }
    }
    centroid /= (float)verts.size();
    return centroid;
}

ofVec3f &ofxEdge::getCentroid()
{
    return centroid; 
}

ofxEdge * ofxEdge::splitEdge(ofxVertex *splitVert)
{
    ofxVertex *startPoint = getVertex(0);
    ofxVertex *midPoint = splitVert;
    ofxVertex *endPoint = getVertex(1);
    
    verts.clear();
    verts.push_back(startPoint);
    verts.push_back(midPoint);
    calculateCentroid();
    
    ofxEdge *newEdge = new ofxEdge(midPoint, endPoint, -1);
    midPoint->addEdge(this);
    endPoint->removeEdge(this);
    endPoint->addEdge(newEdge);
    return newEdge;
}

