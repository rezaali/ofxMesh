#include "ofxFace.h"
#include "ofxVertex.h"
#include "ofxEdge.h"

ofxVertex::ofxVertex(ofVec3f _pos, int _id = -1)
{
    pos = _pos;
    id = _id; 
}

ofxVertex::ofxVertex(ofxVertex *ref)
{
    pos = ofVec3f(ref->getPos()); 
    id = ref->getID();
}

ofxVertex::~ofxVertex()
{
    clear(); 
}

void ofxVertex::clear()
{
    edges.clear();
    faces.clear();
}

void ofxVertex::addEdge(ofxEdge *edge)
{
    edges.push_back(edge);
}

void ofxVertex::addFace(ofxFace *face)
{
    faces.push_back(face); 
}

ofxEdge *ofxVertex::getEdge(int index)
{
    return edges[index%edges.size()];
}

ofxFace *ofxVertex::getFace(int index)
{
    return faces[index%faces.size()];
}

int ofxVertex::getID()
{
    return id;
}

void ofxVertex::setID(int _id)
{
    id = _id; 
}

ofVec3f &ofxVertex::getPos()
{
    return pos;
}

ofVec3f &ofxVertex::getNormal()
{
    return normal;
}

ofVec3f &ofxVertex::calculateNormal()
{
    normal = ofVec3f(0,0,0);
    for(vector<ofxFace *>::iterator it = faces.begin(); it != faces.end(); ++it)
    {
        ofxFace *f = (*it);
        normal += f->getNormal();
    }
    
    normal/=(float)faces.size();
    return normal.normalize(); 
}

ofColor &ofxVertex::getColor()
{
    return color;
}

void ofxVertex::setColor(ofColor _color)
{
    color = _color;
}

int ofxVertex::getNumFaces()
{
    return faces.size(); 
}

int ofxVertex::getNumEdges()
{
    return edges.size(); 
}

vector<ofxVertex *> ofxVertex::getNeighbors()
{
    vector<ofxVertex *> neighbors;
    for(vector<ofxEdge *>::iterator it = edges.begin(); it != edges.end(); ++it)
    {
        ofxEdge *e = (*it);
        ofxVertex *v0 = e->getVertex(0);
        ofxVertex *v1 = e->getVertex(1);
        if(v0->getID() != this->getID())
        {
            neighbors.push_back(v0);
        }
        else
        {
            neighbors.push_back(v1);
        }
    }
    return neighbors;
}

ofVec3f ofxVertex::getNeighborsAveragePos()
{
    ofVec3f average = ofVec3f(0,0,0);
    vector<ofxVertex *> neighbors = getNeighbors();
    for(vector<ofxVertex *>::iterator it = neighbors.begin(); it != neighbors.end(); ++it)
    {
        ofxVertex *v = (*it);
        average += v->getPos();
    }
    average /= (float)neighbors.size();
    return average;
}

ofxVertex * ofxVertex::getClosestNeighborFrom(ofVec3f _pos)
{
    float distance = 100000;
    ofxVertex *closest = NULL;
    vector<ofxVertex *> neighbors = this->getNeighbors();
    for(vector<ofxVertex *>::iterator it = neighbors.begin(); it != neighbors.end(); ++it)
    {
        float nDist = _pos.distance((*it)->getPos());
        if(nDist < distance)
        {
            closest = (*it);
            distance = nDist;
        }
    }
    return closest;
}

void ofxVertex::setPos(ofVec3f _pos)
{
    pos = _pos; 
}

void ofxVertex::setTexcoord(ofVec2f _texcoord)
{
    texcoord = _texcoord;
}

ofVec2f &ofxVertex::getTexcoord()
{
    return texcoord;
}

void ofxVertex::removeEdge(ofxEdge *edge)
{
    for(vector<ofxEdge *>::iterator it = edges.begin(); it != edges.end(); ++it)
    {
        ofxEdge *e = (*it);
        if(e == edge)
        {
            edges.erase(it);
            break; 
        }
    }
}