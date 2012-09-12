//
//  Geometry.h
//  ofxSceneGraphExample
//
//  Created by lars berg on 7/12/12.
//  Copyright 2012 lars berg. All rights reserved.
//

#pragma once
#include "ofMain.h"

namespace ofxScene{
    
    class Scene{
    public:
        Scene(){};
        ~Scene(){};
        
        void add( Mesh& mesh ){
            meshes.push_back( &mesh );
        }
        void add( PointLight& pointLight ){
            pointLights.push_back( &pointLight );
        }
        void add( DirectionalLight& directionalLight ){
            directionalLights.push_back( &directionalLight );
        }
        
        void transformPointLights( ofMatrix4x4& modelView ){
            pointLightsTransformed.resize( pointLights.size() );
            for(int i=0; i<pointLights.size(); i++){
                pointLightsTransformed[i] = *pointLights[i];
                pointLightsTransformed[i].setPosition( pointLightsTransformed[i].getPosition() * modelView );
            }
            
        }
        
        void transformDirectionalLights( ofMatrix4x4& modelView  ){
            ofQuaternion rotQuat = modelView.getRotate();
            directionalLightsTransformed.resize( directionalLights.size() );
            for(int i=0; i<directionalLights.size(); i++){
                directionalLightsTransformed[i] =  *directionalLights[i];
                directionalLightsTransformed[i].setDirection( rotQuat * directionalLightsTransformed[i].getDirection() );
            }
            
        }
        
        
        void draw( ofCamera& camera ){
            camera.begin();//the camera clip planes and viewport are set here( privately )

            //set projection and modelview matrices
            projection = camera.getProjectionMatrix();
            modelView = camera.getModelViewMatrix();
            
            transformPointLights( modelView );
            transformDirectionalLights( modelView );

            for(int i=0; i<meshes.size(); i++){
                meshes[i]->shader->setUniform( "NUM_POINT_LIGHTS", int( pointLights.size() ) );
                meshes[i]->shader->setUniform( "POINT_LIGHTS[0]", pointLightsTransformed );
                
                meshes[i]->shader->setUniform( "NUM_DIRECTIONAL_LIGHTS", int( directionalLights.size() ) );
                meshes[i]->shader->setUniform( "DIRECTIONAL_LIGHTS[0]", directionalLightsTransformed );
                
                meshes[i]->draw( modelView, projection );
            }
            
            camera.end();
        }
        
        vector <Mesh*> meshes;
        vector <PointLight*> pointLights;
        vector <PointLight> pointLightsTransformed;
        
        vector <DirectionalLight*> directionalLights;
        vector <DirectionalLight> directionalLightsTransformed;
        
        ofMatrix4x4 projection, modelView;
    };
}