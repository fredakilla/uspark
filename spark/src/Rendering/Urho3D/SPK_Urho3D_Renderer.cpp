#include <SPARK_Core.h>
#include "Rendering/Urho3D/SPK_Urho3D_Renderer.h"

namespace SPK {
namespace URHO {

IUrho3DRenderer::IUrho3DRenderer(Urho3D::Context* context, bool NEEDS_DATASET) :
    Renderer(NEEDS_DATASET)
{
    _context = context;
    _camera = nullptr;
}

void IUrho3DRenderer::setBlendMode(BlendMode blendMode)
{
    switch(blendMode)
    {
    case BLEND_MODE_NONE :
        break;

    case BLEND_MODE_ADD :
        break;

    case BLEND_MODE_ALPHA :
        break;

    default :
        SPK_LOG_WARNING("IUrho3DRenderer::setBlendMode(BlendMode) - Unsupported blending mode. Nothing happens");
        break;
    }
}


}}
