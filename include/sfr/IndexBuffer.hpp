/*****************************************************************************
 * Simple, Fast Renderer (SFR)                                               *
 * CS249b                                                                    *
 * Matt Fichman                                                              *
 * February, 2011                                                            *
 *****************************************************************************/
#pragma once

#include "sfr/Common.hpp"
#include "sfr/Interface.hpp"

namespace sfr {

/* Contains one attribute array (e.g., normals, indices, etc.) for a mesh. */
class IndexBuffer : public Interface {
public:
    enum Status { SYNCED, DIRTY };

    IndexBuffer(std::string const& name);
    ~IndexBuffer();

    std::string const& name() const;
    GLuint id() const;
    GLuint elementCount() const;
    GLuint elementSize() const;
    GLuint const& element(GLuint index) const;
    Status status() const;

    void elementCountIs(GLuint count);
    void elementIs(GLuint offset, GLuint element);
    void elementEnq(GLuint element) { elementIs(GLuint(element_.size()), element); }
    void statusIs(Status status);

private:
    void syncHardwareBuffers();
    const void* data() const;

    std::string name_; 
    GLuint id_;
    Status status_;
    std::vector<GLuint> element_;
};

}
