/*****************************************************************************
 * Simple, Fast Renderer (SFR)                                               *
 * CS249b                                                                    *
 * Matt Fichman                                                              *
 * February, 2011                                                            *
 *****************************************************************************/

#include "sfr/Common.hpp"
#include "sfr/RibbonRenderer.hpp"
#include "sfr/Ribbon.hpp"
#include "sfr/Program.hpp"
#include "sfr/AssetTable.hpp"
#include "sfr/World.hpp"
#include "sfr/Texture.hpp"
#include "sfr/Camera.hpp"

using namespace sfr;

RibbonRenderer::RibbonRenderer(Ptr<AssetTable> assets) {
    program_ = assets->assetIs<RibbonProgram>("shaders/Ribbon");
    program_->statusIs(Program::LINKED);
}

void RibbonRenderer::onState() {
    if (state() == Renderer::ACTIVE) {
        glUseProgram(program_->id());
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glDepthMask(GL_FALSE);
    } else if (state() == Renderer::INACTIVE) {
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ZERO);
        glDepthMask(GL_TRUE);
    } else {
        assert(!"Invalid state");
    }
}

void RibbonRenderer::operator()(Ptr<Ribbon> ribbon) {
    Ptr<Camera> camera = world()->camera();
    Ptr<Texture> texture = ribbon->texture();
    if (!texture) { return; }

    ribbon->cameraPositionIs(camera->worldTransform().origin());
    ribbon->statusIs(Ribbon::SYNCED);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->id());

    // Pass the matrices to the vertex shader
    Matrix const transform = camera->transform() * worldTransform();
    glUniformMatrix4fv(program_->transform(), 1, 0, transform.mat4f());

    // Render the particles
    Ptr<AttributeBuffer> buffer = ribbon->buffer();
    glBindVertexArray(ribbon->id());
    glDrawArrays(GL_TRIANGLE_STRIP, 0, buffer->elementCount());
    //glDrawArrays(GL_TRIANGLE_STRIP, 0, buffer->elementCount()/2);
    //glDrawArrays(GL_TRIANGLE_STRIP, buffer->elementCount()/2, buffer->elementCount()/2);
    glBindVertexArray(0);

}
