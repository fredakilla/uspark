#include <SPARK_Core.h>
#include "Rendering/Urho3D/SPK_Urho3D_Renderer.h"

namespace SPK {
namespace URHO {

IUrho3DRenderer::IUrho3DRenderer(Urho3D::Context* context, bool NEEDS_DATASET) :
    Renderer(NEEDS_DATASET)
{
    _context = context;
    _camera = nullptr;

    /*material.GouraudShading = true;									// fix for ATI cards
        material.Lighting = false;										// No lights per default
        material.BackfaceCulling = false;								// Deactivates backface culling
        material.MaterialType = irr::video::EMT_ONETEXTURE_BLEND;		// To allow complex blending functions
        setBlendMode(BLEND_MODE_NONE);										// BlendMode is disabled per default*/
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
