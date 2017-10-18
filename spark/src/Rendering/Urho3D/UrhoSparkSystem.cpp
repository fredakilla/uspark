#include "Rendering/Urho3D/UrhoSparkSystem.h"

namespace Urho3D {

UrhoSparkSystem::UrhoSparkSystem(Context* context) :
    Drawable(context, DRAWABLE_GEOMETRY)
{
    _bufferDirty = true;
}

void UrhoSparkSystem::RegisterObject(Context* context)
{
    context->RegisterFactory<UrhoSparkSystem>();

    URHO3D_COPY_BASE_ATTRIBUTES(Drawable);
}

void UrhoSparkSystem::UpdateBatches(const FrameInfo& frame)
{
    // Update information for renderer about this drawable
    distance_ = frame.camera_->GetDistance(GetWorldBoundingBox().Center());
    batches_[0].distance_ = distance_;

    // Calculate scaled distance for animation LOD
    float scale = GetWorldBoundingBox().Size().DotProduct(DOT_SCALE);
    if (scale > M_EPSILON)
        lodDistance_ = frame.camera_->GetLodDistance(distance_, scale, lodBias_);
    else
        lodDistance_ = 0.0f;   


    // for each group, get renderer and update camera view
    for (size_t i = 0; i < _system->getNbGroups(); ++i)
    {
        // Update camera view for quad renderer to align sprites faces to camera
        SPK::URHO::IUrho3DRenderer* renderer = reinterpret_cast<SPK::URHO::IUrho3DRenderer*>(_system->getGroup(i)->getRenderer().get());
        renderer->updateView(frame.camera_);
    }

    // update spark system camera position
    _system->setCameraPosition(SPK::Vector3D(frame.camera_->GetView().m03_, frame.camera_->GetView().m13_, frame.camera_->GetView().m23_));
}

UpdateGeometryType UrhoSparkSystem::GetUpdateGeometryType()
{
    if (_bufferDirty)
        return UPDATE_MAIN_THREAD; // will enable call to UpdateGeometry
    else
        return UPDATE_NONE; // will disable call to UpdateGeometry

}

void UrhoSparkSystem::UpdateGeometry(const FrameInfo& frame)
{
    // for each group, get render buffer to set batches geometries
    for (size_t i = 0; i < _system->getNbGroups(); ++i)
    {        
        SPK::URHO::IUrho3DBuffer* renderBuffer = (SPK::URHO::IUrho3DBuffer*)_system->getGroup(i)->getRenderBuffer();

        if(!renderBuffer)
            return;

        batches_[i].geometry_ = renderBuffer->getGeometry();
    }

    _bufferDirty = false;
}

void UrhoSparkSystem::HandleUpdate(StringHash eventType,VariantMap& eventData)
{
    using namespace Update;
    float timeStep = eventData[P_TIMESTEP].GetFloat();

    if(_system)
    {
        // tarnsform spark system with node tranformation
        _system->getTransform().setNC(node_->GetWorldTransform().Data());

        _system->updateParticles(timeStep);
        _system->renderParticles();

        // update bounding box flag to force bbox calculation
        worldBoundingBoxDirty_ = true;
    }
}

void UrhoSparkSystem::OnNodeSet(Node* node)
{
    Drawable::OnNodeSet(node);

    if (node)
    {
        Scene* scene = GetScene();
        if (scene && IsEnabledEffective())
        {
            SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(UrhoSparkSystem, HandleUpdate));
        }
    }
    else
    {
        UnsubscribeFromEvent(E_UPDATE);
    }
}

void UrhoSparkSystem::OnSetEnabled()
{
    Drawable::OnSetEnabled();

    Scene* scene = GetScene();
    if (scene)
    {
        if (IsEnabledEffective())
        {
            SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(UrhoSparkSystem, HandleUpdate));
        }
        else
        {
            UnsubscribeFromEvent(E_UPDATE);
        }
    }
}

void UrhoSparkSystem::OnWorldBoundingBoxUpdate()
{
    if(_system)
    {
        SPK::Vector3D AABBMin = _system->getAABBMin();
        SPK::Vector3D AABBMax = _system->getAABBMax();
        boundingBox_ = BoundingBox(Vector3(AABBMin.x, AABBMin.y, AABBMin.z),
                                   Vector3(AABBMax.x, AABBMax.y, AABBMax.z));

        worldBoundingBox_ = boundingBox_;
    }
    else
        worldBoundingBox_ = boundingBox_.Transformed(node_->GetWorldTransform());

}

void UrhoSparkSystem::SetSystem(SPK::Ref<SPK::System> system)
{
    //_system = SPK::SPKObject::copy(system);
    _system = system;

    if(_system)
    {
        // force spark to use axis aligned bounding box as urho3d culling need it
        _system->enableAABBComputation(true);

        // get nb groups in system and resize batches
        size_t nbGroup = _system->getNbGroups();
        batches_.Resize(nbGroup);

        // for each group, set a batch
        for (size_t i = 0; i < nbGroup; ++i)
        {
            batches_[i].geometryType_ = GEOM_STATIC;

            SPK::URHO::IUrho3DRenderer* renderer = reinterpret_cast<SPK::URHO::IUrho3DRenderer*>(_system->getGroup(i)->getRenderer().get());
            batches_[i].material_ = renderer->getMaterial();
        }
    }
}

}
