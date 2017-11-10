#include "Rendering/Urho3D/SPK_Urho3D_DEF.h"
#include "Rendering/Urho3D/SPK_Urho3D_QuadRenderer.h"
#include "Core/IO/SPK_IO_Manager.h"


namespace SPK {
namespace URHO {

    Urho3DContext::Urho3DContext()
    {
        registerObjects();
    }

    Urho3DContext& Urho3DContext::get()
    {
        static Urho3DContext instance;
        return instance;
    }

    void Urho3DContext::registerObjects()
    {
        // Urho3D Quad Renderer
        SPK::IO::IOManager::get().registerObject<SPK::URHO::Urho3DQuadRenderer>();
    }

    void Urho3DContext::registerUrhoContext(Urho3D::Context* context)
    {
        urhoContext = context;
    }



    //////////////////////////
    // Conversion functions //
    //////////////////////////


    inline Urho3D::Vector3 spk2urho(const Vector3D& v)
    {
        return Urho3D::Vector3(v.x, v.y, v.z);
    }

    inline SPK::Vector3D urho2spk(const Urho3D::Vector3& v)
    {
        return Vector3D(v.x_, v.y_, v.z_);
    }

    inline Urho3D::Color spk2urho(unsigned char a, unsigned char  r, unsigned char  g, unsigned char  b)
    {
        return Urho3D::Color(r,g,b,a);
    }

    inline const Urho3D::Color spk2urho(SPK::Color c)
    {
        return Urho3D::Color(c.r, c.g, c.g, c.a);
    }

}}
