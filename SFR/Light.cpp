/*****************************************************************************
 * Simple, Fast Renderer (SFR)                                               *
 * CS249b                                                                    *
 * Matt Fichman                                                              *
 * February, 2011                                                            *
 *****************************************************************************/

#include "SFR/Light.hpp"
#include <algorithm>

using namespace SFR;

const Color& Light::ambientColor() const {
    return ambientColor_;
}

const Color& Light::diffuseColor() const {
    return diffuseColor_;
}

const Color& Light::backDiffuseColor() const {
    return backDiffuseColor_;
}

const Color& Light::specularColor() const {
    return specularColor_;
}

const Vector& Light::direction() const {
    return direction_;
}

float Light::constantAttenuation() const {
    return constantAttenuation_;
}

float Light::linearAttenuation() const {
    return linearAttenuation_;
}

float Light::quadraticAttenuation() const {
    return quadraticAttenuation_;
}

float Light::spotCutoff() const {
    return spotCutoff_;
}

float Light::radiusOfEffect() const {
    float a = quadraticAttenuation();
    float b = linearAttenuation();
    float c = constantAttenuation(); 
    float minIntensity = 0.001;

    float d1 = -b + sqrt(b*b - 4*a*(c - 1/minIntensity))/2/a;
    float d2 = -b - sqrt(b*b - 4*a*(c - 1/minIntensity))/2/a;

    return max(d1, d2);
}

Light::Type Light::type() const {
    return type_;
}

void Light::ambientColorIs(const Color& ambient) {
    if (ambientColor_ == ambient) {
        return;
    }
    ambientColor_ = ambient;

    for (size_t i = 0; i < notifiee_.size(); i++) {
        notifiee_[i]->onAmbientColor();
    }
}

void Light::diffuseColorIs(const Color& diffuse) {
    if (diffuseColor_ == diffuse) {
        return;
    }
    diffuseColor_ = diffuse;

    for (size_t i = 0; i < notifiee_.size(); i++) {
        notifiee_[i]->onDiffuseColor();
    }
}

void Light::backDiffuseColorIs(const Color& backDiffuse) {
    if (backDiffuseColor_ = backDiffuse) {
        return;
    }
    backDiffuseColor_ = backDiffuse;

    for (size_t i = 0; i < notifiee_.size(); i++) {
        notifiee_[i]->onBackDiffuseColor();
    }
}

void Light::specularColorIs(const Color& specular) {
    if (specularColor_ == specular) {
        return;
    }
    specularColor_ = specular;

    for (size_t i = 0; i < notifiee_.size(); i++) {
        notifiee_[i]->onSpecularColor();
    }
}

void Light::directionIs(const Vector& direction) {
    if (direction_ == direction) {
        return;
    }

    for (size_t i = 0; i < notifiee_.size(); i++) {
        notifiee_[i]->onDirection();
    }
}

void Light::constantAttenuationIs(float atten) {
    if (constantAttenuation_ == atten) {
        return;
    }
    constantAttenuation_ = atten;

    for (size_t i = 0; i < notifiee_.size(); i++) {
        notifiee_[i]->onConstantAttenuation();
    }
}

void Light::linearAttenuationIs(float atten) {
    if (linearAttenuation_ == atten) {
        return;
    }
    linearAttenuation_ = atten;

    for (size_t i = 0; i < notifiee_.size(); i++) {
        notifiee_[i]->onLinearAttenuation();
    }
}

void Light::quadraticAttenuationIs(float atten) {
    if (quadraticAttenuation_ == atten) {
        return;
    }
    quadraticAttenuation_ = atten;

    for (size_t i = 0; i < notifiee_.size(); i++) {
        notifiee_[i]->onQuadraticAttenuation();
    }
}

void Light::spotCutoffIs(float cutoff) {
    if (spotCutoff_ == cutoff) {
        return;
    }
    spotCutoff_ = cutoff;

    for (size_t i = 0; i < notifiee_.size(); i++) {
        notifiee_[i]->onSpotCutoff();
    }
}

void Light::typeIs(Type type) {
    if (type_ == type) {
        return;
    }
    type_ = type;

    for (size_t i = 0; i < notifiee_.size(); i++) {
        notifiee_[i]->onType();
    }
}

void Light::notifieeNew(Notifiee* notifiee) {
    if (notifiee) {
        notifiee_.push_back(notifiee);
    }
}

void Light::notifieeDel(Notifiee* notifiee) {
    std::remove(notifiee_.begin(), notifiee_.end(), notifiee);
}

void Light::operator()(Functor* functor) {
    functor->operator()(this);
}