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
    if(loadedSystem_)
    {
        // create manually renderer
        // TODO: remove here and implement renderer serialization in spark

        ResourceCache* cache = GetSubsystem<ResourceCache>();
        Material* material = cache->GetResource<Material>("Materials/Particle.xml");

        for (size_t i=0; i < loadedSystem_->getNbGroups(); ++i)
        {
            SPK::Ref<SPK::URHO::IUrho3DQuadRenderer> quadRenderer = SPK::URHO::IUrho3DQuadRenderer::create(context_);
            quadRenderer->setBlendMode(SPK::BLEND_MODE_ADD);
            quadRenderer->enableRenderingOption(SPK::RENDERING_OPTION_DEPTH_WRITE,false);
            //quadRenderer->setTexture(driver->getTexture("res\\flare.bmp"));
            quadRenderer->setTexturingMode(SPK::TEXTURE_MODE_2D);
            quadRenderer->setScale(0.25f, 0.25f);
            quadRenderer->setMaterial(material);

            loadedSystem_->getGroup(i)->setRenderer(quadRenderer);
        }
    }

    return true;
}


bool SparkParticleEffect::BeginLoadSPK(Deserializer& source)
{
    String filename = source.GetName();
    filename.Insert(0, "Data//");
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
