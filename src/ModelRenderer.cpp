/*****************************************************************************
 * Simple, Fast Renderer (SFR)                                               *
 * CS249b                                                                    *
 * Matt Fichman                                                              *
 * February, 2011                                                            *
 *****************************************************************************/

#include "sfr/Common.hpp"
#include "sfr/AssetTable.hpp"
#include "sfr/AttributeBuffer.hpp"
#include "sfr/Camera.hpp"
#include "sfr/IndexBuffer.hpp"
#include "sfr/Material.hpp"
#include "sfr/Mesh.hpp"
#include "sfr/Model.hpp"
#include "sfr/ModelRenderer.hpp"
#include "sfr/Transform.hpp"
#include "sfr/Texture.hpp"
#include "sfr/Scene.hpp"

using namespace sfr;

ModelRenderer::ModelRenderer(Ptr<AssetTable> manager) {
    program_ = manager->assetIs<ModelProgram>("shaders/Model");
    program_->statusIs(Program::LINKED);
}

void ModelRenderer::onState() {
    if (state() == Renderer::ACTIVE) {
        glUseProgram(program_->id());
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
    } else if (state() == Renderer::INACTIVE) {
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
    } else {
        assert(!"Invalid state");
    }
}

void ModelRenderer::operator()(Ptr<Model> model) {
    // Skip transparent objects and objects without materials
    if (!model->material() || model->material()->opacity() < 1.f) {
        return;
    }

    // Set the material parameters for this mesh
    operator()(model->material());
    operator()(model->mesh());
}

void ModelRenderer::operator()(Ptr<Mesh> mesh) {
    if (!mesh || !mesh->indexBuffer() || !scene() || !scene()->camera()) {
        return;
    }

    mesh->statusIs(Mesh::SYNCED);

    // Calculate the normal matrix and pass it to the vertex shader
    Ptr<Camera> camera = scene()->camera();
    Matrix normalMatrix = (camera->viewMatrix() * worldMatrix()).inverse().transpose();

    GLfloat temp[9] = {
        (GLfloat)normalMatrix[0], (GLfloat)normalMatrix[1], (GLfloat)normalMatrix[2],
        (GLfloat)normalMatrix[4], (GLfloat)normalMatrix[5], (GLfloat)normalMatrix[6],
        (GLfloat)normalMatrix[8], (GLfloat)normalMatrix[9], (GLfloat)normalMatrix[10]
    };

    Matrix const worldViewProjectionMatrix = camera->viewProjectionMatrix() * worldMatrix();
    
    // Pass the model matrix to the vertex shader
    glUniformMatrix3fv(program_->normalMatrix(), 1, 0, temp);    
    glUniformMatrix4fv(program_->transform(), 1, 0, worldViewProjectionMatrix.mat4f());

    // Render the mesh
    Ptr<IndexBuffer> buffer = mesh->indexBuffer();
    glBindVertexArray(mesh->id());
    glDrawElements(GL_TRIANGLES, buffer->elementCount(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void ModelRenderer::operator()(Ptr<Material> material) {

    glUniform3fv(program_->ambient(), 1, material->ambientColor().vec4f());
    glUniform3fv(program_->diffuse(), 1, material->diffuseColor().vec4f());
    glUniform3fv(program_->specular(), 1, material->specularColor().vec4f());
    glUniform3fv(program_->emissive(), 1, material->emissiveColor().vec4f());
    glUniform1f(program_->shininess(), material->shininess());

    glActiveTexture(GL_TEXTURE0);
    operator()(material->diffuseMap());
    glActiveTexture(GL_TEXTURE1);
    operator()(material->specularMap());
    glActiveTexture(GL_TEXTURE2);
    operator()(material->normalMap());
    glActiveTexture(GL_TEXTURE3);
    operator()(material->emissiveMap());
}

void ModelRenderer::operator()(Ptr<Texture> texture) {
    if (texture) {
        glBindTexture(GL_TEXTURE_2D, texture->id());
    }
} 
