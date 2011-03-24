/*****************************************************************************
 * Simple, Fast Renderer (SFR)                                               *
 * CS249b                                                                    *
 * Matt Fichman                                                              *
 * February, 2011                                                            *
 *****************************************************************************/
#pragma once

#include "SFR/Common.hpp"
#include "SFR/Interface.hpp"
#include <string>
#include <map>
#include <stdexcept>
#include <vector>

namespace SFR {

/* Manages resource loading for named entities. */
class ResourceManager : public Interface {
public:
    class Notifiee;
    ResourceManager();

    Mesh* meshNew(const std::string& name);
    Material* materialNew(const std::string& name);
    Texture* textureNew(const std::string& name);
    TransformNode* nodeNew(const std::string& name);
    InstanceNode* instanceNew(const std::string& name);
    Effect* effectNew(const std::string& name);

    Mesh* mesh(const std::string& name) const;
    Material* material(const std::string& name) const;
    Texture* texture(const std::string& name) const;
    TransformNode* node(const std::string& name) const;
    Effect* effect(const std::string& name) const;
    void notifieeNew(Notifiee* notifiee);
    void notifieeDel(Notifiee* notifiee);

private:
    std::map<std::string, Ptr<Mesh> > mesh_;
    std::map<std::string, Ptr<Material> > material_;
    std::map<std::string, Ptr<Texture> > texture_;
    std::map<std::string, Ptr<TransformNode> > node_;
    std::map<std::string, Ptr<Effect> > effect_;
    std::vector<Notifiee*> notifiee_;

    Ptr<WavefrontLoader> meshLoader_;
    Ptr<MaterialLoader> materialLoader_;
    Ptr<EffectLoader> effectLoader_;
    Ptr<TextureLoader> textureLoader_;
};

class ResourceManager::Notifiee : public Interface {
public:
    virtual void onMeshNew(Mesh* mesh) {}
    virtual void onMaterialNew(Material* material) {}
    virtual void onTextureNew(Texture* texture) {}
    virtual void onNodeNew(TransformNode* transform) {}
    virtual void onEffectNew(Effect* effect) {}
};

}
