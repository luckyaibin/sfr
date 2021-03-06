/*****************************************************************************
 * Simple, Fast Renderer (SFR)                                               *
 * CS249b                                                                    *
 * Matt Fichman                                                              *
 * February, 2011                                                            *
 *****************************************************************************/
#pragma once

#include "sfr/Common.hpp"
#include "sfr/Renderer.hpp"

namespace sfr {

/* Renders alpha-blend effects: transparent meshes, particles, etc. */
class AlphaRenderer : public Renderer {
public:
    AlphaRenderer(Ptr<AssetTable> manager);
    void operator()(Ptr<Model> model);
    void operator()(Ptr<Particles> particles);
    void operator()(Ptr<Ribbon> ribbon);
    void operator()(Ptr<Billboards> billboards);
    void operator()(Ptr<Quad> quad);
    void operator()(Ptr<Text> text);

    using Renderer::operator();
    
private:
    void onState();
    void rendererIs(Ptr<Renderer> renderer);

    Ptr<TransparencyRenderer> transparencyRenderer_;
    Ptr<ParticleRenderer> particleRenderer_;
    Ptr<BillboardRenderer> billboardRenderer_;
    Ptr<RibbonRenderer> ribbonRenderer_;
    Ptr<QuadRenderer> quadRenderer_;
    Ptr<TextRenderer> textRenderer_;
    Ptr<Renderer> renderer_;
};

}
