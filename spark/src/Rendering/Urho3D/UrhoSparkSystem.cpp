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
    //URHO3D_MIXED_ACCESSOR_ATTRIBUTE("Material", GetMaterialAttr, SetMaterialAttr, ResourceRef, ResourceRef(Material::GetTypeStatic()), AM_DEFAULT);
    //URHO3D_ACCESSOR_ATTRIBUTE("Segments", GetNumTails, SetNumTails, unsigned int, 10, AM_DEFAULT);
}

void UrhoSparkSystem::UpdateBatches(const FrameInfo& frame)
{
    // Update information for renderer about this drawable
    distance_ = frame.camera_->GetDistance(GetWorldBoundingBox().Center());
    batches_[0].distance_ = distance_;

    // Calculate scaled distance for animation LOD
    float scale = GetWorldBoundingBox().Size().DotProduct(DOT_SCALE);
    // If there are no trail, the size becomes zero, and LOD'ed updates no longer happen. Disable LOD in that case
    if (scale > M_EPSILON)
        lodDistance_ = frame.camera_->GetLodDistance(distance_, scale, lodBias_);
    else
        lodDistance_ = 0.0f;


    /// affect les particles en global ou local movement
    //transforms_[0] = node_->GetWorldTransform();
    //transforms_[1] = Matrix3x4(Vector3::ZERO, frame.camera_->GetFaceCameraRotation(
    //   node_->GetWorldPosition(), node_->GetWorldRotation(), FC_DIRECTION, 1), Vector3::ONE);


    for (size_t i = 0; i < _system->getNbGroups(); ++i)
    {
        // Update camera view for quad renderer to align sprites faces to camera
        SPK::URHO::IUrho3DRenderer* renderer = reinterpret_cast<SPK::URHO::IUrho3DRenderer*>(_system->getGroup(i)->getRenderer().get());
        renderer->updateView(frame.camera_);

        // update spark system camera position
        _system->setCameraPosition(SPK::Vector3D(frame.camera_->GetView().m03_, frame.camera_->GetView().m13_, frame.camera_->GetView().m23_));
    }
}

UpdateGeometryType UrhoSparkSystem::GetUpdateGeometryType()
{
    if (_bufferDirty) //bufferDirty_ || bufferSizeDirty_ || vertexBuffer_->IsDataLost() || indexBuffer_->IsDataLost())
        return UPDATE_MAIN_THREAD; // will enable call to UpdateGeometry
    else
        return UPDATE_NONE; // will disable call to UpdateGeometry

}

void UrhoSparkSystem::UpdateGeometry(const FrameInfo& frame)
{
    for (size_t i = 0; i < _system->getNbGroups(); ++i)
    {
        //SPK::URHO::IUrho3DBuffer* renderBuffer = (SPK::URHO::IUrho3DBuffer*)_spkObject->getRenderBuffer();
        SPK::URHO::IUrho3DBuffer* renderBuffer = (SPK::URHO::IUrho3DBuffer*)_system->getGroup(i)->getRenderBuffer();

        if(!renderBuffer)
            return;

        batches_[i].geometry_ = renderBuffer->getGeometry();
        //batches_[i].material_ = renderBuffer->get

    }

    _bufferDirty = false;
}

void UrhoSparkSystem::HandleUpdate(StringHash eventType,VariantMap& eventData)
{
    using namespace Update;
    float timeStep = eventData[P_TIMESTEP].GetFloat();

    if(_system)
    {
        _system->getTransform().setNC(node_->GetWorldTransform().Data());

        _system->updateParticles(timeStep);
        _system->renderParticles();

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
            //worldBoundingBox_ = boundingBox_.Transformed(node_->GetWorldTransform());
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
            //Restart();
            SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(UrhoSparkSystem, HandleUpdate));
        }
        else
        {
            //Stop();
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
    if( _system)
    {
        // force spark to use axis aligned bounding box as urho3d culling need it
        _system->enableAABBComputation(true);



        //ResourceCache* cache = GetSubsystem<ResourceCache>();
         //Material * mat = cache->GetResource<Material>("Materials/Particle.xml");
         //batches_[0].material_ = mat;

        // set material

        size_t nbGroup = _system->getNbGroups();

        batches_.Resize(nbGroup);

        for (size_t i = 0; i < _system->getNbGroups(); ++i)
        {
            batches_[i].geometryType_ = GEOM_STATIC;

            SPK::Ref<SPK::URHO::IUrho3DQuadRenderer> renderer = static_cast<SPK::Ref<SPK::URHO::IUrho3DQuadRenderer>>(_system->getGroup(i)->getRenderer());
            batches_[i].material_ = renderer->getMaterial();//_material;


            /*SPK::Ref<SPK::URHO::IUrho3DPointRenderer> renderer = static_cast<SPK::Ref<SPK::URHO::IUrho3DPointRenderer>>(_system->getGroup(i)->getRenderer());
            assert(renderer);
            batches_[i].material_ = renderer->getMaterial();//_material;
*/


            /*auto mat = renderer->getMaterial()->Clone(); //cache->GetResource<Material>("Materials/Mushroom.xml")->Clone();
            mat->SetRenderOrder(200);	// higher render order
            auto tec = mat->GetTechnique(0)->Clone();
            tec->GetPass(0)->SetDepthTestMode(CMP_ALWAYS);	// Always pass depth test
            mat->SetTechnique(0, tec);*/



            //batches_[i].material_ = mat;

        }

    }
}

};
