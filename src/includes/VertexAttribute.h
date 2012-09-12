//
//  VertexAttribute.h
//  ofxSceneGraphExample
//
//  Created by lars berg on 7/13/12.
//  Copyright 2012 lars berg. All rights reserved.
//

#pragma once
#include "ofMain.h"

namespace ofxScene{
    
    
    class VertexAttributeBase{
    public:
        VertexAttributeBase(){
            attrSize = 4;
        };
        ~VertexAttributeBase(){};
        
        void bind(){
            glBindBuffer( target, bufferId);
        }
        
        void unbind(){
            glBindBuffer( target, 0 );
        }
        
        
        void update( int offset = 0, int _count = -1 ){
            bind();
            glBufferSubData( target, attrSize*offset, (_count == -1)? count*attrSize : _count*attrSize, first );
            unbind();
        }
        
        
        void* first;
        GLuint bufferId;
        GLenum type, target, usage;
        int count, stride, size, attrSize;
    };
    
    template <typename attrType>
    class VertexAttribute : public VertexAttributeBase{
    public:
        VertexAttribute(){};
        ~VertexAttribute(){};
        
        void init( vector<attrType>& _data, void* _first, int _size = 1, GLenum _type = GL_FLOAT, GLenum _target = GL_ARRAY_BUFFER, GLenum _usage = GL_STATIC_READ ){
            
            data = &_data;
            first = _first;
            target = _target;
            type = _type;
            count = data->size();
            size = _size;
            stride = 0;
            usage = _usage;
            attrSize = sizeof(attrType);
            
            glGenBuffers(1, &bufferId);
            glBindBuffer( target, bufferId);
            glBufferData( target, attrSize*count, first, usage );
            glBindBuffer( target, 0);
        }
        
        vector<attrType>* data;
    };
       
}