#include "Rendering/Urho3D/UrhoSparkSystem.h"

namespace Urho3D
{


SparkParticleEffect::SparkParticleEffect(Context* context) :
    Resource(context)
{
    loadedSystem_ = 0;
}

SparkParticleEffect::~SparkParticleEffect()
{
}

void SparkParticleEffect::RegisterObject(Context* context)
{
    // register urho3d context and register for spark urho3d objects
    SPK::URHO::Urho3DContext::get().registerUrhoContext(context);

    context->RegisterFactory<SparkParticleEffect>();
}

bool SparkParticleEffect::BeginLoad(Deserializer& source)
{
    String extension = GetExtension(source.GetName());

    bool success = false;
    if (extension == ".spk")
    {
        success = BeginLoadSPK(source);
        if (success)
            return true;
    }
    return false;
}

bool SparkParticleEffect::EndLoad()
{
    if(!loadedSystem_)
    {
       return false;
    }

    return true;
}


bool SparkParticleEffect::BeginLoadSPK(Deserializer& source)
{
    String filename = source.GetName();
    filename.Insert(0, "Data/");
    loadedSystem_ = SPK::IO::IOManager::get().load(filename.CString());

    if(!loadedSystem_)
        return false;

    return true;
}

const SPK::Ref<SPK::System> SparkParticleEffect::GetSystem() const
{
    return loadedSystem_;
}

SharedPtr<SparkParticleEffect> SparkParticleEffect::Clone(const String& cloneName) const
{
}

}
