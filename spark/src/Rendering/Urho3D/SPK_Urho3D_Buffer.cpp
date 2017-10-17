//#include <SPARK_Core.h>
#include "Rendering/Urho3D/SPK_Urho3D_Buffer.h"

namespace SPK {
namespace URHO {

IUrho3DBuffer::IUrho3DBuffer(Urho3D::Context* context, size_t nbParticles, size_t nbVerticesPerParticle, size_t nbIndicesPerParticle) :
    RenderBuffer()
  , _context(context)
  , _geometry(nullptr)
  , _nbParticles(nbParticles)
  , _nbVerticesPerParticle(nbVerticesPerParticle)
  , _nbIndicesPerParticle(nbIndicesPerParticle)
  , _currentIndexIndex(0)
  , _currentVertexIndex(0)
  , _currentColorIndex(0)
  , _currentTexCoordIndex(0)
{
    SPK_ASSERT(nbParticles > 0, "IUrho3DBuffer::IUrho3DBuffer - The number of particles cannot be 0");
    SPK_ASSERT(nbVerticesPerParticle > 0, "IUrho3DBuffer::IUrho3DBuffer - The number of vertices per particle cannot be 0");
    SPK_ASSERT(nbIndicesPerParticle > 0, "IUrho3DBuffer::IUrho3DBuffer - The number of indices per particle cannot be 0");

    _vb = new VertexBuffer(_context);
    _ib = new IndexBuffer(_context);
    _geometry = new Geometry(_context);



    _vb = new VertexBuffer(_context);
    _ib = new IndexBuffer(_context);
    _geometry = new Geometry(_context);

    //_vb->SetShadowed(true);
    //_vb->SetSize(nbParticles * _nbVerticesPerParticle, _elements, true);

    //_ib->SetShadowed(true);
    //_ib->SetSize(nbParticles * _nbIndicesPerParticle, true, true);

    _geometry->SetVertexBuffer(0, _vb);
    _geometry->SetIndexBuffer(_ib);

    //_geometry->SetDrawRange(TRIANGLE_LIST, 0, _ib->GetIndexCount(),  0, _vb->GetVertexCount());



    /* PODVector<VertexElement> elements;
        elements.Push(VertexElement(TYPE_VECTOR3, SEM_POSITION));
        elements.Push(VertexElement(TYPE_VECTOR2, SEM_TEXCOORD));*/

   /* _vb->SetShadowed(true);
    //vb->SetSize(vertexCount, elements, false);
    //vb->SetData(vertexData);

    _ib->SetShadowed(true);
    //ib->SetSize(indexCount, true, true);
    //ib->SetData(indexData);

    _geometry->SetVertexBuffer(0, _vb);
    _geometry->SetIndexBuffer(_ib);
    //geom->SetDrawRange(TRIANGLE_LIST, 0, indexCount);
    //geom->SetDrawRange(TRIANGLE_LIST, 0, indexCount, 0, vertexCount );


    //PODVector<VertexElement> elements;
    _elements.Push(VertexElement(TYPE_VECTOR3, SEM_POSITION));
    _elements.Push(VertexElement(TYPE_UBYTE4_NORM, SEM_COLOR));
    _elements.Push(VertexElement(TYPE_VECTOR2, SEM_TEXCOORD));
*/

    //setUsed(nbParticles);

    //setIndicesUsed(nbParticles);
    //setVerticesUsed(nbParticles);




    //unsigned vertexCount = nbParticles * _nbVerticesPerParticle;
    //unsigned indexCount = nbParticles * _nbIndicesPerParticle;

    //_geometry->SetDrawRange(TRIANGLE_LIST, 0, vertexCount, 0, indexCount);



}

IUrho3DBuffer::~IUrho3DBuffer()
{
}

/*void IUrho3DBuffer::setUsed(size_t nb)
    {
        if (nb > _nbParticles) // Prevents the buffers from growing too much
        {
            SPK_LOG_WARNING("IUrho3DBuffer::setUsed - The number of active particles cannot be more than the initial capacity. Value is clamped")
            nb = _nbParticles;
        }

        _vertexData.Resize(nb * _nbVerticesPerParticle);
        _indexData.Resize(nb * _nbIndicesPerParticle);
    }*/


/*

void IUrho3DBuffer::setIndicesUsed(size_t nb)
{
    if (nb > _nbParticles) // Prevents the buffers from growing too much
    {
        SPK_LOG_WARNING("IUrho3DBuffer::setUsed - The number of active particles cannot be more than the initial capacity. Value is clamped")
                nb = _nbParticles;
    }
    _indexData.Resize(nb * _nbIndicesPerParticle);
}

void IUrho3DBuffer::setVerticesUsed(size_t nb)
{
    if (nb > _nbParticles) // Prevents the buffers from growing too much
    {
        SPK_LOG_WARNING("IUrho3DBuffer::setUsed - The number of active particles cannot be more than the initial capacity. Value is clamped")
                nb = _nbParticles;
    }
    //_vertexData.Resize(nb * _nbVerticesPerParticle);

    if(_vb->GetVertexCount() != nb * _nbVerticesPerParticle)
        _vb->SetSize(nb * _nbVerticesPerParticle, _elements, true);
}
*/

}}
