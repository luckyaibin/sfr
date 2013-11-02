/*****************************************************************************
 * Simple, Fast Renderer (SFR)                                               *
 * CS249b                                                                    *
 * Matt Fichman                                                              *
 * February, 2011                                                            *
 *****************************************************************************/

#include "SFR/Common.hpp"
#include "SFR/Shader.hpp"

using namespace SFR;

Shader::Shader(const std::string& name, GLenum type) {
    name_ = name;
    id_ = 0;
    status_ = DIRTY;
    type_ = type;

    id_ = glCreateShader(type_);
}

Shader::~Shader() {
    glDeleteShader(id_);
}

const std::string& Shader::name() const {
    return name_;
}

GLenum Shader::type() const {
    return type_;
}

const std::string& Shader::source() const {
    return source_;
}

Shader::Status Shader::status() const {
    return status_;
}

GLuint Shader::id() const {
    return id_;
}

void Shader::sourceIs(const std::string& source) {
    if (source == source_) {
        return;
    }
    source_ = source;
    statusIs(DIRTY);
}

void Shader::statusIs(Status status) {
    if (status_ == status) {
        return;
    }
    if (COMPILED == status) {
        const GLchar* strings = source_.c_str();
        GLint lengths = source_.length();
        glShaderSource(id_, 1, &strings, &lengths);
        glCompileShader(id_);
    }
    status_ = status;
}
