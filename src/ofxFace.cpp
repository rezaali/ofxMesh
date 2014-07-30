#include "ofxFace.h"
#include "ofxVertex.h"
#include "ofxEdge.h"

ofxFace::ofxFace(int _id)
{
    id = _id;
}

ofxFace::~ofxFace()
{
    clear(); 
}

int ofxFace::getID()
{
    return id;
}

ofRectangle ofxFace::getBoundingRect()
{
    float minX = 100000;
    float maxX = -100000;

    float minY = 100000;
    float maxY = -100000;
    
    for(vector<ofxVertex *>::iterator it = verts.begin(); it != verts.end(); ++it)
    {
        ofxVertex *v = (*it);
        float x = v->pos.x;
        float y = v->pos.y;
        if(x < minX)
        {
            minX = x; 
        }
        
        if(x > maxX)
        {
            maxX = x;
        }
        
        if(y < minY)
        {
            minY = y;
        }
        
        if(y > maxY)
        {
            maxY = y;
        }
    }
    
    return ofRectangle(minX, minY, maxX - minX, maxY - minY);
}

void ofxFace::clearVerts()
{
    verts.clear();
}

void ofxFace::clearEdges()
{
    edges.clear();
}

void ofxFace::clear()
{
    clearVerts();
    clearEdges();
}

void ofxFace::addVertex(ofxVertex *vertex)
{
    vertex->addFace(this);
    verts.push_back(vertex);
    if(verts.size() == 3)
    {
        calculateCentroid();
        calculateNormal();
    }
}

void ofxFace::addEdge(ofxEdge *edge)
{
    edge->addFace(this); 
    edges.push_back(edge); 
}

ofVec3f &ofxFace::getCentroid()
{
    return centroid;
}

ofVec3f &ofxFace::calculateCentroid()
{
    centroid = ofPoint(0,0,0);
    vector<ofxVertex *>::iterator it = verts.begin();
    vector<ofxVertex *>::iterator eit = verts.end();
    for(; it != eit; ++it)
    {
        ofxVertex *v = (*it);
        centroid += v->getPos(); 
    }
    centroid/=(float)verts.size();
    return centroid;
}

int ofxFace::getNumVerts()
{
    return verts.size();
}

int ofxFace::getNumEdges()
{
    return edges.size(); 
}

ofxVertex *ofxFace::getVertex(int index)
{
    return verts[index%verts.size()]; 
}

ofxEdge *ofxFace::getEdge(int index)
{
    return edges[index%edges.size()];
}

void ofxFace::deleteVerts()
{
    for(vector<ofxVertex *>::iterator it = verts.begin(); it != verts.end(); ++it)
    {
        ofxVertex *v = (*it);
        delete v;
    }
    verts.clear();
}
void ofxFace::deleteEdges()
{
    for(vector<ofxEdge *>::iterator it = edges.begin(); it != edges.end(); ++it)
    {
        ofxEdge *e = (*it);
        delete e;
    }
    edges.clear();
}

ofVec3f &ofxFace::getNormal()
{
    return normal;
}

ofVec3f &ofxFace::calculateNormal()
{
    sortPoints();
    ofxVertex *v0 = getVertex(0);
    ofxVertex *v1 = getVertex(1);
    ofxVertex *v2 = getVertex(2);
    ofVec3f v20 = v2->getPos() - v0->getPos();
    ofVec3f v10 = v1->getPos() - v0->getPos();
    normal = v20.getCrossed(v10).normalized();
    return normal;
}

void ofxFace::sortPoints()
{
    
}
