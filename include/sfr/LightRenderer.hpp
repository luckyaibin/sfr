/*****************************************************************************
 * Simple, Fast Renderer (SFR)                                               *
 * CS249b                                                                    *
 * Matt Fichman                                                              *
 * February, 2011                                                            *
 *****************************************************************************/
#pragma once

#include "sfr/Common.hpp"
#include "sfr/Node.hpp"
#include "sfr/Matrix.hpp"

namespace sfr {

/* Deferred shading renderer (2nd pass). */
class LightRenderer : public Node::Functor {
public:
    LightRenderer(Ptr<AssetTable> manager);
    void operator()(Ptr<World> world);
    void operator()(Ptr<Transform> transform);
    void operator()(Ptr<PointLight> light);
    void operator()(Ptr<Effect> effect);
    void operator()(Ptr<HemiLight> light);
    void operator()(Ptr<SpotLight> light);
    void operator()(Ptr<Mesh> mesh);

private:
    Ptr<World> world_;
    Ptr<Mesh> unitSphere_;
    Ptr<Mesh> unitCone_;
    Ptr<Effect> effect_;
    Ptr<Effect> directionalLight_;
    Ptr<Effect> pointLight_;
    Ptr<Effect> hemiLight_;
    Ptr<Effect> spotLight_;

    Matrix transform_;

    GLint shadowMap_;
    GLint diffuseBuffer_;
    GLint specularBuffer_;
    GLint normalBuffer_;
    GLint positionBuffer_;
    GLint emissiveBuffer_;
    GLint depthBuffer_;

    GLint diffuse_;
    GLint backDiffuse_;
    GLint specular_;

    GLint atten0_;
    GLint atten1_;
    GLint atten2_;

    GLint direction_;
    GLint spotCutoff_;
    GLint spotPower_;

    GLint model_;
    GLint view_;
    GLint projection_;
    GLint unproject_;
    GLint light_;
    
    GLint shadowMapSize_;
};

}
