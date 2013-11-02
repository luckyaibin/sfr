/*****************************************************************************
 * Simple, Fast Renderer (SFR)                                               *
 * CS249b                                                                    *
 * Matt Fichman                                                              *
 * February, 2011                                                            *
 *****************************************************************************/

#include "SFR/Common.hpp"
#include "SFR/AttributeBuffer.hpp"
#include <algorithm>

using namespace SFR;

AttributeBuffer::AttributeBuffer(const std::string& name) {
    name_ = name;
    id_ = 0;
    status_ = DIRTY;
    
    glGenBuffers(1, &id_);
}

AttributeBuffer::~AttributeBuffer() {
    glDeleteBuffers(1, &id_);
}

GLuint AttributeBuffer::id() const {
    return id_;
}

const std::string& AttributeBuffer::name() const {
    return name_;
}

void AttributeBuffer::statusIs(Status status) {
    if (status_ == status) {
        return;
    }
    if (SYNCED == status) {
        syncHardwareBuffers();
    }

    status_ = status;
}

void AttributeBuffer::syncHardwareBuffers() {
    GLuint size = elementCount()*elementSize();
    glBindBuffer(GL_ARRAY_BUFFER, id_);
    glBufferData(GL_ARRAY_BUFFER, size, data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
