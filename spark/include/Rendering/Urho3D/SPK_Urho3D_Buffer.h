#ifndef H_SPK_URHO3DBUFFER
#define H_SPK_URHO3DBUFFER

#include <SPARK_Core.h>

#include "Rendering/Urho3D/SPK_Urho3D_DEF.h"

#include <Urho3D/Urho3DAll.h>

namespace SPK {
namespace URHO {

class SPK_URHO_PREFIX IUrho3DBuffer : public RenderBuffer
{

public :

    IUrho3DBuffer(Context *context, size_t nbParticles, size_t nbVerticesPerParticle, size_t nbIndicesPerParticle);
    ~IUrho3DBuffer();

    Urho3D::SharedPtr<Urho3D::Geometry>     getGeometry();
    Urho3D::VertexBuffer*                   getVertexBuffer() { return _vb; }
    Urho3D::IndexBuffer*                    getIndexBuffer() { return _ib; }

private:

    Urho3D::Context*                        _context;
    Urho3D::SharedPtr<Urho3D::Geometry>     _geometry;
    Urho3D::SharedPtr<Urho3D::VertexBuffer> _vb;
    Urho3D::SharedPtr<Urho3D::IndexBuffer>  _ib;

};


inline Urho3D::SharedPtr<Urho3D::Geometry> IUrho3DBuffer::getGeometry()
{
    return _geometry;
}

}}

#endif
