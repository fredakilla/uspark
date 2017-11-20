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

    void updateView(Urho3D::Camera* camera);

    Urho3D::Material* getMaterial();
    void setMaterial(Urho3D::Material* material);


    Urho3D::String getUrhoTexture();
    void setUrhoTexture(Urho3D::String textureName);

    bool getUrhoDepthWrite();
    void setUrhoDepthWrite(bool depthWrite);

    Urho3D::BlendMode getUrhoBlendMode();
    void setUrhoBlendMode(Urho3D::BlendMode blendMode);

    Urho3D::CompareMode getUrhoDepthTestMode();
    void setUrhoDepthTestMode(Urho3D::CompareMode compareMode);

protected :

    IUrho3DRenderer(Context *context, bool NEEDS_DATASET = false);
    Urho3D::Context*    _context;
    Urho3D::Camera*     _camera;
    Urho3D::SharedPtr<Urho3D::Material>   _material;

    bool                _depthWrite;
    Urho3D::BlendMode   _blendMode;
    Urho3D::String      _textureName;
    CompareMode         _depthTestMode;
};


}}

#endif
