#include <SPARK_Core.h>
#include "Rendering/Urho3D/SPK_Urho3D_PointRenderer.h"

#include <Urho3D/Urho3DAll.h>

namespace SPK {
namespace URHO {

IUrho3DPointRenderer::IUrho3DPointRenderer(Urho3D::Context* context, float pointSize) :
    IUrho3DRenderer(context),
    PointRenderBehavior(POINT_TYPE_SQUARE, screenSize)
{
    _camera = nullptr;
    _material = nullptr;

    _elements.Push(VertexElement(TYPE_VECTOR3, SEM_POSITION));
    _elements.Push(VertexElement(TYPE_UBYTE4_NORM, SEM_COLOR));
}

IUrho3DPointRenderer::IUrho3DPointRenderer(const IUrho3DPointRenderer &renderer) :
    IUrho3DRenderer(renderer),
    PointRenderBehavior(renderer)
{
    // used in copy mechanism

    _camera = renderer._camera;
    _material = renderer._material;
    _elements = renderer._elements;
}


RenderBuffer* IUrho3DPointRenderer::attachRenderBuffer(const Group& group) const
{
    // Creates the render buffer
    IUrho3DBuffer* buffer = SPK_NEW(IUrho3DBuffer,_context,group.getCapacity(),NB_VERTICES_PER_PARTICLE,NB_INDICES_PER_PARTICLE);

    Urho3D::IndexBuffer* indexBuffer = buffer->getIndexBuffer();

    unsigned numParticles = group.getCapacity();

    bool largeIndices = (numParticles * NB_VERTICES_PER_PARTICLE) >= 65536;

    if (indexBuffer->GetIndexCount() != numParticles)
        indexBuffer->SetSize(numParticles * NB_INDICES_PER_PARTICLE, largeIndices);

    void* destPtr = indexBuffer->Lock(0, numParticles * NB_INDICES_PER_PARTICLE, true);
    if (!destPtr)
        return nullptr;

    if (!largeIndices)
    {
        unsigned short* dest = (unsigned short*)destPtr;
        unsigned short vertexIndex = 0;
        while (numParticles--)
        {
            dest[0] = vertexIndex;
            dest += NB_INDICES_PER_PARTICLE;
            vertexIndex += NB_VERTICES_PER_PARTICLE;
        }
    }
    else
    {
        unsigned* dest = (unsigned*)destPtr;
        unsigned vertexIndex = 0;
        while (numParticles--)
        {
            dest[0] = vertexIndex;
            dest += NB_INDICES_PER_PARTICLE;
            vertexIndex += NB_VERTICES_PER_PARTICLE;


        }
    }

    indexBuffer->Unlock();
    indexBuffer->ClearDataLost();

    return buffer;
}

void IUrho3DPointRenderer::render(const Group& group,const DataSet* dataSet,RenderBuffer* renderBuffer) const
{
    SPK_ASSERT(renderBuffer != NULL,"IRRQuadRenderer::render(const Group&,const DataSet*,RenderBuffer*) - renderBuffer must not be NULL");
    IUrho3DBuffer& buffer = static_cast<IUrho3DBuffer&>(*renderBuffer);

    Urho3D::VertexBuffer* vertexBuffer = buffer.getVertexBuffer();
    unsigned numParticles = group.getNbParticles();

    if (vertexBuffer->GetVertexCount() != numParticles * NB_VERTICES_PER_PARTICLE)
        vertexBuffer->SetSize(numParticles * NB_VERTICES_PER_PARTICLE, _elements, true);

    float* dest = (float*)vertexBuffer->Lock(0, numParticles * NB_VERTICES_PER_PARTICLE, true);
    if (!dest)
        return;

    for (ConstGroupIterator particleIt(group); !particleIt.end(); ++particleIt)
    {
        const Particle& particle = *particleIt;

        const unsigned& color =  particle.getColor().getABGR() ;

        dest[0] = particle.position().x;
        dest[1] = particle.position().y;
        dest[2] = particle.position().z;
        ((unsigned&)dest[3]) = color;

        dest += 4;
    }

    vertexBuffer->Unlock();
    vertexBuffer->ClearDataLost();

    buffer.getGeometry()->SetDrawRange(POINT_LIST, 0, numParticles * NB_INDICES_PER_PARTICLE, 0, numParticles * NB_VERTICES_PER_PARTICLE);

}

void IUrho3DPointRenderer::computeAABB(Vector3D& AABBMin,Vector3D& AABBMax,const Group& group,const DataSet* dataSet) const
{
    for (ConstGroupIterator particleIt(group); !particleIt.end(); ++particleIt)
    {
        AABBMin.setMin(particleIt->position());
        AABBMax.setMax(particleIt->position());
    }
}


}}
