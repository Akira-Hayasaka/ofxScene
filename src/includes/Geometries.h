//
//  Geometry.h
//

#pragma once

namespace ofxScene{
    
    class PlaneGeometry : public Geometry{
    public:
        PlaneGeometry( float width=100, float height=100, int subdX=1, int subdY=1 ){
            init( width, height, subdX, subdY);
        }
        ~PlaneGeometry(){};
        
        void init( float width=100, float height=100, int subdX=1, int subdY=1 ){
            clearData();
            subdX = max( 1, subdX );
            subdY = max( 1, subdY );
            
            float halfWidth = width/2.f;
            float halfHeight = height/2.f;
            
            //vertices
            ofVec2f uv;
            float xStep = 1.f/float( subdX );
            float yStep = 1.f/float( subdY );
            vector< vector<int> > vIndices( subdX+1 );
            for(int x=0; x<=subdX; x++){
                vIndices[x].resize( subdY+1 );
                for(int y=0; y<=subdY; y++){
                    vIndices[x][y] = vertices.size();
                    uv.set(xStep*float(x), yStep*float(y));
                    texCoords.push_back( uv );
                    vertices.push_back( ofVec3f(ofMap(uv.x, 0, 1, -halfWidth, halfWidth),
                                                ofMap(uv.y, 0, 1, -halfHeight, halfHeight),
                                                0));
                    
                }
            }
            
            //faces
            for(int x=0;x<subdX;x++){
                for(int y=0; y<subdY;y++){
                    addFace(vIndices[x][y], vIndices[x+1][y], vIndices[x+1][y+1], vIndices[x][y+1]);
                    //                    addFace(vIndices[x][y], vIndices[x][y+1], vIndices[x+1][y+1], vIndices[x+1][y] );
                }
            }
            
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
    
    
    class SphereGeometry : public Geometry{
    public:
        
        SphereGeometry( float radius = 50, int _subdX = 31, int _subdY = 10, bool weldSeam = false ){
            
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
                addFace(x + vCount, capCenter, x+1+vCount );
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
