#ifndef H_SPK_URHO3D_POINTRENDERER
#define H_SPK_URHO3D_POINTRENDERER

#include "Rendering/Urho3D/SPK_Urho3D_Renderer.h"
#include "Extensions/Renderers/SPK_PointRenderBehavior.h"

#include <Urho3D/Core/Context.h>

namespace SPK {
namespace URHO {

class SPK_URHO_PREFIX IUrho3DPointRenderer :
        public IUrho3DRenderer,
        public PointRenderBehavior
{
    SPK_IMPLEMENT_OBJECT(IUrho3DPointRenderer)

public:

    static Ref<IUrho3DPointRenderer> create(Urho3D::Context* context, float pointSize = 16.0f);

    Urho3D::Material* getMaterial()
    {
        return _material;
    }
    void setMaterial(Urho3D::Material* material)
    {
        _material =  material;
    }



private:

    static const size_t NB_INDICES_PER_PARTICLE = 1;
    static const size_t NB_VERTICES_PER_PARTICLE = 1;
    PODVector<VertexElement> _elements;
    Urho3D::SharedPtr<Material> _material;

    IUrho3DPointRenderer(Urho3D::Context* context = nullptr, float pointSize = 16.0f);
    IUrho3DPointRenderer(const IUrho3DPointRenderer& renderer);

    virtual RenderBuffer* attachRenderBuffer(const Group& group) const override;
    virtual void render(const Group& group,const DataSet* dataSet,RenderBuffer* renderBuffer) const override;
    virtual void computeAABB(Vector3D& AABBMin,Vector3D& AABBMax,const Group& group,const DataSet* dataSet) const override;

};


inline Ref<IUrho3DPointRenderer> IUrho3DPointRenderer::create(Urho3D::Context* context, float pointSize)
{
    return SPK_NEW(IUrho3DPointRenderer, context, pointSize);
}





}}

#endif
