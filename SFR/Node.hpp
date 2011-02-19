/*****************************************************************************
 * Simple, Fast Renderer (SFR)                                               *
 * CS249b                                                                    *
 * Matt Fichman                                                              *
 * February, 2011                                                            *
 *****************************************************************************/
#pragma once

#include "SFR/Common.hpp"
#include "SFR/Interface.hpp"
#include "SFR/List.hpp"

namespace SFR {

/* Provides functor support for the classes in the scene graph. */
class Node : public Interface {
public:
    class Functor;
        
    Node* next() const;
    void nextIs(Node* node);
    
    virtual void operator()(Functor* functor)=0;

private:
    Ptr<Node> next_;
};

/* Implement this interface to perform processing on the scene. */
class Node::Functor : public Interface {
public:
    virtual void operator()(Camera* camera) {}
    virtual void operator()(Transform* transform) {}
    virtual void operator()(Mesh* mesh) {}
    virtual void operator()(Material* material) {}
    virtual void operator()(Texture* texture) {}
    virtual void operator()(Light* light) {}
    virtual void operator()(AttributeBuffer* buffer) {}
    virtual void operator()(IndexBuffer* buffer) {}
    virtual void operator()(Effect* effect) {}
    virtual void operator()(Shader* shader) {}
};

}