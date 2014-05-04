#include "ofxMesh.h"
#include "ofxFace.h"
#include "ofxVertex.h"
#include "ofxEdge.h"
#include "ofGraphics.h"

ofxMesh::ofxMesh()
{
    clear();
    init();
}

ofxMesh::~ofxMesh()
{
    clear();
}

void ofxMesh::init()
{
    scaleFactor = 1.0; 
}

void ofxMesh::setScaleFactor(float _scaleFactor)
{
    scaleFactor = _scaleFactor;
}

void ofxMesh::calculateBoundingBox()
{
    float xMin = 10000000;
    float xMax = -10000000;
    
    float yMin = 10000000;
    float yMax = -10000000;
    
    float zMin = 10000000;
    float zMax = -10000000;
    
    for(vector<ofxVertex *>::iterator it = verts.begin(); it != verts.end(); ++it)
    {
        ofxVertex *v = (*it);
        if(v->getNumFaces() > 0)
        {
            ofVec3f vertPos = v->getPos() - centroid;
            if(vertPos.x > xMax)
            {
                xMax = vertPos.x;
            }
            if(vertPos.x < xMin)
            {
                xMin = vertPos.x;
            }
            
            if(vertPos.y > yMax)
            {
                yMax = vertPos.y;
            }
            if(vertPos.y < yMin)
            {
                yMin = vertPos.y;
            }
            
            if(vertPos.z > zMax)
            {
                zMax = vertPos.z;
            }
            if(vertPos.z < zMin)
            {
                zMin = vertPos.z;
            }            
        }
    }
    
    meshWidth = xMax - xMin;
    meshHeight = yMax - yMin;
    meshDepth = zMax - zMin;
}
void ofxMesh::loadFromMesh(ofMesh &mesh)
{
    vector<ofVec3f> & mVerts = mesh.getVertices();
	vector<ofIndexType> & mIndi = mesh.getIndices();

    
    for(int i = 0; i < mVerts.size(); i++)
    {
        ofVec3f vertPos = scaleFactor*mVerts[i];
        ofxVertex *vertex = new ofxVertex(vertPos, i);
        verts.push_back(vertex); 
    }
    
    for(int i = 0; i < mIndi.size(); i+=3)
    {            
        ofxFace *face = new ofxFace(faces.size());
        
        ofxVertex *vertex0 = verts[mIndi[i]];
        ofxVertex *vertex1 = verts[mIndi[i+1]];
        ofxVertex *vertex2 = verts[mIndi[i+2]];
                
        ofxEdge *edge0 = getEdge(mIndi[i], mIndi[i+1]);
        ofxEdge *edge1 = getEdge(mIndi[i+1], mIndi[i+2]);
        ofxEdge *edge2 = getEdge(mIndi[i+2], mIndi[i]); 
        
        face->addVertex(vertex0);
        face->addVertex(vertex1); 
        face->addVertex(vertex2);

        face->addEdge(edge0);
        face->addEdge(edge1);
        face->addEdge(edge2);

        faces.push_back(face);
    }
    calculateCentroid();
    calculateBoundingBox();    
    computeVertexNormals(this);
}

void ofxMesh::loadFromObj(string path)
{
    ofBuffer buffer = ofBufferFromFile(path);
    int lineIndex = 0;

    while (!buffer.isLastLine())
    {
        string line = buffer.getNextLine();

        string cKey ("# ");
        string vKey ("v ");
        string fKey ("f ");

        size_t foundComment = line.find(cKey);
        size_t foundVertex = line.find(vKey);
        size_t foundFace = line.find(fKey);
        if (foundComment!=string::npos)
        {
            //Do Nothing
        }
        else if (foundVertex!=string::npos)
        {
            vector <string> vertexString = ofSplitString(line, " ");
            float x = ofToFloat(vertexString[1]);
            float y = ofToFloat(vertexString[2]);
            float z = ofToFloat(vertexString[3]);
            
            ofVec3f vertPos = scaleFactor*ofVec3f(x, y, z);
            ofxVertex *vertex = new ofxVertex(vertPos, verts.size());
            verts.push_back(vertex);
        }
        else if(foundFace!=string::npos)
        {
            vector <string> faceString = ofSplitString(line, " ");
            ofxFace *face = new ofxFace(faces.size());
            vector<int> indicies;
            
            for(int i = 1; i < faceString.size(); i++)
            {
                vector<string> index = ofSplitString(faceString[i], "/");
                indicies.push_back(ofToInt(index[0])-1);
            }
                    
            int length = indicies.size(); 
            for(int k = 0; k < length; k++)
            {
                int index1 = indicies[k];
                int index2 = indicies[(k+1)%length];
                
                ofxVertex *vertex0 = verts[index1];
                ofxVertex *vertex1 = verts[index2];
                
                ofxEdge *edge0 = getEdge(index1, index2);
                
                face->addVertex(vertex0);                
                face->addEdge(edge0);
            }
            
            faces.push_back(face);
        }
        lineIndex++;
    }
    calculateCentroid();
    calculateBoundingBox();
    computeVertexNormals(this);
}

void ofxMesh::computeVertexNormals(ofxMesh *mesh)
{
    for(vector<ofxVertex *>::iterator it = mesh->verts.begin(); it != mesh->verts.end(); ++it)
    {
        ofxVertex *v = (*it);
        v->calculateNormal();
    }
}

ofxEdge * ofxMesh::getEdge(ofIndexType i0, ofIndexType i1)
{
    string i0i1 = ofToString(i0) + "," + ofToString(i1);
    string i1i0 = ofToString(i1) + "," + ofToString(i0);

    map<string,ofxEdge *>::iterator it = edgeMap.find(i0i1);
    if(it != edgeMap.end())
    {
        return it->second;
    }
    
    it = edgeMap.find(i1i0);
    if(it != edgeMap.end())
    {
        return it->second;
    }

    ofxEdge *edge = new ofxEdge(verts[i0], verts[i1], edges.size());
    edges.push_back(edge); 
    edgeMap[i0i1] = edge;
    
    return edge;
}

void ofxMesh::clear()
{
    for(vector<ofxVertex *>::iterator it = verts.begin(); it != verts.end(); ++it)
    {
        ofxVertex *v = (*it);
        v->clear();
        delete v;
    }
    
    for(vector<ofxFace *>::iterator it = faces.begin(); it != faces.end(); ++it)
    {
        ofxFace *f = (*it);
        f->clear();
        delete f;
    }

    for(vector<ofxEdge *>::iterator it = edges.begin(); it != edges.end(); ++it)
    {
        ofxEdge *e = (*it);
        e->clear();
        delete e;
    }
    
    verts.clear();
    faces.clear();
    edges.clear();
}

ofVec3f &ofxMesh::calculateCentroid()
{
    centroid = ofVec3f(0,0,0); 
    for(vector<ofxVertex *>::iterator it = verts.begin(); it != verts.end(); ++it)
    {
        ofxVertex *v = (*it);
        centroid +=v->getPos();
    }
    centroid/=(float)verts.size();
}

ofVec3f &ofxMesh::getCentroid()
{
    return centroid;
}

void ofxMesh::center()
{
    for(vector<ofxVertex *>::iterator it = verts.begin(); it != verts.end(); ++it)
    {
        ofxVertex *v = (*it);
        v->setPos(v->getPos()-getCentroid());
    }
    for(vector<ofxFace *>::iterator it = faces.begin(); it != faces.end(); ++it)
    {
        ofxFace *f = (*it);
        f->calculateCentroid();
    }
    centroid = ofVec3f(0,0,0); 
}

float ofxMesh::getWidth()
{
    return meshWidth;
}

float ofxMesh::getHeight()
{
    return meshHeight;
}

float ofxMesh::getDepth()
{
    return meshDepth;
}

void ofxMesh::scale(float _x, float _y, float _z)
{
    
}

void ofxMesh::translate(float _x, float _y, float _z)
{
    
}

void ofxMesh::draw()
{
    glBegin(GL_TRIANGLES);
    for(vector<ofxFace *>::iterator fit = faces.begin(); fit != faces.end(); ++fit)
    {
        ofxFace *f = (*fit);
        glNormal3fv(f->getNormal().getPtr());
        for(vector<ofxVertex *>::iterator vit = f->verts.begin(); vit != f->verts.end(); ++vit)
        {
            ofxVertex *v = (*vit);
            glNormal3fv(v->getNormal().getPtr());
            glVertex3fv(v->getPos().getPtr());
        }
    }    
    glEnd();
}

void ofxMesh::drawVertices()
{
    glBegin(GL_POINTS);
    for(vector<ofxVertex *>::iterator it = verts.begin(); it != verts.end(); ++it)
    {
        ofxVertex *v = (*it);
        glNormal3fv(v->getNormal().getPtr());
        glVertex3fv(v->getPos().getPtr());
    }
    glEnd();
}

void ofxMesh::drawVerticesNumbers()
{
    for(vector<ofxVertex *>::iterator it = verts.begin(); it != verts.end(); ++it)
    {
        ofxVertex *v = (*it);
        ofDrawBitmapString(ofToString(v->getID()), v->getPos());
    }
}

void ofxMesh::drawEdges()
{
    glBegin(GL_LINES);
    for(vector<ofxEdge *>::iterator it = edges.begin(); it != edges.end(); ++it)
    {
        ofxEdge *e = (*it);
        for(vector<ofxVertex *>::iterator vit = e->verts.begin(); vit != e->verts.end(); ++vit)
        {
            ofxVertex *v = (*vit);
            glNormal3fv(v->getNormal().getPtr());
            glVertex3fv(v->getPos().getPtr());
        }
    }
    glEnd();    
}

void ofxMesh::drawFaces()
{
    for(vector<ofxFace *>::iterator fit = faces.begin(); fit != faces.end(); ++fit)
    {
        ofxFace *f = (*fit);
        glBegin(GL_POLYGON);
        glNormal3fv(f->getNormal().getPtr());
        for(vector<ofxVertex *>::iterator vit = f->verts.begin(); vit != f->verts.end(); ++vit)
        {
            ofxVertex *v = (*vit);
            glNormal3fv(v->getNormal().getPtr());
            glVertex3fv(v->getPos().getPtr());
        }
        glEnd();
    }
}

void ofxMesh::drawFaceNormals(float scale)
{
    glBegin(GL_LINES);
    for(vector<ofxFace *>::iterator it = faces.begin(); it != faces.end(); ++it)
    {
        ofxFace *f = (*it);
        ofVec3f center = f->getCentroid();
        ofVec3f normal = f->getNormal();
        glVertex3fv(center.getPtr()); 
        glVertex3f(center.x+normal.x*scale, center.y+normal.y*scale, center.z+normal.z*scale);
    }
    glEnd();
}

void ofxMesh::drawVertexNormals(float scale)
{
    glBegin(GL_LINES);
    for(vector<ofxVertex *>::iterator vit = verts.begin(); vit != verts.end(); ++vit)
    {
        ofxVertex *v = (*vit);
        ofVec3f pos = v->getPos();
        ofVec3f nor = v->getNormal();
        glVertex3fv(v->getPos().getPtr());
        glVertex3f(pos.x + nor.x*scale, pos.y + nor.y*scale, pos.z + nor.z*scale);
    }
    glEnd();
}

void ofxMesh::drawFace(int index)
{
    ofxFace *f = this->getFace(index); 
    glBegin(GL_POLYGON);
    glNormal3fv(f->getNormal().getPtr());
    for(vector<ofxVertex *>::iterator vit = f->verts.begin(); vit != f->verts.end(); ++vit)
    {
        ofxVertex *v = (*vit);
        glNormal3fv(v->getNormal().getPtr());        
        glVertex3fv(v->getPos().getPtr());
    }
    glEnd();
}

void ofxMesh::drawBoundingBox()
{
    float halfWidth = meshWidth*0.5;
    float halfHeight = meshHeight*0.5;
    float halfDepth = meshDepth*0.5;
    float cx = centroid.x;
    float cy = centroid.y;
    float cz = centroid.z;
    
    glBegin(GL_POINTS);
    

    glVertex3f(cx, cy, cz); //center
    glVertex3f(cx+halfWidth, cy+halfHeight, cz+halfDepth);  //TBR
    glVertex3f(cx-halfWidth, cy+halfHeight, cz+halfDepth);  //TTR
    glVertex3f(cx-halfWidth, cy-halfHeight, cz+halfDepth);  //TTL
    glVertex3f(cx+halfWidth, cy-halfHeight, cz+halfDepth);  //TBL
    
    glVertex3f(cx+halfWidth, cy+halfHeight, cz-halfDepth);  //BBR
    glVertex3f(cx-halfWidth, cy+halfHeight, cz-halfDepth);  //BTR
    glVertex3f(cx-halfWidth, cy-halfHeight, cz-halfDepth);  //BTL
    glVertex3f(cx+halfWidth, cy-halfHeight, cz-halfDepth);  //BBL
    
    glEnd();
    
    
    glBegin(GL_LINES);
    
    glVertex3f(cx+halfWidth, cy+halfHeight, cz+halfDepth);  //TBR
    glVertex3f(cx-halfWidth, cy+halfHeight, cz+halfDepth);  //TTR

    glVertex3f(cx-halfWidth, cy+halfHeight, cz+halfDepth);  //TTR
    glVertex3f(cx-halfWidth, cy-halfHeight, cz+halfDepth);  //TTL
    
    glVertex3f(cx-halfWidth, cy-halfHeight, cz+halfDepth);  //TTL
    glVertex3f(cx+halfWidth, cy-halfHeight, cz+halfDepth);  //TBL

    glVertex3f(cx+halfWidth, cy-halfHeight, cz+halfDepth);  //TBL
    glVertex3f(cx+halfWidth, cy+halfHeight, cz+halfDepth);  //TBR
    
    
    glVertex3f(cx+halfWidth, cy+halfHeight, cz-halfDepth);  //BBR
    glVertex3f(cx-halfWidth, cy+halfHeight, cz-halfDepth);  //BTR

    glVertex3f(cx-halfWidth, cy+halfHeight, cz-halfDepth);  //BTR
    glVertex3f(cx-halfWidth, cy-halfHeight, cz-halfDepth);  //BTL

    glVertex3f(cx-halfWidth, cy-halfHeight, cz-halfDepth);  //BTL
    glVertex3f(cx+halfWidth, cy-halfHeight, cz-halfDepth);  //BBL
    
    glVertex3f(cx+halfWidth, cy-halfHeight, cz-halfDepth);  //BBL
    glVertex3f(cx+halfWidth, cy+halfHeight, cz-halfDepth);  //BBR
    
    
    glVertex3f(cx+halfWidth, cy+halfHeight, cz+halfDepth);  //TBR
    glVertex3f(cx+halfWidth, cy+halfHeight, cz-halfDepth);  //BBR
    
    glVertex3f(cx-halfWidth, cy+halfHeight, cz+halfDepth);  //TTR
    glVertex3f(cx-halfWidth, cy+halfHeight, cz-halfDepth);  //BTR
    
    glVertex3f(cx-halfWidth, cy-halfHeight, cz+halfDepth);  //TTL
    glVertex3f(cx-halfWidth, cy-halfHeight, cz-halfDepth);  //BTL

    glVertex3f(cx+halfWidth, cy-halfHeight, cz+halfDepth);  //TBL
    glVertex3f(cx+halfWidth, cy-halfHeight, cz-halfDepth);  //BBL
    
    glEnd();
}

int ofxMesh::getNumVertices()
{
    return verts.size(); 
}

int ofxMesh::getNumEdges()
{
    return edges.size(); 
}

int ofxMesh::getNumFaces()
{
    return faces.size();
}

ofxVertex *ofxMesh::getVertex(int index)
{
    return verts[index%verts.size()];
}

ofRectangle ofxMesh::getBoundingRect()
{
    float minX = 100000;
    float maxX = -100000;
    
    float minY = 100000;
    float maxY = -100000;
    
    for(vector<ofxVertex *>::iterator it = verts.begin(); it != verts.end(); ++it)
    {
        ofxVertex *v = (*it);
        float x = v->getPos().x;
        float y = v->getPos().y;
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

ofPoint ofxMesh::getMinPoint()
{
    float x1 = 10000000;       //x1,y1---------//
    float y1 = 10000000;       //---------x2,y2//
    
    for(vector<ofxVertex *>::iterator it = verts.begin(); it != verts.end(); ++it)
    {
        ofVec3f pos = (*it)->getPos();
        if(pos.x < x1)
        {
            x1 = pos.x;
        }        
        if(pos.y < y1)
        {
            y1 = pos.y;
        }
    }
    return ofPoint(x1, y1); 
}

ofPoint ofxMesh::getMaxPoint()
{
    float x2 = -10000000;       //x1,y1---------//
    float y2 = -10000000;       //---------x2,y2//
    
    for(vector<ofxVertex *>::iterator it = verts.begin(); it != verts.end(); ++it)
    {
        ofVec3f pos = (*it)->getPos();
        if(pos.x > x2)
        {
            x2 = pos.x;
        }
        if(pos.y > y2)
        {
            y2 = pos.y;
        }
    }
    return ofPoint(x2, y2);
}

ofMesh ofxMesh::generateMesh()
{
    ofMesh mesh;
    
    int index = 0;
    for(vector<ofxVertex *>::iterator it = verts.begin(); it != verts.end(); ++it)
    {
        ofxVertex *v = (*it);
        mesh.addVertex(v->getPos());
        mesh.addNormal(v->getNormal());
    }
    
    for(vector<ofxFace *>::iterator it = faces.begin(); it != faces.end(); ++it)
    {
        ofxFace *f = (*it);
        int i0 = f->getVertex(0)->getID();
        int i1 = f->getVertex(1)->getID();
        int i2 = f->getVertex(2)->getID();
        mesh.addTriangle(i0, i1, i2);
    }
    return mesh; 
}

ofxEdge *ofxMesh::splitEdge(ofxEdge *e, ofxVertex *ep)
{
    ofxVertex *ev0 = e->getVertex(0);
    ofxVertex *ev1 = e->getVertex(1);
    
    ofxFace *f0 = e->getFace(0);
    ofxFace *f1 = e->getFace(1);
    
    ofxVertex *opp0 = NULL;
    ofxVertex *opp1 = NULL;
    
    for(int i = 0; i < f0->getNumVerts(); i++)
    {
        ofxVertex *temp = f0->getVertex(i);
        if(temp != ev0 && temp != ev1)
        {
            opp0 = temp;
            break;
        }
    }
    
    for(int i = 0; i < f1->getNumVerts(); i++)
    {
        ofxVertex *temp = f1->getVertex(i);
        if(temp != ev0 && temp != ev1)
        {
            opp1 = temp;
            break;
        }
    }
    
    ofVec3f epv;
    
    if(opp0 != NULL && opp1 != NULL)
    {
        epv = (ev0->getPos() + ev1->getPos())*(3.0/8.0) + (1.0/8.0)*(opp0->getPos() + opp1->getPos());
    }
    else if(opp0 != NULL)
    {
        epv = (ev0->getPos() + ev1->getPos())*(3.0/8.0) + (1.0/16.0)*(opp0->getPos());
    }
    else if(opp1 != NULL)
    {
        epv = (ev0->getPos() + ev1->getPos())*(3.0/8.0) + (1.0/16.0)*(opp1->getPos());
    }
    
    ep->setPos(epv);
    return e->splitEdge(ep);
}

ofxMesh *ofxMesh::clone()
{
    vector<ofxVertex *> newVerts;
    map<int, ofxVertex *> newVertsIDMap;
    
    for(vector<ofxVertex *>::iterator it = verts.begin(); it != verts.end(); ++it)
    {
        ofxVertex *ref = (*it);
        ofxVertex *nv = new ofxVertex(ref);
        newVerts.push_back(nv);
        newVertsIDMap[nv->getID()] = nv;
    }
    
    vector<ofxEdge *> newEdges;
    map<int, ofxEdge *> newEdgesIDMap;
    for(vector<ofxEdge *>::iterator it = edges.begin(); it != edges.end(); ++it)
    {
        ofxEdge *ref = (*it);
        int id0 = ref->getVertex(0)->getID();
        int id1 = ref->getVertex(1)->getID();
        ofxEdge *ne = new ofxEdge(newVertsIDMap[id0], newVertsIDMap[id1], ref->id);
        
        newEdges.push_back(ne);
        newEdgesIDMap[ne->getID()] = ne;
    }
    
    vector<ofxFace *> newFaces;
    map<int, ofxFace *> newFacesIDMap;
    for(vector<ofxFace *>::iterator it = faces.begin(); it != faces.end(); ++it)
    {
        ofxFace *ref = (*it);
        ofxFace *face = new ofxFace(ref->getID());
        face->addVertex(newVertsIDMap[ref->getVertex(0)->getID()]);
        face->addVertex(newVertsIDMap[ref->getVertex(1)->getID()]);
        face->addVertex(newVertsIDMap[ref->getVertex(2)->getID()]);
        
        face->addEdge(newEdgesIDMap[ref->getEdge(0)->getID()]);
        face->addEdge(newEdgesIDMap[ref->getEdge(1)->getID()]);
        face->addEdge(newEdgesIDMap[ref->getEdge(2)->getID()]);
        
        newFaces.push_back(face);
        newFacesIDMap[face->getID()] = face;
    }
    
    ofxMesh *newMesh = new ofxMesh();
    newMesh->verts = newVerts;
    newMesh->edges = newEdges;
    newMesh->faces = newFaces;
    
    computeVertexNormals(newMesh);
    return newMesh;
}

bool ofxMesh::areEdgesOrdered(ofxFace *face)
{
    ofxEdge *e0 = face->getEdge(0);
    ofxEdge *e1 = face->getEdge(1);
    ofxEdge *e2 = face->getEdge(2);
    
    if(e0->getVertex(1) == e1->getVertex(0) && e1->getVertex(1) == e2->getVertex(0) && e2->getVertex(1) == e0->getVertex(0))
    {
        return true;
    }
    return false;
}

bool ofxMesh::areEdgesOrdered(vector<ofxEdge*> _edges)
{
    ofxEdge *e0 = _edges[0];
    ofxEdge *e1 = _edges[1];
    ofxEdge *e2 = _edges[2];
    
    if(e0->getVertex(1) == e1->getVertex(0) && e1->getVertex(1) == e2->getVertex(0) && e2->getVertex(1) == e0->getVertex(0))
    {
        return true;
    }
    return false;
}

void ofxMesh::orderEdges(ofxFace *face)
{
    if(areEdgesOrdered(face))
    {
        return;
    }
    
    ofxEdge *e0 = face->getEdge(0);
    ofxEdge *e1 = face->getEdge(1);
    ofxEdge *e2 = face->getEdge(2);
    
    if(e0->getVertex(1) == e1->getVertex(0) || e0->getVertex(1) == e1->getVertex(1))
    {
        if(e0->getVertex(1) == e1->getVertex(1))
        {
            ofxVertex *v0 = e1->getVertex(0);
            ofxVertex *v1 = e1->getVertex(1);
            e1->clearVerts();
            e1->addVertex(v1);
            e1->addVertex(v0);
        }
        face->clearEdges();
        face->addEdge(e1);
        face->addEdge(e2);
        face->addEdge(e0);
        orderEdges(face);
    }
    
    if(e0->getVertex(1) == e2->getVertex(0) || e0->getVertex(1) == e2->getVertex(1))        //means e0->e2
    {
        if(e0->getVertex(1) == e2->getVertex(1))
        {
            ofxVertex *v0 = e2->getVertex(0);
            ofxVertex *v1 = e2->getVertex(1);
            e2->clearVerts();
            e2->addVertex(v1);
            e2->addVertex(v0);
        }
        face->clearEdges();
        face->addEdge(e0);
        face->addEdge(e2);
        face->addEdge(e1);
        orderEdges(face);
    }
}

vector<ofxEdge *> ofxMesh::orderEdges(vector<ofxEdge *> _edges)
{
    if(areEdgesOrdered(_edges))
    {
        return;
    }
    
    vector<ofxEdge *> result;
    
    ofxEdge *e0 = _edges[0];
    ofxEdge *e1 = _edges[1];
    ofxEdge *e2 = _edges[2];
    
    if(e0->getVertex(1) == e1->getVertex(0) || e0->getVertex(1) == e1->getVertex(1))
    {
        if(e0->getVertex(1) == e1->getVertex(1))
        {
            ofxVertex *v0 = e1->getVertex(0);
            ofxVertex *v1 = e1->getVertex(1);
            e1->clearVerts();
            e1->addVertex(v1);
            e1->addVertex(v0);
        }
        result.push_back(e1);
        result.push_back(e2);
        result.push_back(e0);
        orderEdges(result);
    }
    
    if(e0->getVertex(1) == e2->getVertex(0) || e0->getVertex(1) == e2->getVertex(1))        //means e0->e2
    {
        if(e0->getVertex(1) == e2->getVertex(1))
        {
            ofxVertex *v0 = e2->getVertex(0);
            ofxVertex *v1 = e2->getVertex(1);
            e2->clearVerts();
            e2->addVertex(v1);
            e2->addVertex(v0);
        }

        result.push_back(e0);
        result.push_back(e2);
        result.push_back(e1);
        orderEdges(result);
    }
}

ofxVertex *ofxMesh::getOppositeVertex(ofxEdge *e, ofxFace *f)
{
    ofxFace *otherface = (e->getFace(0) != f) ? e->getFace(0) : e->getFace(1);

    ofxVertex *v0 = otherface->getVertex(0);
    ofxVertex *v1 = otherface->getVertex(1);
    ofxVertex *v2 = otherface->getVertex(2);
    
    ofxVertex *ev0 = e->getVertex(0);
    ofxVertex *ev1 = e->getVertex(1);
    
    if(v0 != ev0 && v0 != ev1)
    {
        return v0;
    }
    else if(v1 != ev0 && v1 != ev1)
    {
        return v1;
    }
    else
    {
        return v2;
    }
}

ofxMesh *ofxMesh::smooth(int level)
{
    ofxMesh *other = NULL;
    for(int i = 0; i < level; i++)
    {
        if(other == NULL)
        {
            other = clone();
        }
        else
        {
            ofxMesh *old = other;
            other = other->clone();
            delete old; 
        }
        vector<ofxVertex *> cloneVerts = other->verts;
        vector<ofxFace *> cloneFaces = other->faces;
        vector<ofxEdge *> cloneEdges = other->edges;
        
        vector<ofxFace *> newFaces;
        vector<ofxEdge *> newEdges;
        
        multimap<ofxEdge *, ofxEdge *> subdividedEdges;
        map<ofxVertex *, ofVec3f *> newVertexPositions;
        
        for(vector<ofxFace *>::iterator it = cloneFaces.begin(); it != cloneFaces.end(); ++it)
        {
            ofxFace *fo = (*it);
            bool reverseOrder = !areEdgesOrdered(fo);
            orderEdges(fo);
            //Old Edges
            ofxEdge *e0o = fo->getEdge(0);
            ofxEdge *e1o = fo->getEdge(1);
            ofxEdge *e2o = fo->getEdge(2);
            
            //Control Vertex Points (vp)
            ofxVertex *vp0 = e0o->getVertex(0);
            ofxVertex *vp1 = e1o->getVertex(0);
            ofxVertex *vp2 = e2o->getVertex(0);
            
//            vector<ofxVertex *> vp0neighbors = vp0->getNeighbors();
//            vector<ofxVertex *> vp1neighbors = vp0->getNeighbors();
//            vector<ofxVertex *> vp2neighbors = vp0->getNeighbors();
//            
//            int nvp0 = vp0neighbors.size();
//            int nvp1 = vp1neighbors.size();
//            int nvp2 = vp2neighbors.size();
////            cout << "n0: " << vp0->getNumEdges() << endl;
////            cout << "n1: " << vp1->getNumEdges() << endl;
////            cout << "n2: " << vp2->getNumEdges() << endl;
////            
//            ofVec3f vp0nsum = ofVec3f(0,0,0);
//            ofVec3f vp1nsum = ofVec3f(0,0,0);
//            ofVec3f vp2nsum = ofVec3f(0,0,0);
//            
//            for(vector<ofxVertex *>::iterator it = vp0neighbors.begin(); it != vp0neighbors.end(); ++it)
//            {
//                vp0nsum += (*it)->getPos();
//            }
//            
//            for(vector<ofxVertex *>::iterator it = vp1neighbors.begin(); it != vp1neighbors.end(); ++it)
//            {
//                vp1nsum += (*it)->getPos();
//            }
//            
//            for(vector<ofxVertex *>::iterator it = vp2neighbors.begin(); it != vp2neighbors.end(); ++it)
//            {
//                vp2nsum += (*it)->getPos();
//            }
//            
//            
//            float svp0 = (nvp0 == 3) ? (3.0/16.0) : ( (1.0/nvp0) * ( (5.0/8.0) - pow( (3.0/8.0) + (1.0/4.0)*cos( TWO_PI / (float)nvp0 ) , 2.0 ) ) );
//            float svp1 = (nvp1 == 3) ? (3.0/16.0) : ( (1.0/nvp1) * ( (5.0/8.0) - pow( (3.0/8.0) + (1.0/4.0)*cos( TWO_PI / (float)nvp1 ) , 2.0 ) ) );
//            float svp2 = (nvp2 == 3) ? (3.0/16.0) : ( (1.0/nvp2) * ( (5.0/8.0) - pow( (3.0/8.0) + (1.0/4.0)*cos( TWO_PI / (float)nvp2 ) , 2.0 ) ) );
//            
//            ofVec3f *vp0np = new ofVec3f((1.0 - nvp0)*svp0*vp0->getPos() + svp0*vp0nsum);
//            ofVec3f *vp1np = new ofVec3f((1.0 - nvp1)*svp1*vp1->getPos() + svp1*vp1nsum);
//            ofVec3f *vp2np = new ofVec3f((1.0 - nvp2)*svp2*vp2->getPos() + svp2*vp2nsum);
//            
//            newVertexPositions[vp0] = vp0np;
//            newVertexPositions[vp1] = vp1np;
//            newVertexPositions[vp2] = vp2np;
//            /* A given vertex has n neighbor vertices. 
//             The new vertex point is one minus n times s 
//             times the old vertex, plus s times the sum 
//             of the neighboring vertices, where s is a 
//             scaling factor. For n equal to three, s 
//             is three sixteenths. For n greater than 
//             three, s = 1/n (5/8 - (3/8 + 1/4 cos(2π / n))2)
//            */
//            
            //Edge Vertex Points (ep)
            ofxVertex *ep0 = NULL;
            ofxVertex *ep1 = NULL;
            ofxVertex *ep2 = NULL;
            /* These points are three eighths of the sum
             of the two end points of the edge plus one
             eighth of the sum of the two other points
             that form the two triangles that share the
             edge in question.
             */
            
            //New Edges
            ofxEdge *e0n0 = NULL;
            ofxEdge *e0n1 = NULL;
            
            ofxEdge *e1n0 = NULL;
            ofxEdge *e1n1 = NULL;
            
            ofxEdge *e2n0 = NULL;
            ofxEdge *e2n1 = NULL;
            
            if(subdividedEdges.count(e0o) != 0)
            {
                vector<ofxEdge *> sedges;
                for(multimap<ofxEdge *, ofxEdge *>::iterator it =subdividedEdges.equal_range(e0o).first; it!=subdividedEdges.equal_range(e0o).second; ++it)
                {
                    sedges.push_back(it->second);
                }
                e0n0 = sedges[0];
                e0n1 = sedges[1];
                ep0 = e0n1->getVertex(0);
            }
            else
            {
//                ofxVertex *opp = getOppositeVertex(e0o, fo);
                ofxVertex *opp = NULL;
                if(opp != NULL)
                {
                    ofVec3f ep0np = (vp0->getPos()+vp1->getPos())*(3.0/8.0) + (vp2->getPos() + opp->getPos())*(1.0/8.0);
                    ep0 = new ofxVertex(ep0np, cloneVerts.size());
                }
                else
                {
                    ep0 = new ofxVertex(e0o->getCentroid(), cloneVerts.size());
                }
                
                cloneVerts.push_back(ep0);
                
                e0n0 = new ofxEdge(e0o->getVertex(0), ep0, newEdges.size());
                newEdges.push_back(e0n0);
                
                e0n1 = new ofxEdge(ep0, e0o->getVertex(1), newEdges.size());
                newEdges.push_back(e0n1);
                
                subdividedEdges.insert(pair<ofxEdge *, ofxEdge *>(e0o, e0n0));
                subdividedEdges.insert(pair<ofxEdge *, ofxEdge *>(e0o, e0n1));
            }
            
            
            if(subdividedEdges.count(e1o) != 0)
            {
                vector<ofxEdge *> sedges;
                for(multimap<ofxEdge *, ofxEdge *>::iterator it =subdividedEdges.equal_range(e1o).first; it!=subdividedEdges.equal_range(e1o).second; ++it)
                {
                    sedges.push_back(it->second);
                }
                e1n0 = sedges[0];
                e1n1 = sedges[1];
                ep1 = e1n1->getVertex(0);
            }
            else
            {
//                ofxVertex *opp = getOppositeVertex(e1o, fo);
                ofxVertex *opp = NULL;
                if(opp != NULL)
                {
                    ofVec3f ep1np = (vp1->getPos()+vp2->getPos())*(3.0 / 8.0) + (1.0/8.0)*(vp0->getPos() + opp->getPos());
                    ep1 = new ofxVertex(ep1np, cloneVerts.size());
                }
                else
                {
                    ep1 = new ofxVertex(e1o->getCentroid(), cloneVerts.size());
                }

                cloneVerts.push_back(ep1);
                
                e1n0 = new ofxEdge(e1o->getVertex(0), ep1, newEdges.size());
                newEdges.push_back(e1n0);
                
                e1n1 = new ofxEdge(ep1, e1o->getVertex(1), newEdges.size());
                newEdges.push_back(e1n1);
                
                subdividedEdges.insert(pair<ofxEdge *, ofxEdge *>(e1o, e1n0));
                subdividedEdges.insert(pair<ofxEdge *, ofxEdge *>(e1o, e1n1));
            }
            
            if(subdividedEdges.count(e2o) != 0)
            {
                vector<ofxEdge *> sedges;
                for(multimap<ofxEdge *, ofxEdge *>::iterator it =subdividedEdges.equal_range(e2o).first; it!=subdividedEdges.equal_range(e2o).second; ++it)
                {
                    sedges.push_back(it->second);
                }
                e2n0 = sedges[0];
                e2n1 = sedges[1];
                ep2 = e2n1->getVertex(0);
            }
            else
            {
//                ofxVertex *opp = getOppositeVertex(e2o, fo);
                ofxVertex *opp = NULL;
                if(opp != NULL)
                {
                    ofVec3f ep2np = (vp2->getPos()+vp1->getPos())*(3.0 / 8.0) + (1.0/8.0)*(vp1->getPos() + opp->getPos());
                    ep2 = new ofxVertex(ep2np, cloneVerts.size());
                }
                else
                {
                    ep2 = new ofxVertex(e2o->getCentroid(), cloneVerts.size());
                }
                
                cloneVerts.push_back(ep2);
                
                e2n0 = new ofxEdge(e2o->getVertex(0), ep2, newEdges.size());
                newEdges.push_back(e2n0);
                
                e2n1 = new ofxEdge(ep2, e2o->getVertex(1), newEdges.size());
                newEdges.push_back(e2n1);
                
                subdividedEdges.insert(pair<ofxEdge *, ofxEdge *>(e2o, e2n0));
                subdividedEdges.insert(pair<ofxEdge *, ofxEdge *>(e2o, e2n1));
            }
            
            ofxEdge *e3n0 = new ofxEdge(ep1, ep0, newEdges.size());
            newEdges.push_back(e3n0);
            
            ofxEdge *e3n1 = new ofxEdge(ep2, ep1, newEdges.size());
            newEdges.push_back(e3n1);
            
            ofxEdge *e3n2 = new ofxEdge(ep0, ep2, newEdges.size());
            newEdges.push_back(e3n2);
            
            ofxFace *fn0 = new ofxFace(newFaces.size());
            newFaces.push_back(fn0);
            
            if(reverseOrder)
            {
                fn0->addVertex(ep2);
                fn0->addVertex(ep0);
                fn0->addVertex(vp0);

                fn0->addEdge(e2n1);
                fn0->addEdge(e3n2);
                fn0->addEdge(e0n0);
            }
            else
            {
                fn0->addVertex(vp0);
                fn0->addVertex(ep0);
                fn0->addVertex(ep2);

                fn0->addEdge(e0n0);
                fn0->addEdge(e3n2);
                fn0->addEdge(e2n1);
            }
            
            
            ofxFace *fn1 = new ofxFace(newFaces.size());
            newFaces.push_back(fn1);
            
            if(reverseOrder)
            {
                fn1->addVertex(ep0);
                fn1->addVertex(ep1);
                fn1->addVertex(vp1);

                fn1->addEdge(e0n1);
                fn1->addEdge(e3n0);
                fn1->addEdge(e1n0);
            }
            else
            {
                fn1->addVertex(vp1);
                fn1->addVertex(ep1);
                fn1->addVertex(ep0);

                fn1->addEdge(e1n0);
                fn1->addEdge(e3n0);
                fn1->addEdge(e0n1);
            }
            
            
            ofxFace *fn2 = new ofxFace(newFaces.size());
            newFaces.push_back(fn2);
            
            if(reverseOrder)
            {
                fn2->addVertex(ep1);
                fn2->addVertex(ep2);
                fn2->addVertex(vp2);

                fn2->addEdge(e1n1);
                fn2->addEdge(e3n1);
                fn2->addEdge(e2n0);
            }
            else
            {
                fn2->addVertex(vp2);
                fn2->addVertex(ep2);
                fn2->addVertex(ep1);

                fn2->addEdge(e2n0);
                fn2->addEdge(e3n1);
                fn2->addEdge(e1n1);
            }
            
            
            ofxFace *fn3 = new ofxFace(newFaces.size());
            newFaces.push_back(fn3);
            
            if(reverseOrder)
            {
                fn3->addVertex(ep2);
                fn3->addVertex(ep1);
                fn3->addVertex(ep0);
                
                fn3->addEdge(e3n2);
                fn3->addEdge(e3n1);
                fn3->addEdge(e3n0);
            }
            else
            {
                fn3->addVertex(ep0);
                fn3->addVertex(ep1);
                fn3->addVertex(ep2);

                fn3->addEdge(e3n0);
                fn3->addEdge(e3n1);
                fn3->addEdge(e3n2);
            }
            
        }
        
//        for (vector<ofxVertex *>::iterator it = cloneVerts.begin(); it != cloneVerts.end(); ++it)
//        {
//            ofxVertex *vtx = (*it);
//            ofVec3f *vpos = newVertexPositions[vtx];
//            if(vpos != NULL)
//            {
//                vtx->setPos(*vpos);
//                delete vpos;
//                newVertexPositions[vtx] = NULL;
//            }
//        }
        
        other->faces = newFaces;
        other->edges = newEdges;
        other->verts = cloneVerts;
        
        for(vector<ofxFace *>::iterator it = cloneFaces.begin(); it != cloneFaces.end(); ++it)
        {
            ofxFace *f = (*it);
            delete f;
        }
        cloneFaces.clear();
        
        for(vector<ofxEdge *>::iterator it = cloneEdges.begin(); it != cloneEdges.end(); ++it)
        {
            ofxEdge *e = (*it);
            delete e;
            
        }
        cloneEdges.clear();
        subdividedEdges.clear();
    }

    computeVertexNormals(other);
    return other;
}

ofxFace *ofxMesh::getFace(int index)
{
    return faces[index%faces.size()];
}

ofxEdge *ofxMesh::getEdge(int index)
{
    return edges[index%edges.size()];
}