/*****************************************************************************
 * Simple, Fast Renderer (SFR)                                               *
 * CS249b                                                                    *
 * Matt Fichman                                                              *
 * February, 2011                                                            *
 *****************************************************************************/
#pragma once

#ifndef sfr_DEPS_INCLUDED
#define sfr_DEPS_INLUCDED

#define GLEW_STATIC
#ifdef _WIN32
#define sfr_USE_GLEW
#define NOMINMAX
#include <windows.h>
#include <GL/glew.h>
#include <GL/GL.h>
#endif
#ifdef __linux__
#define sfr_USE_GLEW
#include <GL/glew.h>
#include <GL/GL.h>
#endif
#ifdef __APPLE__
#include <OpenGL/GL.h>
#endif

#define _USE_MATH_DEFINES
#include <cmath>
#include <algorithm>
#include <string>
#include <vector>
#include <iostream>
#include <map>
#include <stdexcept>
#include <fstream>
#include <memory>

#include "sfr/Ptr.hpp"

#endif

namespace sfr {

class ShadowRenderer;
class FlatRenderer;
class DeferredRenderer;
class MaterialRenderer;
class LightRenderer;
class TransparencyRenderer;
class Node;
class Transform;
class Instance;
class Mesh;
class Material;
class Texture;
class PointLight;
class HemiLight;
class SpotLight;
class AttributeBuffer;
class IndexBuffer;
class Effect;
class Shader;
class Matrix;
class Quaternion;
class Vector;
class Frustum;
class TexCoord;
class Color;
class WavefrontLoader;
class MaterialLoader;
class EffectLoader;
class TextureLoader;
class Camera;
class AssetTable;
class DepthRenderTarget;
class DeferredRenderTarget;
class Model;
class World;

}

#undef far
#undef near