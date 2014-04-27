/*****************************************************************************
 * Simple, Fast Renderer (SFR)                                               *
 * CS249b                                                                    *
 * Matt Fichman                                                              *
 * February, 2011                                                            *
 *****************************************************************************/

#include "sfr/Common.hpp"
#include "sfr/DeferredRenderTarget.hpp"

using namespace sfr;

DeferredRenderTarget::DeferredRenderTarget(GLuint w, GLuint h) {
    status_ = DISABLED;

    GLuint formats[] = {
        GL_RGB, // Diffuse
        GL_RGBA, // Specular (+ exponent)
        GL_RGB16F, // Normal (needs pretty good precision, hence 16f)
        GL_RGB, // Emissive 
    };
    // Total: 3+4+6+3 = 16 bytes = ~31MB at 1920x1080
    target_.resize(sizeof(formats)/sizeof(*formats));
            
    // Initialize the framebuffer, which will hold all of the target textures
    glGenFramebuffers(1, &id_);
    glBindFramebuffer(GL_FRAMEBUFFER, id_);

    // Initialize all the render target textures, and bind them to the FBO
    glGenTextures(target_.size(), &target_[0]);
    for (size_t i = 0; i < target_.size(); i++) {
        glBindTexture(GL_TEXTURE_2D, target_[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        
        glTexImage2D(GL_TEXTURE_2D, 0, formats[i], w, h, 0, 
            GL_RGBA, GL_UNSIGNED_BYTE, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, 
            GL_TEXTURE_2D, target_[i], 0);
        buffers_.push_back(GL_COLOR_ATTACHMENT0 + i);
    }

    // Create a depth buffer texture using 32 bit depth
    glGenTextures(1, &depthBuffer_);
    glBindTexture(GL_TEXTURE_2D, depthBuffer_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, w, h, 0,
        GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
        GL_TEXTURE_2D, depthBuffer_, 0);

    // Test the framebuffer configuration
    statusIs(ENABLED);
    if (GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER)) {
        throw ResourceException("couldn't create deferred render target");
    }
    statusIs(DISABLED);
}

DeferredRenderTarget::~DeferredRenderTarget() {
    glDeleteFramebuffers(1, &id_);
    glDeleteTextures(target_.size(), &target_[0]);
    glDeleteTextures(1, &depthBuffer_);
}

GLuint DeferredRenderTarget::targetCount() const {
    return target_.size();
}

GLuint DeferredRenderTarget::target(GLuint index) const {
    return target_[index];
}

GLuint DeferredRenderTarget::id() const {
    return id_;
}

GLuint DeferredRenderTarget::depthBuffer() const {
    return depthBuffer_;
}

DeferredRenderTarget::Status DeferredRenderTarget::status() const {
    return status_;
}

void DeferredRenderTarget::statusIs(Status status) {
    if (status_ == status) {
        return;
    }
    status_ = status;
    if (ENABLED == status_) {
        glBindFramebuffer(GL_FRAMEBUFFER, id_);
        glDrawBuffers(buffers_.size(), &buffers_[0]);
    } else if (DISABLED == status_) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    } else {
        assert(!"invalid enum");
    }
}

