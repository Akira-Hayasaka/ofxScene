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
            bUpdateFaceIndices = false;
            bUpdateVertexData = true;
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
        
        
        //faces
        void addFace( int v0, int v1, int v2, int v3 = -1 ){
            faces.push_back( Face3( v0, v1, v2 ) );
            if( v3 > -1 ){
                faces.push_back( Face3( v0, v2, v3 ) );
            }
            bUpdateFaceIndices = true;
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
        
        void setDefualtBuffers(){
            
            if(getAttr("position") == NULL && vertices.size() ){
                addAttribute("position", vertices );
            }
            
            if(getAttr("normal") == NULL && normals.size() ){
                addAttribute("normal", normals );
            }
            
            if(getAttr("indices") == NULL && indices.size() ){
                addIndices(indices);
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
        }
        
        //mesh construction
        vector<ofVec3f> vertices;
        vector<ofVec3f> normals;
        vector<ofVec2f> texCoords;
        vector<Face3> faces;
        vector<int> indices;
        
        bool bUpdateFaceIndices;
        bool bUpdateVertexData;
        
        int lastFrame;
        
        int useCount;
    };
    
    
    class SphereGeometry : public Geometry{
    public:
        
        SphereGeometry( float radius = 50, int _subdX = 31, int _subdY = 10, bool weldSeam = false ){
            bUpdateFaceIndices = false;
            bUpdateVertexData = true;
            
            init( radius, _subdX, _subdY, weldSeam );
        };
        ~SphereGeometry(){}
            
        void init( float radius = 50, int _subdX = 31, int _subdY = 10, bool weldSeam = false ){
            clearData();
            
            int subdX = max( 3, _subdX );
            int subdY = max( 1, _subdY );
            
            //vertices
            float u,v, xval, yval;
            for(int x=0; x<=subdX; x++){
                xval = float(x)/float(subdX);
                for(int y=0; y<=subdY; y++){
                    yval = float(y)/float(subdY);
                    u = PI * xval * 2.;
                    v = PI * yval;
                    vertices.push_back( ofVec3f(radius * cos( u ) * sin( v ),  
                                                radius * cos( v ),             
                                                radius * sin( u ) * sin( v )));
                    normals.push_back( vertices.back().normalized() );
                    texCoords.push_back( ofVec2f( 1. - xval, yval ) );
                }
                if( weldSeam && x == subdX - 1 ){
                    continue;
                }
            }
            
            //faces
            for( int x=0; x<subdX; x++){
                for(int y=0; y<subdY; y++){
                    //caps: this isn't perfect. the tex coords get screwed up. will fix it better when I have time...
                    if( y == 0 ){
                        addFace( (x+1)*(subdY+1)+y, (x+1)*(subdY+1)+y+1, x*(subdY+1)+y+1 );
                    }
                    else if( y == subdY-1){
                        addFace( x*(subdY+1)+y, (x+1)*(subdY+1)+y, (x+1)*(subdY+1)+y+1 );
                    }
                    else{
                        addFace( x*(subdY+1)+y, (x+1)*(subdY+1)+y, (x+1)*(subdY+1)+y+1, x*(subdY+1)+y+1);
                    }
                }
            }
            
            updateFaceIndices();
            
            addAttribute( "position", vertices );
            addAttribute( "normal", normals );
            addAttribute( "texCoord", texCoords );
            addIndices( indices );
        }
    };
    
    class ConeGeometry : public Geometry{
    public:
        ConeGeometry( float radius=50, float height=100, int subdivisions=11){
            init( radius, height, subdivisions);
        };
        ~ConeGeometry(){};
        
        void init( float radius=50, float height=100, int subdivisions=11){
            clearData();
            
            
            int subdX = max( 3, subdivisions );
            
            //            sides
            //vertices
            float u,theta;
            for(int x=0; x<=subdX; x++){
                u = float(x)/float(subdX);
                theta = PI * 2. * u;
                vertices.push_back( ofVec3f(radius * sin( theta ),
                                            -height/2.,
                                            radius * cos(theta)));
            }
            vertices.push_back( ofVec3f(0,height/2.,0) );
            
            //faces
            int pointIndex = vertices.size()-1;
            for( int x=0; x<subdX; x++){
                addFace(x, x+1, pointIndex );
            }
            
            //            cap
            //vertices
            int vCount = vertices.size();
            for(int x=0; x<=subdX; x++){
                u = float(x)/float(subdX);
                theta = PI * 2. * u;
                vertices.push_back( ofVec3f(radius * sin( theta ),
                                            -height/2.,
                                            radius * cos(theta)));
            }
            vertices.push_back( ofVec3f(0,-height/2.,0) );
            
            //faces
            int capCenter = vertices.size()-1;
            for( int x=0; x<subdX; x++){
                addFace(x + vCount, x+1+vCount, capCenter );
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
    };
    
    class CylinderGeometry : public Geometry{
    public:
        CylinderGeometry( float radius=50, float height=100, int subdivisions=11){
            init( radius, height, subdivisions);
        };
        ~CylinderGeometry(){};
        
        void init( float radius=50, float height=100, int subdivisions=11){
            clearData();
            
            
            int subdX = max( 3, subdivisions );
            
            //            sides
            //vertices
            float u,theta;
            for(int x=0; x<=subdX; x++){
                u = float(x)/float(subdX);
                theta = PI * 2. * u;
                vertices.push_back( ofVec3f(radius * sin( theta ),
                                            -height/2.,
                                            radius * cos(theta)));
                
                vertices.push_back( ofVec3f(radius * sin( theta ),
                                            height/2.,
                                            radius * cos(theta)));
            }
            
            //faces
            int pointIndex = vertices.size()-1;
            for( int x=0; x<subdX*2; x+=2){
                addFace(x, x+1, x+3, x+2 );
            }
            
            //bottom cap
            //vertices
            int vCount = vertices.size();
            for(int x=0; x<=subdX; x++){
                u = float(x)/float(subdX);
                theta = PI * 2. * u;
                vertices.push_back( ofVec3f(radius * sin( theta ),
                                            -height/2.,
                                            radius * cos(theta)));
            }
            vertices.push_back( ofVec3f(0,-height/2.,0) );
            
            //faces
            int capCenter = vertices.size()-1;
            for( int x=0; x<subdX; x++){
                addFace(x + vCount, x+1+vCount, capCenter );
            }
            
            //top cap
            //vertices
            vCount = vertices.size();
            for(int x=0; x<=subdX; x++){
                u = float(x)/float(subdX);
                theta = PI * 2. * u;
                vertices.push_back( ofVec3f(radius * sin( theta ),
                                            height/2.,
                                            radius * cos(theta)));
            }
            vertices.push_back( ofVec3f(0,height/2.,0) );
            
            //faces
            capCenter = vertices.size()-1;
            for( int x=0; x<subdX; x++){
                addFace(x + vCount, x+1+vCount, capCenter );
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
    };
    
    
    class CubeGeometry : public Geometry{
    public:
        
        CubeGeometry(float width = 1.f, float height = 1.f, float depth = 1.f){
            bUpdateFaceIndices = false;
            bUpdateVertexData = true;
            
            init( width, height, depth);
        };
        ~CubeGeometry(){
        }
        
        void init( float width = 1.f, float height = 1.f, float depth = 1.f ){
            clearData();
            
            vector<ofVec3f> corners(8);
            
            width *= .5;
            height *= .5;
            depth *= .5;
            
            corners[0].set( -width, -height, depth );
            corners[1].set( width, -height, depth );
            corners[2].set( width, height, depth );
            corners[3].set( -width, height, depth );
            
            corners[4].set( -width, -height, -depth );
            corners[5].set( width, -height, -depth );
            corners[6].set( width, height, -depth );
            corners[7].set( -width, height, -depth );
            
            //front and back
            vertices.push_back( corners[0] );
            vertices.push_back( corners[1] );
            vertices.push_back( corners[2] );
            vertices.push_back( corners[3] );
            vertices.push_back( corners[4] );
            vertices.push_back( corners[5] );
            vertices.push_back( corners[6] );
            vertices.push_back( corners[7] );
            texCoords.push_back( ofVec2f( 0, 0 ));
            texCoords.push_back( ofVec2f( 0, 1 ));
            texCoords.push_back( ofVec2f( 1, 1 ));
            texCoords.push_back( ofVec2f( 1, 0 ));
            texCoords.push_back( ofVec2f( 0, 0 ));
            texCoords.push_back( ofVec2f( 0, 1 ));
            texCoords.push_back( ofVec2f( 1, 1 ));
            texCoords.push_back( ofVec2f( 1, 0 ));
            addFace(0,1,2,3);//front
            addFace(4,7,6,5);//back
            
            //left and right
            vertices.push_back( corners[0] );
            vertices.push_back( corners[1] );
            vertices.push_back( corners[2] );
            vertices.push_back( corners[3] );
            vertices.push_back( corners[4] );
            vertices.push_back( corners[5] );
            vertices.push_back( corners[6] );
            vertices.push_back( corners[7] );
            texCoords.push_back( ofVec2f(0,0));
            texCoords.push_back( ofVec2f( 0,0 ));
            texCoords.push_back( ofVec2f( 1,0 ));
            texCoords.push_back( ofVec2f(0,1));
            texCoords.push_back( ofVec2f(1,0 ));
            texCoords.push_back( ofVec2f( 0,1 ));
            texCoords.push_back( ofVec2f( 1,1 ));
            texCoords.push_back( ofVec2f(1,1));

            addFace(8,11,15,12);
            addFace(9,13,14,10);
            
            //top and bottom
            vertices.push_back( corners[0] );
            vertices.push_back( corners[1] );
            vertices.push_back( corners[2] );
            vertices.push_back( corners[3] );
            vertices.push_back( corners[4] );
            vertices.push_back( corners[5] );
            vertices.push_back( corners[6] );
            vertices.push_back( corners[7] );
            texCoords.push_back( ofVec2f(0,0));
            texCoords.push_back( ofVec2f(1,0));
            texCoords.push_back( ofVec2f( 0,0 ));
            texCoords.push_back( ofVec2f( 1,0 ));
            texCoords.push_back( ofVec2f(0,1));
            texCoords.push_back( ofVec2f(1,1));
            texCoords.push_back( ofVec2f( 0,1 ));
            texCoords.push_back( ofVec2f( 1,1 ));

            addFace(16,20,21,17);
            addFace(18,22,23,19);
            
            //calculate normals
            calcVertexNotmals();
            
            //add indices
            updateFaceIndices();
            
            addAttribute( "position", vertices );
            addAttribute( "normal", normals );
            addAttribute( "texCoord", texCoords );
            addIndices( indices );
        }
    };
}


//    class Geometry{
//    public:
//        Geometry(){};
//        ~Geometry(){};
//
//
//        bool attrExists( string name );
//        void* attrError( string _name );
//
//        VertexAttribute<int>* addAttr( string name, int v );
//        VertexAttribute<float>* addAttr( string name, float v );
//        VertexAttribute<ofVec2f>* addAttr( string name, ofVec2f v );
//        VertexAttribute<ofVec3f>* addAttr( string name, ofVec3f v );
//        VertexAttribute<ofVec4f>* addAttr( string name, ofVec4f v );
//
//        VertexAttribute<int>* addIndices( vector <int>* _indices ){ return addAttr1i( "indices", _indices )->indexBuffer();}
//
//        VertexAttribute<int>* addAttr( string _name, vector<int>& _data );
//        VertexAttribute<float>* addAttr( string _name, vector<float>& _data );
//        VertexAttribute<ofVec2f>*  addAttr( string _name, vector<ofVec2f>& _data );
//        VertexAttribute<ofVec3f>*  addAttr( string _name, vector<ofVec3f>& _data );
//        VertexAttribute<ofVec4f>*  addAttr( string _name, vector<ofVec4f>& _data );
//
//        VertexAttribute<int>* addAttr1i( string _name, vector<int>* _data = NULL );
//        VertexAttribute<float>* addAttr1f( string _name, vector<float>* _data = NULL );
//        VertexAttribute<ofVec2f>* addAttr2f( string _name, vector<ofVec2f>* _data = NULL );
//        VertexAttribute<ofVec3f>* addAttr3f( string _name, vector<ofVec3f>* _data = NULL );
//        VertexAttribute<ofVec4f>* addAttr4f( string _name, vector<ofVec4f>* _data = NULL );
//
//
//        //convenience methods, should look familiar to ofMesh
//        void addVertex( ofVec3f v ){    addAttr( name, v );}
//        void addNormal( ofVec3f n ){    addAttr( "normal", n );}
//        void addColor( ofVec3f c ){    addAttr( "color", c );}
//        void addTexCoord( ofVec2f tc ){    addAttr( "texCoord", tc );}
//
//        void addIndex( int index ){ addAttr( "indices", index )->indexBuffer();}
//
//
//        VertexAttribute<int>* getAttr1i( string _name );
//        VertexAttribute<float>* getAttr1f( string _name );
//        VertexAttribute<ofVec2f>* getAttr2f( string _name );
//        VertexAttribute<ofVec3f>* getAttr3f( string _name );
//        VertexAttribute<ofVec4f>* getAttr4f( string _name );
//
//        //getting data
//        vector<int>* getAttrData1i( string name ){  return  attrExists( name )? getAttr1i( name )->data : NULL;}
//        vector<float>* getAttrData1f( string name ){  return  attrExists( name )? getAttr1f( name )->data : NULL;}
//        vector<ofVec2f>* getAttrData2f( string name ){  return  attrExists( name )? getAttr2f( name )->data : NULL;}
//        vector<ofVec3f>* getAttrData3f( string name ){  return  attrExists( name )? getAttr3f( name )->data : NULL;}
//        vector<ofVec4f>* getAttrData4f( string name ){  return  attrExists( name )? getAttr4f( name )->data : NULL;}
//
//        void getAttrData( string name, vector<int>* v ){  v = attrExists( name )? getAttr1i( name )->data : NULL;}
//        void getAttrData( string name, vector<float>*& v ){ v = attrExists( name )? getAttr1f( name )->data : NULL;}
//        void getAttrData( string name, vector<ofVec2f>*& v ){  v =   attrExists( name )? getAttr2f( name )->data : NULL;}
//        void getAttrData( string name, vector<ofVec3f>*& v ){  v =   attrExists( name )? getAttr3f( name )->data : NULL;}
//        void getAttrData( string name, vector<ofVec4f>*& v ){  v =   attrExists( name )? getAttr4f( name )->data : NULL;}
//
////        void update(){
////            for(it1i=attr1i.begin()
////        }
//        void bindAttr( string name ){
//            switch (attributes[name]) {
//                case GL_INT:
//                        attr1i[name].bind();
//                    break;
//                case GL_FLOAT:
//                        attr1f[name].bind();
//                    break;
//                case GL_FLOAT_VEC2:
//                        attr2f[name].bind();
//                    break;
//                case GL_FLOAT_VEC3:
//                        attr3f[name].bind();
//                    break;
//                case GL_FLOAT_VEC4:
//                        attr4f[name].bind();
//                    break;
//
//                default:
//                    break;
//            }
//        }
//
//        void updateAttr( string name ){
//            switch (attributes[name]) {
//                case GL_INT:
//                    if(attr1i[name].needsUpdate){
//                        attr1i[name].update();
//                    }
//                    break;
//                case GL_FLOAT:
//                    if(attr1f[name].needsUpdate){
//                        attr1f[name].update();
//                    }
//                    break;
//                case GL_FLOAT_VEC2:
//                    if(attr2f[name].needsUpdate){
//                        attr2f[name].update();
//                    }
//                    break;
//                case GL_FLOAT_VEC3:
//                    if(attr3f[name].needsUpdate){
//                        attr3f[name].update();
//                    }
//                    break;
//                case GL_FLOAT_VEC4:
//                    if(attr4f[name].needsUpdate){
//                        attr4f[name].update();
//                    }
//                    break;
//
//                default:
//                    break;
//            }
//        }
//
//        void draw( GLenum renderType = GL_TRIANGLES, int count=-1, int start=0, bool bUseIndices = true){
////            vector<int>* indices = getAttr1i( "indices" );
//
////            if( bUseIndices && indicesId != -1 ){
////                glDrawElements( renderType, (count == -1)? getIndexCount() : count , GL_UNSIGNED_INT, (const GLvoid *)start );
////            }
////            else{
////                glDrawArrays( renderType, start, (count == -1)? getVertexCount() : count );
////            }
//        }
//
//    private:
//
//        map< string, VertexAttribute<int> > attr1i;
//        map< string, VertexAttribute<float> > attr1f;
//        map< string, VertexAttribute<ofVec2f> > attr2f;
//        map< string, VertexAttribute<ofVec3f> > attr3f;
//        map< string, VertexAttribute<ofVec4f> > attr4f;
//
//        map< string, VertexAttribute<int> >::iterator       it1i;
//        map< string, VertexAttribute<float> >::iterator     it1f;
//        map< string, VertexAttribute<ofVec2f> >::iterator   it2f;
//        map< string, VertexAttribute<ofVec3f> >::iterator   it3f;
//        map< string, VertexAttribute<ofVec4f> >::iterator   it4f;
//
//        map <string, GLenum> attributes;
//    };
