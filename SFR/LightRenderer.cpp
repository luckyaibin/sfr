/*****************************************************************************
 * Simple, Fast Renderer (SFR)                                               *
 * CS249b                                                                    *
 * Matt Fichman                                                              *
 * February, 2011                                                            *
 *****************************************************************************/

#include "SFR/LightRenderer.hpp"
#include "SFR/Effect.hpp"
#include "SFR/Transform.hpp"
#include "SFR/PointLight.hpp"
#include "SFR/HemiLight.hpp"
#include "SFR/SpotLight.hpp"
#include "SFR/ResourceManager.hpp"
#include "SFR/Mesh.hpp"
#include "SFR/Camera.hpp"

#define _USE_MATH_DEFINES
#include <math.h>

using namespace SFR;

LightRenderer::LightRenderer(ResourceManager* manager) {
    unitSphere_ = manager->meshNew("meshes/Sphere.obj");
    pointLight_ = manager->effectNew("shaders/PointLight");
    hemiLight_ = manager->effectNew("shaders/HemiLight");
    spotLight_ = manager->effectNew("shaders/SpotLight");
}

void LightRenderer::operator()(Transform* transform) {
    Matrix previous = modelTransform_;

    modelTransform_ = modelTransform_ * transform->transform();
    for (Iterator<Node> node = transform->children(); node; node++) {
        node(this);
    }

    modelTransform_ = previous;

    // Clear out the current effect
    operator()(static_cast<Effect*>(0));
}

void LightRenderer::operator()(PointLight* light) {
    operator()(pointLight_.ptr());

    // Set the light color, attenuation, and position properties.
    if (diffuse_ != -1) {
        glUniform3fv(diffuse_, 1, light->diffuseColor());
    }
    if (specular_ != -1) {
        glUniform3fv(specular_, 1, light->specularColor());
    }
    if (atten0_ != -1) {
        glUniform1f(atten0_, light->constantAttenuation());
    }
    if (atten1_ != -1) {
        glUniform1f(atten1_, light->linearAttenuation());
    }
    if (atten2_ != -1) {
        glUniform1f(atten2_, light->quadraticAttenuation());
    }

    // Calculate the model transform, and scale the model to cover the light's 
    // area of effect.
    Matrix previous = modelTransform_;
    float radius = light->radiusOfEffect();
    modelTransform_ = modelTransform_ * Matrix::scale(radius, radius, radius);
    
    // This renders the light's bounding volume (usually a sphere)
    operator()(unitSphere_.ptr());
    modelTransform_ = previous;
}

void LightRenderer::operator()(HemiLight* light) {
    operator()(hemiLight_.ptr());

    // Set the light color, attenuation, and direction properties
    if (diffuse_ != -1) {
        glUniform3fv(diffuse_, 1, light->diffuseColor());
    } 
    if (backDiffuse_ != -1) {
        glUniform3fv(backDiffuse_, 1, light->backDiffuseColor());
    }
    if (atten0_ != -1) {
        glUniform1f(atten0_, light->constantAttenuation());
    }
    if (atten1_ != -1) {
        glUniform1f(atten1_, light->linearAttenuation());
    }
    if (atten2_ != -1) {
        glUniform1f(atten2_, light->quadraticAttenuation());
    }
    if (direction_ != -1) {
        Matrix transform = viewTransform_ * modelTransform_;
        Vector direction = transform.normal(light->direction());
        glUniform3fv(direction_, 1, direction);
    }

    // Calculate the model transform, and scale the model to cover the light's 
    // area of effect.
    Matrix previous = modelTransform_;
    float radius = light->radiusOfEffect();
    modelTransform_ = modelTransform_ * Matrix::scale(radius, radius, radius);
    
    // This renders the light's bounding volume (usually a sphere)
    operator()(unitSphere_.ptr());
    modelTransform_ = previous;
}

void LightRenderer::operator()(SpotLight* light) {
    operator()(spotLight_.ptr());

    // Set the light color, attenuation, and direction properties
    if (diffuse_ != -1) {
        glUniform3fv(diffuse_, 1, light->diffuseColor());
    } 
    if (atten0_ != -1) {
        glUniform1f(atten0_, light->constantAttenuation());
    }
    if (atten1_ != -1) {
        glUniform1f(atten1_, light->linearAttenuation());
    }
    if (atten2_ != -1) {
        glUniform1f(atten2_, light->quadraticAttenuation());
    }
    if (spotCutoff_ != -1) {
        float cosCutoff = std::cos((float)M_PI * light->spotCutoff() / 180.f);
        glUniform1f(spotCutoff_, cosCutoff);
    }
    if (spotPower_ != -1) {
        glUniform1f(spotPower_, light->spotPower());
    }
    if (direction_ != -1) {
        Matrix transform = viewTransform_ * modelTransform_;
        Vector direction = transform.normal(light->direction());
        glUniform3fv(direction_, 1, direction);
    }

    // Calculate the model transform, and scale the model to cover the light's 
    // area of effect.
    Matrix previous = modelTransform_;
    float radius = 5.f; // TODO: HACK HACK HACK
    modelTransform_ = modelTransform_ * Matrix::scale(radius, radius, radius);
    
    // This renders the light's bounding volume (usually a sphere)
    operator()(unitSphere_.ptr());
    modelTransform_ = previous;
}

void LightRenderer::operator()(Mesh* mesh) {
    operator()(mesh->attributeBuffer("position"));
    operator()(mesh->indexBuffer());
}

void LightRenderer::operator()(AttributeBuffer* buffer) {
    if (buffer && position_ != -1) {
        buffer->statusIs(AttributeBuffer::SYNCED);
        glEnableVertexAttribArray(position_);
        glBindBuffer(GL_ARRAY_BUFFER, buffer->id());
        GLint size = buffer->elementSize()/sizeof(GLfloat);
        glVertexAttribPointer(position_, size, GL_FLOAT, 0, 0, 0);
    } else if (position_ != -1) {
        glDisableVertexAttribArray(position_);
    }
}

void LightRenderer::operator()(IndexBuffer* buffer) {
    // Set up the view, projection, and inverse projection transforms
    Matrix inverseProjection = projectionTransform_.inverse();
    glUniformMatrix4fv(model_, 1, 0, modelTransform_);
    glUniformMatrix4fv(projection_, 1, 0, projectionTransform_);
    glUniformMatrix4fv(view_, 1, 0, viewTransform_);
    glUniformMatrix4fv(unproject_, 1, 0, inverseProjection);

    // Render the indices of the light's bounding volume
    IndexBuffer* indices = unitSphere_->indexBuffer();
    indices->statusIs(IndexBuffer::SYNCED);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices->id());
    glDrawElements(GL_TRIANGLES, buffer->elementCount(), GL_UNSIGNED_INT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void LightRenderer::operator()(Camera* camera) {
    projectionTransform_ = camera->projectionMatrix();
    viewTransform_ = modelTransform_;
}

void LightRenderer::operator()(Effect* effect) {
    if (effect_.ptr() == effect) {
        return;
    }
    if (effect_) {
        glDisableVertexAttribArray(position_);
    }
    effect_ = effect;
    if (!effect_) {
        glUseProgram(0);
        return;
    }

    // Activate the shader by querying for uniform variables and attributes 
    effect_->statusIs(Effect::LINKED);
    glUseProgram(effect_->id());
    shadowMap_ = glGetUniformLocation(effect_->id(), "shadowMap");
    diffuseBuffer_ = glGetUniformLocation(effect_->id(), "diffuseBuffer");
    specularBuffer_ = glGetUniformLocation(effect_->id(), "specularBuffer");
    normalBuffer_ = glGetUniformLocation(effect_->id(), "normalBuffer");
    depthBuffer_ = glGetUniformLocation(effect_->id(), "depthBuffer");
    diffuse_ = glGetUniformLocation(effect_->id(), "Ld");
    backDiffuse_ = glGetUniformLocation(effect_->id(), "Ldb");
    specular_ = glGetUniformLocation(effect_->id(), "Ls");
    atten0_ = glGetUniformLocation(effect_->id(), "atten0");
    atten1_ = glGetUniformLocation(effect_->id(), "atten1");
    atten2_ = glGetUniformLocation(effect_->id(), "atten2");
    spotCutoff_ = glGetUniformLocation(effect_->id(), "spotCutoff");
    spotPower_ = glGetUniformLocation(effect_->id(), "spotPower");
    direction_ = glGetUniformLocation(effect_->id(), "direction");
    model_ = glGetUniformLocation(effect_->id(), "modelMatrix");
    view_ = glGetUniformLocation(effect_->id(), "viewMatrix");
    projection_ = glGetUniformLocation(effect_->id(), "projectionMatrix");
    unproject_ = glGetUniformLocation(effect_->id(), "unprojectMatrix");
    position_ = glGetAttribLocation(effect_->id(), "positionIn");

    // Set texture samplers
    glUniform1i(diffuseBuffer_, 0);
    glUniform1i(specularBuffer_, 1);
    glUniform1i(normalBuffer_, 2);
    glUniform1i(depthBuffer_, 3);
    glUniform1i(shadowMap_, 4);
}
