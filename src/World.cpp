/*****************************************************************************
 * Simple, Fast Renderer (SFR)                                               *
 * CS249b                                                                    *
 * Matt Fichman                                                              *
 * February, 2011                                                            *
 *****************************************************************************/

#include "SFR/Common.hpp"
#include "SFR/World.hpp"
#include "SFR/Camera.hpp"
#include <algorithm>

using namespace SFR;

World::World() {
    root_ = std::make_shared<Transform>();
    camera_ = std::make_shared<Camera>();
}

Ptr<Transform> World::root() const {
    return root_;
}

Ptr<Camera> World::camera() const {
    return camera_;
}

void World::cameraIs(Ptr<Camera> camera) {
    if (camera_ == camera) {
        return;
    }
    camera_ = camera;
}

