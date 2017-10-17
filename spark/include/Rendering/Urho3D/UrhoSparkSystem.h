#ifndef H_SPK_URHOSPARKSYSTEM
#define H_SPK_URHOSPARKSYSTEM

//#include <Urho3D/Urho3DAll.h>
//#include <Urho3D/Core/Context.h>
#include <Urho3D/Graphics/Drawable.h>
//#include <Urho3D/Graphics/VertexBuffer.h>
//#include <Urho3D/Graphics/IndexBuffer.h>
//#include <Urho3D/Graphics/Geometry.h>
//#include <Urho3D/Resource/ResourceCache.h>
//#include <Urho3D/Graphics/Material.h>
//#include <Urho3D/Scene/Node.h>

//#include <Urho3D/Urho3DAll.h>

#include <SPARK_Core.h>
#include <SPARK_URHO3D.h>

namespace Urho3D {


class UrhoSparkSystem : public Drawable
{
    URHO3D_OBJECT(UrhoSparkSystem, Drawable);

public:

    UrhoSparkSystem(Context* context);

    static void RegisterObject(Context* context);

    virtual void UpdateBatches(const FrameInfo& frame) override;
    virtual void UpdateGeometry(const FrameInfo& frame) override;
    virtual UpdateGeometryType GetUpdateGeometryType() override;

    void HandleUpdate(StringHash eventType,VariantMap& eventData);
    void OnNodeSet(Node* node);
    void OnSetEnabled();

protected:
    virtual void OnWorldBoundingBoxUpdate() override;

private:
    bool _bufferDirty;


    //Matrix3x4 transforms_[2];
    //--------------------

private:
    //void HandleUpdate(StringHash eventType, VariantMap& eventData);


    SharedPtr<Geometry> _geometry;
    SPK::Ref<SPK::System> _system;
    //SPK::Ref<SPK::Group> _spkObject;

    //SPK::Ref<T> _spkObject2;

public:
    void SetSystem(SPK::Ref<SPK::System> system);
};

}


#endif



