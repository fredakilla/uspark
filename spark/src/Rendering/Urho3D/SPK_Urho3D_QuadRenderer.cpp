#include <SPARK_Core.h>
#include "Rendering/Urho3D/SPK_Urho3D_QuadRenderer.h"

namespace SPK {
namespace URHO {

IUrho3DQuadRenderer::IUrho3DQuadRenderer(Urho3D::Context* context, float scaleX, float scaleY) :
    IUrho3DRenderer(context),
    QuadRenderBehavior(scaleX,scaleY),
    Oriented3DRenderBehavior()
{
    // set vertex buffer elements
    _elements.Push(VertexElement(TYPE_VECTOR3, SEM_POSITION));
    _elements.Push(VertexElement(TYPE_UBYTE4_NORM, SEM_COLOR));
    _elements.Push(VertexElement(TYPE_VECTOR2, SEM_TEXCOORD));
}

IUrho3DQuadRenderer::IUrho3DQuadRenderer(const IUrho3DQuadRenderer &renderer) :
    IUrho3DRenderer(renderer),
    QuadRenderBehavior(renderer),
    Oriented3DRenderBehavior(renderer)
{
    // used in copy mechanism

    _camera = renderer._camera;
    _material = renderer._material;
    _elements = renderer._elements;
}

RenderBuffer* IUrho3DQuadRenderer::attachRenderBuffer(const Group& group) const
{
    // Creates the render buffer
    IUrho3DBuffer* buffer = SPK_NEW(IUrho3DBuffer,_context,group.getCapacity(),NB_VERTICES_PER_PARTICLE,NB_INDICES_PER_PARTICLE);

    unsigned numParticles = group.getCapacity();
    bool largeIndices = (numParticles * NB_VERTICES_PER_PARTICLE) >= 65536;

    Urho3D::IndexBuffer* indexBuffer = buffer->getIndexBuffer();

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
            dest[0] = vertexIndex + 0;
            dest[1] = vertexIndex + 1;
            dest[2] = vertexIndex + 2;
            dest[3] = vertexIndex + 2;
            dest[4] = vertexIndex + 3;
            dest[5] = vertexIndex + 0;

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
            dest[0] = vertexIndex + 0;
            dest[1] = vertexIndex + 1;
            dest[2] = vertexIndex + 2;
            dest[3] = vertexIndex + 2;
            dest[4] = vertexIndex + 3;
            dest[5] = vertexIndex + 0;

            dest += NB_INDICES_PER_PARTICLE;
            vertexIndex += NB_VERTICES_PER_PARTICLE;
        }
    }

    indexBuffer->Unlock();
    indexBuffer->ClearDataLost();

    return buffer;
}

void IUrho3DQuadRenderer::render(const Group& group,const DataSet* dataSet,RenderBuffer* renderBuffer) const
{
    SPK_ASSERT(renderBuffer != NULL,"IRRQuadRenderer::render(const Group&,const DataSet*,RenderBuffer*) - renderBuffer must not be NULL");
    IUrho3DBuffer& buffer = static_cast<IUrho3DBuffer&>(*renderBuffer);

    unsigned numParticles = group.getNbParticles();

    buffer.getGeometry()->SetDrawRange(TRIANGLE_LIST, 0, numParticles * NB_INDICES_PER_PARTICLE);

    if(numParticles == 0)
       return;

    // Computes the inverse model view
    assert(_camera);
    Matrix4 invModelView = Matrix4::IDENTITY * _camera->GetView();
    invModelView = invModelView.Transpose().Inverse();

    // select render method
    if ((texturingMode == TEXTURE_MODE_2D)&&(group.isEnabled(PARAM_TEXTURE_INDEX)))
    {
        if (group.isEnabled(PARAM_ANGLE))
            renderParticle = &IUrho3DQuadRenderer::renderAtlasRot;
        else
            renderParticle = &IUrho3DQuadRenderer::renderAtlas;
    }
    else
    {
        if (group.isEnabled(PARAM_ANGLE))
            renderParticle = &IUrho3DQuadRenderer::renderRot;
        else
            renderParticle = &IUrho3DQuadRenderer::renderBasic;
    }

    bool globalOrientation = precomputeOrientation3D(group,
                Vector3D(invModelView.Data()[8],  invModelView.Data()[9],  invModelView.Data()[10]),
                Vector3D(invModelView.Data()[4],  invModelView.Data()[5],  invModelView.Data()[6]),
                Vector3D(invModelView.Data()[12], invModelView.Data()[13], invModelView.Data()[14]));

    if (globalOrientation)
        computeGlobalOrientation3D(group);

    Urho3D::VertexBuffer* vertexBuffer = buffer.getVertexBuffer();

    if (vertexBuffer->GetVertexCount() != numParticles * NB_VERTICES_PER_PARTICLE)
        vertexBuffer->SetSize(numParticles * NB_VERTICES_PER_PARTICLE, _elements, true);

    float* dest = (float*)vertexBuffer->Lock(0, numParticles * NB_VERTICES_PER_PARTICLE);
    assert(dest);

    for (ConstGroupIterator particleIt(group); !particleIt.end(); ++particleIt)
    {
        const Particle& particle = *particleIt;

        if (!globalOrientation)
            computeSingleOrientation3D(particle);

        // call render method to compute quad
        (this->*renderParticle)(particle,buffer);

        // Vertices are drawn in clockwise order (front face).
        // First triangle  : v0,v1,v2
        // Second triangle : v2,v3,v0
        // UV coord are same as DirectX

        //    Vertices     |          UV
        //                 |
        //   v0     v1     |     0,0        1,0
        //     x---x       |        x------x
        //     |\  |       |        |      |
        //     | \ |       |        |      |
        //     |  \|       |        |      |
        //     x---x       |        x------x
        //   v3     v2     |     0,1        1,1

        Vector3D v0 = particle.position() + quadSide() + quadUp(); // top left vertex
        Vector3D v1 = particle.position() - quadSide() + quadUp(); // top right vertex
        Vector3D v2 = particle.position() - quadSide() - quadUp(); // bottom right vertex
        Vector3D v3 = particle.position() + quadSide() - quadUp(); // bottom left vertex

        const unsigned& color =  particle.getColor().getABGR() ;

        dest[0] = v0.x;
        dest[1] = v0.y;
        dest[2] = v0.z;
        ((unsigned&)dest[3]) = color;
        dest[4] = _u0;
        dest[5] = _v0;

        dest[6] = v1.x;
        dest[7] = v1.y;
        dest[8] = v1.z;
        ((unsigned&)dest[9]) = color;
        dest[10] = _u1;
        dest[11] = _v0;

        dest[12] = v2.x;
        dest[13] = v2.y;
        dest[14] = v2.z;
        ((unsigned&)dest[15]) = color;
        dest[16] = _u1;
        dest[17] = _v1;

        dest[18] = v3.x;
        dest[19] = v3.y;
        dest[20] = v3.z;
        ((unsigned&)dest[21]) = color;
        dest[22] = _u0;
        dest[23] = _v1;

        dest += 24;
    }

    vertexBuffer->Unlock();
    vertexBuffer->ClearDataLost();
}

void IUrho3DQuadRenderer::computeAABB(Vector3D& AABBMin,Vector3D& AABBMax,const Group& group,const DataSet* dataSet) const
{
    float diagonal = group.getGraphicalRadius() * std::sqrt(scaleX * scaleX + scaleY * scaleY);
    Vector3D diagV(diagonal,diagonal,diagonal);

    if (group.isEnabled(PARAM_SCALE))
        for (ConstGroupIterator particleIt(group); !particleIt.end(); ++particleIt)
        {
            Vector3D scaledDiagV = diagV * particleIt->getParamNC(PARAM_SCALE);
            AABBMin.setMin(particleIt->position() - scaledDiagV);
            AABBMax.setMax(particleIt->position() + scaledDiagV);
        }
    else
    {
        for (ConstGroupIterator particleIt(group); !particleIt.end(); ++particleIt)
        {
            AABBMin.setMin(particleIt->position());
            AABBMax.setMax(particleIt->position());
        }
        AABBMin -= diagV;
        AABBMax += diagV;
    }
}

void IUrho3DQuadRenderer::renderBasic(const Particle& particle,IUrho3DBuffer& renderBuffer) const
{
    scaleQuadVectors(particle,scaleX,scaleY);
    _u0 = _v0 = 0.0f;
    _u1 = _v1 = 1.0f;
}

void IUrho3DQuadRenderer::renderRot(const Particle& particle,IUrho3DBuffer& renderBuffer) const
{
    rotateAndScaleQuadVectors(particle,scaleX,scaleY);
    _u0 = _v0 = 0.0f;
    _u1 = _v1 = 1.0f;
}

void IUrho3DQuadRenderer::renderAtlas(const Particle& particle,IUrho3DBuffer& renderBuffer) const
{
    scaleQuadVectors(particle,scaleX,scaleY);
    computeAtlasCoordinates(particle);
    _u0 = textureAtlasU0();
    _u1 = textureAtlasU1();
    _v0 = textureAtlasV0();
    _v1 = textureAtlasV1();
}

void IUrho3DQuadRenderer::renderAtlasRot(const Particle& particle,IUrho3DBuffer& renderBuffer) const
{
    rotateAndScaleQuadVectors(particle,scaleX,scaleY);
    computeAtlasCoordinates(particle);
    _u0 = textureAtlasU0();
    _u1 = textureAtlasU1();
    _v0 = textureAtlasV0();
    _v1 = textureAtlasV1();
}

void IUrho3DQuadRenderer::innerImport(const IO::Descriptor& descriptor)
{
    Renderer::innerImport(descriptor);

    ResourceCache* cache = _context->GetSubsystem<ResourceCache>();

    Material * material = nullptr;

    const IO::Attribute* attrib = NULL;

    if (attrib = descriptor.getAttributeWithValue("material"))
    {
        std::string materialName = attrib->getValue<std::string>();
        Material * material = cache->GetResource<Material>(materialName.c_str());
        setMaterial(material);
    }

    if(material)
    if (attrib = descriptor.getAttributeWithValue("texture"))
    {
        std::string textureName = attrib->getValue<std::string>();
        Texture * texture = cache->GetResource<Texture>(textureName.c_str());
        material->SetTexture(TU_DIFFUSE, texture);
    }
}

void IUrho3DQuadRenderer::innerExport(IO::Descriptor& descriptor) const
{
    Renderer::innerExport(descriptor);
    descriptor.getAttribute("material")->setValue<std::string>(getMaterial()->GetName().CString());
    descriptor.getAttribute("texture")->setValue<std::string>(getMaterial()->GetTexture(TU_DIFFUSE)->GetName().CString());
}



}}
