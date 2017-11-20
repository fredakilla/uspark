#include <SPARK_Core.h>
#include "Rendering/Urho3D/SPK_Urho3D_Renderer.h"

namespace SPK {
namespace URHO {

IUrho3DRenderer::IUrho3DRenderer(Urho3D::Context* context, bool NEEDS_DATASET) :
    Renderer(NEEDS_DATASET)
{
    _context = context;
    _camera = nullptr;
    _material = nullptr;
    _depthWrite  = false;
    _blendMode = BLEND_ADDALPHA;
    _textureName = "";
    _depthTestMode = CMP_LESSEQUAL;
}

void IUrho3DRenderer::setBlendMode(BlendMode blendMode)
{
    // not used, use setUrhoBlendMode instead.
}

void IUrho3DRenderer::updateView(Urho3D::Camera* camera)
{
    _camera = camera;
}

Urho3D::Material* IUrho3DRenderer::getMaterial()
{
    return _material;
}
void IUrho3DRenderer::setMaterial(Urho3D::Material* material)
{
    _material =  material;
}









Urho3D::String IUrho3DRenderer::getUrhoTexture()
{
    return _textureName;
}

void IUrho3DRenderer::setUrhoTexture(Urho3D::String textureName)
{
    _textureName = textureName;
}

bool IUrho3DRenderer::getUrhoDepthWrite()
{
    return _depthWrite;
}

void IUrho3DRenderer::setUrhoDepthWrite(bool depthWrite)
{
    _depthWrite = depthWrite;
}

Urho3D::BlendMode IUrho3DRenderer::getUrhoBlendMode()
{
    return _blendMode;
}

void IUrho3DRenderer::setUrhoBlendMode(Urho3D::BlendMode blendMode)
{
    _blendMode = blendMode;
}

CompareMode IUrho3DRenderer::getUrhoDepthTestMode()
{
    return _depthTestMode;
}

void IUrho3DRenderer::setUrhoDepthTestMode(CompareMode compareMode)
{
    _depthTestMode = compareMode;
}


}}
