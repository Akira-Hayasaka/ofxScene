//
//  Geometry.h
//

#pragma once
#include "ofMain.h"
#include "ofxAssimpModelLoader.h"
#include "VBO.h"

namespace ofxScene{
    
    class Face3{
    public:
        Face3(int v1=0, int v2=0, int v3=0){ indices[0]=v1; indices[1]=v2; indices[2]=v3; }
        ~Face3(){};
        
        void flipFace(){ int temp=indices[1]; indices[1]=indices[2]; indices[2]=temp; }
        int& operator[]( int n ){   return indices[n]; }
        
        int indices[3];
        ofVec3f centroid, normal;
        
    };
    
    class Geometry : public VBO {
    public:
        Geometry(){
            useCount = 0;
        };
        ~Geometry(){
            
            //delete the vertex & index buffers
            map< string, ofxScene::VertexAttributeBase* >::iterator vbo_it;
            for ( vbo_it = attributes.begin() ; vbo_it != attributes.end(); vbo_it++ ){
                glDeleteBuffers( 1, &vbo_it->second->bufferId );
            }
            
        };
        
        void update(int frame = ofGetFrameNum()){
            if(lastFrame != frame ){
                lastFrame = frame;
                
                map< string, ofxScene::VertexAttributeBase* >::iterator vbo_it;
                for ( vbo_it = attributes.begin() ; vbo_it != attributes.end(); vbo_it++ ){
                    vbo_it->second->update();
                }
            }
        }
        
        void updateAttribute( string name ){
            if(attributes.count(name)){
                attributes[name]->update();
            }
        }
        
        //TODO::   void bind(Shader* shader); it'd be nice to bind the geometry and shader once, "instancing" an array of nodes
        
        
        //faces
        void addFace( int v0, int v1, int v2, int v3 = -1 ){
            faces.push_back( Face3( v0, v1, v2 ) );
            if( v3 > -1 ){
                faces.push_back( Face3( v0, v2, v3 ) );
            }
        }
        //        void addFaceNSided( vector<int>& fIndices );
        
        void updateFaceIndices(){
            indices.resize( faces.size() * 3 );
            
            for(int i=0; i<faces.size(); i++){
                indices[i*3] = faces[i][0];
                indices[i*3+1] = faces[i][1];
                indices[i*3+2] = faces[i][2];
            }
        }
        
        void calcFaceNormal( int index ){
            faces[index].normal = (vertices[faces[index][2]] - vertices[faces[index][1]]).cross( vertices[faces[index][0]] - vertices[faces[index][1]] ).normalize();
        }
        
        void calcFaceNormals(){
            for(int i=0; i<faces.size(); i++){
                calcFaceNormal( i );
            }
        }
        
        void calcVertexNotmals(){
            calcFaceNormals();
            normals.resize( vertices.size(), ofVec3f(0) );
            for(int i=0; i<normals.size(); i++){
                normals[i].set(0,0,0);
            }
            for(int i=0; i<faces.size();i++){
                normals[ faces[i][0] ] += faces[i].normal;
                normals[ faces[i][1] ] += faces[i].normal;
                normals[ faces[i][2] ] += faces[i].normal;
            }
            for(int i=0; i<normals.size(); i++){
                normals[i].normalize();
            }
        }
        
        void calcFaceCentroids(){
            for(int i=0;i<faces.size(); i++){
                faces[i].centroid = (vertices[faces[i][0]] + vertices[faces[i][1]] + vertices[faces[i][2]])/3.f;
            }
        }
        
        void calcTangents(){
            // Lengyel, Eric. “Computing Tangent Space Basis Vectors for an Arbitrary Mesh”.
            // Terathon Software 3D Graphics Library, 2001. http://www.terathon.com/code/tangent.html
            
            if(texCoords.size() != vertices.size() ){
                cout << "we need texCoords to compute tangents" << endl;
                return;
            }
            
            vector<ofVec3f> tan1( vertices.size(), ofVec3f(0) );
            vector<ofVec3f> tan2( vertices.size(), ofVec3f(0) );
            ofVec3f sdir, tdir;
            int i1, i2, i3;
            float x1, x2, y1, y2, z1, z2, s1, s2, t1, t2, r;
            
            for(int f=0; f<faces.size(); f++){
                i1 = faces[f][0];
                i2 = faces[f][1];
                i3 = faces[f][2];

                ofVec3f& v1 = vertices[i1];
                ofVec3f& v2 = vertices[i2];
                ofVec3f& v3 = vertices[i3];

                ofVec2f& w1 = texCoords[i1];
                ofVec2f& w2 = texCoords[i2];
                ofVec2f& w3 = texCoords[i3];
                

                x1 = v2.x - v1.x;
                x2 = v3.x - v1.x;
                y1 = v2.y - v1.y;
                y2 = v3.y - v1.y;
                z1 = v2.z - v1.z;
                z2 = v3.z - v1.z;

                s1 = w2.x - w1.x;
                s2 = w3.x - w1.x;
                t1 = w2.y - w1.y;
                t2 = w3.y - w1.y;
                
                r = 1.0F / (s1 * t2 - s2 * t1);
                sdir.set((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
                         (t2 * z1 - t1 * z2) * r);
                tdir.set((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,
                         (s1 * z2 - s2 * z1) * r);
                
                tan1[i1] += sdir;
                tan1[i2] += sdir;
                tan1[i3] += sdir;

                tan2[i1] += tdir;
                tan2[i2] += tdir;
                tan2[i3] += tdir;
            }

            
            tangents.resize( vertices.size() );
            binormals.resize( vertices.size() );
            for (int a=0; a < vertices.size(); a++){
                ofVec3f& n = normals[a];
                ofVec3f& t = tan1[a];
                
                // Gram-Schmidt orthogonalize
                tangents[a] = (t - n * n.dot(t)).normalize();
                
                // Calculate handedness
                tangents[a].w = ( n.crossed(t).dot( tan2[a] ) < 0.) ? -1.f : 1.f;
                binormals[a] = n.crossed( tangents[a] );
            }
            
            if(getAttr( "tangent") == NULL ){
                addAttribute( "tangent", tangents );
                
                cout << "adding tangent attribute" << endl;
            }
        }
        
        void load( string fileloc ){
            ofxAssimpModelLoader loader;
            loader.loadModel( fileloc );
            
            ofMesh loadedMesh = loader.getMesh( 0);
            cout << "loader.getNumMeshes(): " << loader.getNumMeshes() << endl;
            
            vertices.clear();
            normals.clear();
            texCoords.clear();
            faces.clear();
            indices.clear();
            
            const aiScene* scn = loader.getAssimpScene();
            
            cout << "scn->mNumMeshes: "<<scn->mNumMeshes << endl;
            
            
            aiVector3D* v;
            aiFace* f;
            int indexCount = 0, vertexCount = 0;
            for(int i=0; i<scn->mNumMeshes;i++){
                //add vertices
                vertexCount = vertices.size();
                vertices.resize( vertexCount  + scn->mMeshes[i]->mNumVertices );
                for(int j=0; j<scn->mMeshes[i]->mNumVertices; j++){
                    v = &scn->mMeshes[i]->mVertices[j];
                    vertices[j + vertexCount].set(v->x, v->y, v->z );
                }
                
                //add faces
                indexCount = faces.size()*3;
                for(int j=0;j<scn->mMeshes[i]->mNumFaces; j++){
                    f = &scn->mMeshes[i]->mFaces[j];
                    addFace( f->mIndices[0]+indexCount, f->mIndices[1]+indexCount, f->mIndices[2]+indexCount );
                }
            }
            
            //calculate normals
            calcVertexNotmals();
            
            //add indices
            updateFaceIndices();
            
            addAttribute( "position", vertices );
            addAttribute( "normal", normals );
            //TODO::            addAttribute( "texCoord", texCoords ); 
            addIndices( indices );
        }
        
        void clearData(){
            vertices.clear();
            normals.clear();
            texCoords.clear();
            faces.clear();
            indices.clear();
            tangents.clear();
            binormals.clear();
        }
        
        //mesh construction
        vector<ofVec3f> vertices;
        vector<ofVec3f> normals;
        vector<ofVec2f> texCoords;
        vector<ofVec3f> binormals;
        vector<ofVec4f> tangents;
        vector<Face3> faces;
        
        //TODO:: setup faceted rendering...
        vector<ofVec3f> faceVertices;
        vector<ofVec3f> faceNormals;
        vector<ofVec2f> faceTexCoords;
        vector<int> indices;
        
        int lastFrame;
        
        int useCount;
    };
}
