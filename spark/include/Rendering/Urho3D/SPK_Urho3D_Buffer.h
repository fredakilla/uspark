#ifndef H_SPK_URHO3DBUFFER
#define H_SPK_URHO3DBUFFER

#include <SPARK_Core.h>

#include "Rendering/Urho3D/SPK_Urho3D_DEF.h"

#include <Urho3D/Urho3DAll.h>

namespace SPK {
namespace URHO {

struct sVertexDeclaration
{
    SPK::Vector3D iPos;
    SPK::Color iColor;
    Urho3D::Vector2 iTexCoord;

    /*
    Position (Vector3)
    Normal (Vector3)
    Color (unsigned char[4], normalized)
    Texcoord1 (Vector2)
    Texcoord2 (Vector2)
    Cubetexcoord1 (Vector3)
    Cubetexcoord2 (Vector3)
    Tangent (Vector4)
    Blendweights (float[4])
    Blendindices (unsigned char[4])
    Instancematrix1-3 (Vector4)
    Object index (int, not supported on D3D9)
    */

};


class SPK_URHO_PREFIX IUrho3DBuffer : public RenderBuffer
{
public :

    IUrho3DBuffer(Context *context, size_t nbParticles, size_t nbVerticesPerParticle, size_t nbIndicesPerParticle);
    ~IUrho3DBuffer();

    void positionAtStart();

    void setNextIndex(unsigned index);
    void setNextVertex(const Vector3D& vertex);
    void setNextColor(const Color& color);
    void skipNextColors(size_t nb);
    void setNextTexCoords(float u,float v);
    void skipNextTexCoords(size_t nb);

    void setIndicesUsed(size_t nb);
    void setVerticesUsed(size_t nb);


    Urho3D::SharedPtr<Urho3D::Geometry> getGeometry();
    Urho3D::VertexBuffer* getVertexBuffer() { return _vb; }
    Urho3D::IndexBuffer* getIndexBuffer() { return _ib; }

    /*void setIndexBufferDirty()
    {
        if(_indexData.Size() != 0)
        {
            _ib->SetSize(_indexData.Size(), true, true);
            _ib->SetData(&_indexData[0]);
        }
    }

    void setVertexBufferDirty()
    {
        if(_vertexData.Size() != 0)
        {
            _vb->SetSize(_vertexData.Size(), _elements, true);
            _vb->SetData(&_vertexData[0]);
        }
    }*/

   /* void prepare(int nbParticles)
    {
        //_geometry->SetDrawRange(TRIANGLE_LIST, 0, _ib->GetIndexCount(),  0, _vb->GetVertexCount());
        //assert(_ib->GetIndexCount() == _indexData.Size());
        //assert(_vb->GetVertexCount() == _vertexData.Size());

       // _geometry->SetDrawRange(TRIANGLE_LIST, 0, _indexData.Size(),  0, _vertexData.Size());

         _geometry->SetDrawRange(TRIANGLE_LIST, 0, nbParticles * 6, 0, nbParticles * 4);

         //_geometry->SetDrawRange(TRIANGLE_LIST, 0, nbParticles);


    }*/


private:

    Urho3D::Context* _context;
    Urho3D::SharedPtr<Urho3D::Geometry> _geometry;
    Urho3D::SharedPtr<Urho3D::VertexBuffer> _vb;
    Urho3D::SharedPtr<Urho3D::IndexBuffer> _ib;


    size_t _nbParticles;
    size_t _nbVerticesPerParticle;
    size_t _nbIndicesPerParticle;

    size_t _currentIndexIndex;
    size_t _currentVertexIndex;
    size_t _currentColorIndex;
    size_t _currentTexCoordIndex;

    Urho3D::PODVector<sVertexDeclaration> _vertexData;
    Urho3D::Vector<unsigned int> _indexData;

};



inline Urho3D::SharedPtr<Urho3D::Geometry> IUrho3DBuffer::getGeometry()
{
    return _geometry;
}

inline void IUrho3DBuffer::positionAtStart()
{
    _currentIndexIndex = 0;
    _currentVertexIndex = 0;
    _currentColorIndex = 0;
    _currentTexCoordIndex = 0;
}

inline void IUrho3DBuffer::setNextIndex(unsigned index)
{
    _indexData[_currentIndexIndex++] = index;
}

inline void IUrho3DBuffer::setNextVertex(const Vector3D& vertex)
{
    _vertexData[_currentVertexIndex++].iPos = vertex; //Urho3D::Vector3(vertex.x, vertex.y, vertex.z);
}

inline void IUrho3DBuffer::setNextColor(const Color& color)
{
    _vertexData[_currentColorIndex++].iColor = color; //color.getARGB();
}

inline void IUrho3DBuffer::skipNextColors(size_t nb)
{
    _currentColorIndex += nb;
}

inline void IUrho3DBuffer::setNextTexCoords(float u,float v)
{
    //_vertexData[_currentTexCoordIndex++].iTexCoord = Urho3D::Vector2(u,v);
    _vertexData[_currentTexCoordIndex].iTexCoord.x_ = u;
    _vertexData[_currentTexCoordIndex].iTexCoord.y_ = v;
    _currentTexCoordIndex++;
}

inline void IUrho3DBuffer::skipNextTexCoords(size_t nb)
{
    _currentTexCoordIndex += nb;
}

}}

#endif
