#ifndef H_SPK_URHOSPARKSYSTEM
#define H_SPK_URHOSPARKSYSTEM

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
    virtual void OnNodeSet(Node* node) override;
    virtual void OnSetEnabled() override;
    virtual void OnWorldBoundingBoxUpdate() override;

    void SetSystem(SPK::Ref<SPK::System> system);

    const SPK::Ref<SPK::System> GetSystem() const
    {
        return _system;
    }

private:
    void HandleUpdate(StringHash eventType,VariantMap& eventData);

    bool                    _bufferDirty;
    SharedPtr<Geometry>     _geometry;
    SPK::Ref<SPK::System>   _system;
};

}

#endif



