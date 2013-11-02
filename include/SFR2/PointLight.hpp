/*****************************************************************************
 * Simple, Fast Renderer (SFR)                                               *
 * CS249b                                                                    *
 * Matt Fichman                                                              *
 * February, 2011                                                            *
 *****************************************************************************/
#pragma once

#include "sfr/Common.hpp"
#include "sfr/Node.hpp"
#include "sfr/Color.hpp"
#include "sfr/Vector.hpp"
#include "sfr/Matrix.hpp"

namespace sfr {

/* Simple point light with attenuation */
class PointLight : public Node {
public:
    PointLight();
    const Color& diffuseColor() const;
    const Color& specularColor() const;
    float constantAttenuation() const;
    float linearAttenuation() const;
    float quadraticAttenuation() const;
    float radiusOfEffect() const;

    void diffuseColorIs(const Color& diffuse);
    void specularColorIs(const Color& specular);
    void constantAttenuationIs(float atten);
    void linearAttenuationIs(float atten);
    void quadraticAttenuationIs(float atten);

    void operator()(Ptr<Functor> functor);

private:
    Color diffuseColor_;
    Color specularColor_;
    float constantAttenuation_;
    float linearAttenuation_;
    float quadraticAttenuation_;
};

}
