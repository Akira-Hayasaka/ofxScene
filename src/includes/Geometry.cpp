//
//  Geometry.cpp
//  ofxSceneGraphExample
//
//  Created by lars berg on 7/12/12.
//  Copyright 2012 lars berg. All rights reserved.
//

#include "Geometry.h"
using namespace ofxScene;

//
//bool Geometry::attrExists( string name ){
//    return attributes.count( name ) != 0 ;
//}
//
//void* Geometry::attrError( string _name ){
//    cout << "ofxScene::Geometry - attribute named "<< _name << " already exists. returned NULL"<<endl;
//    return NULL;
//}
//
//VertexAttribute<int>* Geometry::addAttr( string _name, vector<int>& _data ){
//    
//    if(!attrExists( _name )){
//        attributes[_name] = GL_INT;//typeid( int ).name();
//        attr1i[_name].init( _name, &_data );
//        return &attr1i[ _name ];
//    }
//    else    attrError( _name );
//}
//VertexAttribute<float>* Geometry::addAttr( string _name, vector<float>& _data ){
//    if(!attrExists( _name )){
//        attributes[_name] = GL_FLOAT;//typeid( float ).name();
//        attr1f[_name].init( _name, &_data );
//        return &attr1f[ _name ];
//    }
//    else    attrError( _name );
//}
//VertexAttribute<ofVec2f>*  Geometry::addAttr( string _name, vector<ofVec2f>& _data ){
//    if(!attrExists( _name )){
//        attributes[_name] = GL_FLOAT_VEC2;//typeid( ofVec2f ).name();
//        attr2f[_name].init( _name, &_data );
//        return &attr2f[ _name ];
//    }
//    else    attrError( _name );
//}
//VertexAttribute<ofVec3f>*  Geometry::addAttr( string _name, vector<ofVec3f>& _data ){   
//    if(!attrExists( _name )){
//        attributes[_name] = GL_FLOAT_VEC3;//typeid( ofVec3f ).name();
//        attr3f[_name].init( _name, &_data );
//        return &attr3f[ _name ];
//    }
//    else    attrError( _name );
//}
//VertexAttribute<ofVec4f>*  Geometry::addAttr( string _name, vector<ofVec4f>& _data ){   
//    if(!attrExists( _name )){
//        attributes[_name] = GL_FLOAT_VEC4;//typeid( ofVec4f ).name();
//        attr4f[_name].init( _name, &_data );
//        return &attr4f[ _name ];
//    }
//    else    attrError( _name );
//}
//
//
//VertexAttribute<int>* Geometry::addAttr( string name, int v ){
//    if(!attrExists(name)){
//        addAttr1i( name );
//    }
//    if(attributes[name] == GL_INT ){
//        VertexAttribute<int>* attr = getAttr1i( name );
//        attr->data->push_back( v );
//        attr->needsUpdate = true;
//        return attr;
//    }
//    else    return NULL;
//}
//VertexAttribute<float>* Geometry::addAttr( string name, float v ){
//    if(!attrExists(name)){
//        addAttr1f( name );
//    }
//    if(attributes[name] == GL_FLOAT ){
//        VertexAttribute<float>* attr = getAttr1f( name );
//        attr->data->push_back( v );
//        attr->needsUpdate = true;
//        return attr;
//    }
//    else    return NULL;
//}
//VertexAttribute<ofVec2f>* Geometry::addAttr( string name, ofVec2f v ){
//    if(!attrExists(name)){
//        addAttr2f( name );
//    }
//    if(attributes[name] == GL_FLOAT_VEC2 ){
//        VertexAttribute<ofVec2f>* attr = getAttr2f( name );
//        attr->data->push_back( v );
//        attr->needsUpdate = true;
//        return attr;
//    }
//    else    return NULL;
//}
//
//VertexAttribute<ofVec3f>* Geometry::addAttr( string name, ofVec3f v ){
//    if(!attrExists(name)){
//        addAttr3f( name );
//    }
//
//    if(attributes[name] == GL_FLOAT_VEC3 ){        
//        VertexAttribute<ofVec3f>* attr = getAttr3f( name );
//        attr->data->push_back( v );
//        attr->needsUpdate = true;
//        return attr;
//    }
//    else    return NULL;
//}
//
//VertexAttribute<ofVec4f>* Geometry::addAttr( string name, ofVec4f v ){
//    if(!attrExists(name)){
//        addAttr4f( name );
//    }
//    if(attributes[name] == GL_FLOAT_VEC4 ){
//        VertexAttribute<ofVec4f>* attr = getAttr4f( name );
//        attr->data->push_back( v );
//        attr->needsUpdate = true;
//        return attr;
//    }
//    else    return NULL;
//}
//
//
//VertexAttribute<int>* Geometry::getAttr1i( string _name ){
//    if(attrExists( _name )){
//        return &attr1i[_name];
//    }
//    else{
//        cout << "ofxScene::VertexBuffer didn't find attribute "<< _name<<". returned NULL"<<endl;
//        return NULL;
//    }
//}
//VertexAttribute<float>* Geometry::getAttr1f( string _name ){
//    if(attrExists( _name )){
//        return &attr1f[_name];
//    }
//    else{
//        cout << "ofxScene::VertexBuffer didn't find attribute "<< _name<<". returned NULL"<<endl;
//        return NULL;
//    }
//}
//
//VertexAttribute<ofVec2f>* Geometry::getAttr2f( string _name ){
//    if(attrExists( _name )){
//        return &attr2f[_name];
//    }
//    else{
//        cout << "ofxScene::VertexBuffer didn't find attribute "<< _name<<". returned NULL"<<endl;
//        return NULL;
//    }
//}
//
//VertexAttribute<ofVec3f>* Geometry::getAttr3f( string _name ){
//    if(attrExists( _name )){
//        return &attr3f[_name];
//    }
//    else{
//        cout << "ofxScene::VertexBuffer didn't find attribute "<< _name<<". returned NULL"<<endl;
//        return NULL;
//    }
//}
//
//VertexAttribute<ofVec4f>* Geometry::getAttr4f( string _name ){
//    if(attrExists( _name )){
//        return &attr4f[_name];
//    }
//    else{
//        cout << "ofxScene::VertexBuffer didn't find attribute "<< _name<<". returned NULL"<<endl;
//        return NULL;
//    }
//}
//
//VertexAttribute<int>* Geometry::addAttr1i( string _name, vector<int>* _data ){
//    attributes[_name] = GL_INT;
//    attr1i[_name].init( _name, _data );
//    return &attr1i[_name];
//}
//VertexAttribute<float>* Geometry::addAttr1f( string _name, vector<float>* _data ){
//    attributes[_name] = GL_FLOAT;
//    attr1f[_name].init( _name, _data );
//    return &attr1f[_name];
//}
//VertexAttribute<ofVec2f>* Geometry::addAttr2f( string _name, vector<ofVec2f>* _data ){
//    attributes[_name] = GL_FLOAT_VEC2;
//    attr2f[_name].init( _name, _data );
//    return &attr2f[_name];
//}
//VertexAttribute<ofVec3f>* Geometry::addAttr3f( string _name, vector<ofVec3f>* _data ){
//    attributes[_name] = GL_FLOAT_VEC3;
//    attr3f[_name].init( _name, _data );
//    return &attr3f[_name];
//}
//VertexAttribute<ofVec4f>* Geometry::addAttr4f( string _name, vector<ofVec4f>* _data ){ 
//    attributes[_name] = GL_FLOAT_VEC4;
//    attr4f[_name].init( _name, _data );
//    return &attr4f[_name];
//}
//
////void Geometry::addAttr( string name, float v ){
////    if(attributes.count( name ) == 0){
////        attributes[name].name = name;
////        attributes[name].type = GL_FLOAT;
////        attributes[name].needsUpdate = true;
////        data1f[name] = vector<float>();
////    }
////    getAttrData1f(name)->push_back( v );
////}
////
////void Geometry::addAttr( string name, ofVec2f v ){
////    if(attributes.count( name ) == 0){
////        attributes[name].name = name;
////        attributes[name].type = GL_FLOAT_VEC2;
////        attributes[name].needsUpdate = true;
////        data2f[name] = vector<ofVec2f>();
////    }
////    getAttrData2f(name)->push_back( v );
////}
////
////void Geometry::addAttr( string name, ofVec3f v ){
////    if(attributes.count( name ) == 0){
////        attributes[name].name = name;
////        attributes[name].type = GL_FLOAT_VEC3;
////        attributes[name].needsUpdate = true;
////        data3f[name] = vector<ofVec3f>();
////    }
////    getAttrData3f(name)->push_back( v );
////}
////
////void Geometry::addAttr( string name, ofVec4f v ){
////    if(attributes.count( name ) == 0){
////        attributes[name].name = name;
////        attributes[name].type = GL_FLOAT_VEC4;
////        attributes[name].needsUpdate = true;
////        data4f[name] = vector<ofVec4f>();
////    }
////    getAttrData4f(name)->push_back( v );
////}
////
////void Geometry::addAttr( string name, vector<float>& data ){
////    if(attributes.count( name ) == 0 ){
////
////        data1f[name] = data;
////        
////        attributes[name].name = name;
////        attributes[name].type = GL_FLOAT;
////        attributes[name].size = sizeof( float );
////        
////        if(data.size()){
////            updateAttr( name );
////        }
////    }
////    else{
////        cout << "ofxScene::Geometry - attribute "<<name<<" already exists."<<endl;
////    }
////}
////
////void Geometry::addAttr( string name, vector<ofVec2f>& data ){
////    if(attributes.count( name ) == 0 ){
////
////        data2f[name] = data;
////        
////        attributes[name].name = name;
////        attributes[name].type = GL_FLOAT_VEC2;
////        attributes[name].size = sizeof( ofVec2f );
////        
////        if(data.size()){
////            updateAttr( name );
////        }
////    }
////    else{
////        cout << "ofxScene::Geometry - attribute "<<name<<" already exists."<<endl;
////    }
////}
////
////void Geometry::addAttr( string name, vector<ofVec3f>& data ){
////    if(attributes.count( name ) == 0 ){
////
////        data3f[name] = data;
////        
////        attributes[name].name = name;
////        attributes[name].type = GL_FLOAT_VEC3;
////        attributes[name].size = sizeof( ofVec3f );
////        
////        if(data.size()){
////            updateAttr( name );
////        }
////    }
////    else{
////        cout << "ofxScene::Geometry - attribute "<<name<<" already exists."<<endl;
////    }
////}
////
////void Geometry::addAttr( string name, vector<ofVec4f>& data ){
////    if(attributes.count( name ) == 0 ){
////
////        data4f[name] = data;
////        
////        attributes[name].name = name;
////        attributes[name].type = GL_FLOAT_VEC4;
////        attributes[name].size = sizeof( ofVec4f );
////        
////        if(data.size()){
////            updateAttr( name );
////        }
////    }
////    else{
////        cout << "ofxScene::Geometry - attribute "<<name<<" already exists."<<endl;
////    }
////}
////
////
////
////void Geometry::updateAttr( string name ){
////    VertexAttribute<ofVec3f>* attr = &attributes[name];
////    switch (attr->type) {
////        case GL_FLOAT:
////            updateAttr( attr, &data1f[name] );
////            break;
////        case GL_FLOAT_VEC2:
////            updateAttr( attr, &data2f[name] ); 
////            break;
////        case GL_FLOAT_VEC3:
////            updateAttr( attr, &data3f[name] ); 
////            break;
////        case GL_FLOAT_VEC4:
////            updateAttr( attr, &data4f[name] ); 
////            break;
////            
////        default:
////            break;
////    }
////}
////
//////would be nice to have these templated but I was running into issues with vectors of ofVecs
////void Geometry::updateAttr( VertexAttribute<ofVec3f>* attr, vector<float>* data ){
////    if(attr->count != data->size() ){
////        attr->count = data->size();
////        attr->bufferSize = attr->count * attr->size;
////        attr->data = &(*data)[0];
////        
////        initVertexBuffer( *attr );
////    }
////    attr->data = &(*data)[0];
////    setVertexBuffer( *attr );
////    
////    attr->needsUpdate = false;
////}
////
////void Geometry::updateAttr( VertexAttribute<ofVec3f>* attr, vector<ofVec2f>* data ){
////    if(attr->count != data->size() ){
////        attr->count = data->size();
////        attr->bufferSize = attr->count * attr->size;
////        attr->data = &(*data)[0].x;
////        
////        initVertexBuffer( *attr );
////    }
////    attr->data = &(*data)[0].x;
////    setVertexBuffer( *attr );
////    
////    attr->needsUpdate = false;
////}
////
////void Geometry::updateAttr( VertexAttribute<ofVec3f>* attr, vector<ofVec3f>* data ){
////    if(attr->count != data->size() ){
////        attr->count = data->size();
////        attr->bufferSize = attr->count * attr->size;
////        attr->data = &(*data)[0].x;
////        
////        initVertexBuffer( *attr );
////    }
////    attr->data = &(*data)[0].x;
////    setVertexBuffer( *attr );
////    
////    attr->needsUpdate = false;
////}
////
////void Geometry::updateAttr( VertexAttribute<ofVec3f>* attr, vector<ofVec4f>* data ){
////    if(attr->count != data->size() ){
////        attr->count = data->size();
////        attr->bufferSize = attr->count * attr->size;
////        
////        initVertexBuffer( *attr );
////    }
////    attr->data = &(*data)[0].x;
////    setVertexBuffer( *attr );
////    
////    attr->needsUpdate = false;
////}
////
////
////void Geometry::initVertexBuffer( VertexAttribute<ofVec3f>& attr ){
////    cout << "initing attr "<< attr.name << endl;
////    glBindBuffer(GL_ARRAY_BUFFER, attr.bufferId);
////    glBufferData(GL_ARRAY_BUFFER, attr.bufferSize, 0, attr.usage);
////    glBindBuffer(GL_ARRAY_BUFFER, 0);
////}
////void Geometry::setVertexBuffer( VertexAttribute<ofVec3f>& attr ){
////    glBindBuffer(GL_ARRAY_BUFFER, attr.bufferId);
////    glBufferSubData(GL_ARRAY_BUFFER, 0, attr.bufferSize, attr.data );
////    glBindBuffer(GL_ARRAY_BUFFER, 0);
////}
////
////
////
//////***************************
//////      VertexAttribute
//////***************************
////
////VertexAttribute<ofVec3f>* Geometry::getAttr( string name ){
////    if(attributes.count( name ) != 0){
////        return &attributes[ name ];
////    }
////    else{
////        cout << "ofxScene::Geometry - no attribute "<<name <<". returned NULL"<<endl;
////        return NULL;
////    }
////}
////
////vector<float>* Geometry::getAttrData1f( string name ){
////    return  (data1f.count( name ) != 0 )? &data1f[ name ] : NULL;
////}
////
////vector<ofVec2f>* Geometry::getAttrData2f( string name ){
////    return  (data2f.count( name ) != 0)? &data2f[ name ] : NULL;
////}
////
////vector<ofVec3f>* Geometry::getAttrData3f( string name ){
////    return  (data3f.count( name ) != 0)? &data3f[ name ] : NULL;
////}
////
////vector<ofVec4f>* Geometry::getAttrData4f( string name ){
////    return  (data4f.count( name ) != 0)? &data4f[ name ] : NULL;
////}
