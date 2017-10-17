#ifndef H_SPK_URHO3D_RENDERER
#define H_SPK_URHO3D_RENDERER

#include "Rendering/Urho3D/SPK_Urho3D_DEF.h"
#include "Rendering/Urho3D/SPK_Urho3D_Buffer.h"

#include "Core/SPK_Renderer.h"

namespace SPK {
namespace URHO {

/// The base renderer for all Urho3D renderers
class SPK_URHO_PREFIX IUrho3DRenderer : public Renderer
{
public :

    /// Destructor of IUrho3DRenderer
    virtual ~IUrho3DRenderer() {}

    virtual void setBlendMode(BlendMode blendMode) override;


    void updateView(Urho3D::Camera* camera)
    {
        _camera = camera;
    }

protected :

    IUrho3DRenderer(Context *context, bool NEEDS_DATASET = false);
    Urho3D::Context* _context;

    Urho3D::Camera* _camera;
};


}}

#endif
