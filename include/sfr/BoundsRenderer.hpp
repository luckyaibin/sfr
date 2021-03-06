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

/* Shadow map renderer. */
class BoundsRenderer : public Renderer {
public:
    BoundsRenderer(Ptr<AssetTable> manager);
    void operator()(Ptr<Model> object);
    void operator()(Ptr<Mesh> mesh);
    void operator()(Ptr<SpotLight> light);

    using Renderer::operator();

private:
    void onState();
    Ptr<ModelProgram> program_;
    Ptr<Mesh> unitCube_;
    Ptr<Mesh> unitCone_;
};

}
