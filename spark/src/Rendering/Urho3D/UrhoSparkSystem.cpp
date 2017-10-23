#include "Urho3D/Precompiled.h"
#include "Urho3D/Core/Context.h"
#include "Urho3D/Graphics/VertexBuffer.h"
#include "Urho3D/Graphics/IndexBuffer.h"
#include "Urho3D/Graphics/Camera.h"
#include "Urho3D/Graphics/Material.h"
#include "Urho3D/Graphics/OctreeQuery.h"
#include "Urho3D/Graphics/Geometry.h"
#include "Urho3D/Scene/Scene.h"
#include "Urho3D/Scene/SceneEvents.h"
#include "Urho3D/Resource/ResourceCache.h"
#include "Urho3D/IO/Log.h"

#include "Rendering/Urho3D/UrhoSparkSystem.h"

namespace Urho3D
{

extern const char* GEOMETRY_CATEGORY;


UrhoSparkSystem::UrhoSparkSystem(Context* context) :
    Drawable(context, DRAWABLE_GEOMETRY),    
    animationLodBias_(1.0f),
    animationLodTimer_(0.0f),
    //geometry_(new Geometry(context_)),
    //vertexBuffer_(new VertexBuffer(context_)),
    //indexBuffer_(new IndexBuffer(context_)),
    bufferDirty_(true),
    lastUpdateFrameNumber_(M_MAX_UNSIGNED),
    needUpdate_(false),
    sorted_(false),
    previousOffset_(Vector3::ZERO),
    forceUpdate_(false),
    updateInvisible_(false)
{
    //geometry_->SetVertexBuffer(0, vertexBuffer_);
    //geometry_->SetIndexBuffer(indexBuffer_);
    //
    transforms_ = Matrix3x4::IDENTITY;
    //
    //batches_.Resize(1);
    //batches_[0].geometry_ = geometry_;
    //batches_[0].geometryType_ = GEOM_STATIC;
    //batches_[0].worldTransform_ = &transforms_;
    //batches_[0].numWorldTransforms_ = 1;

    updateInvisible_ = false;
    firstRenderSet_ = false;
}

UrhoSparkSystem::~UrhoSparkSystem()
{
}

void UrhoSparkSystem::RegisterObject(Context* context)
{
    context->RegisterFactory<UrhoSparkSystem>(GEOMETRY_CATEGORY);

    URHO3D_ACCESSOR_ATTRIBUTE("Is Enabled", IsEnabled, SetEnabled, bool, true, AM_DEFAULT);
    URHO3D_COPY_BASE_ATTRIBUTES(Drawable);   
    URHO3D_ACCESSOR_ATTRIBUTE("Update Invisible", GetUpdateInvisible, SetUpdateInvisible, bool, false, AM_DEFAULT);   
}

void UrhoSparkSystem::ProcessRayQuery(const RayOctreeQuery& query, PODVector<RayQueryResult>& results)
{
    // If no particles-level testing, use the Drawable test
    if (query.level_ < RAY_TRIANGLE)
    {
        Drawable::ProcessRayQuery(query, results);
        return;
    }

    // Check ray hit distance to AABB before proceeding with particles-level tests
    if (query.ray_.HitDistance(GetWorldBoundingBox()) >= query.maxDistance_)
        return;

    // Approximate the particles as spheres for raycasting
    // TODO
}

void UrhoSparkSystem::HandleScenePostUpdate(StringHash eventType, VariantMap& eventData)
{
    using namespace ScenePostUpdate;
    lastTimeStep_ = eventData[P_TIMESTEP].GetFloat();

    // Update if :
    // - first render not yet effectued (needed to compute particles bounding box)
    // - frame has changed
    // - update invibles
    if (!firstRenderSet_ || updateInvisible_ || viewFrameNumber_ != lastUpdateFrameNumber_)
    {
        lastUpdateFrameNumber_ = viewFrameNumber_;
        needUpdate_ = true;
        MarkForUpdate();
    }
}

void UrhoSparkSystem::Update(const FrameInfo &frame)
{
    Drawable::Update(frame);

    if (!needUpdate_)
        return;

    UpdateParticles();
    OnMarkedDirty(node_);
    needUpdate_ = false;
}

void UrhoSparkSystem::UpdateParticles()
{
    // Transform spark system with node tranformation
    _system->getTransform().setNC(node_->GetWorldTransform().Data());

    // Update particle system
    _system->updateParticles(lastTimeStep_);

    bufferSizeDirty_ = true;
    bufferDirty_ = true;
}


void UrhoSparkSystem::UpdateBatches(const FrameInfo& frame)
{

/*
    // Update information for renderer about this drawable
    distance_ = frame.camera_->GetDistance(GetWorldBoundingBox().Center());
    batches_[0].distance_ = distance_;

    // Calculate scaled distance for animation LOD
    float scale = GetWorldBoundingBox().Size().DotProduct(DOT_SCALE);
    // If there are no particles, the size becomes zero, and LOD'ed updates no longer happen. Disable LOD in that case
    if (scale > M_EPSILON)
        lodDistance_ = frame.camera_->GetLodDistance(distance_, scale, lodBias_);
    else
        lodDistance_ = 0.0f;

    Vector3 worldPos = node_->GetWorldPosition();
    Vector3 offset = (worldPos - frame.camera_->GetNode()->GetWorldPosition());
    if (sorted_ && offset != previousOffset_)
    {
        bufferDirty_ = true;
        previousOffset_ = offset;
    }

    // for each group, get renderer and update camera view
    for (size_t i = 0; i < _system->getNbGroups(); ++i)
    {
        // Update camera view for quad renderer to align sprites faces to camera
        SPK::URHO::IUrho3DRenderer* renderer = reinterpret_cast<SPK::URHO::IUrho3DRenderer*>(_system->getGroup(i)->getRenderer().get());
        renderer->updateView(frame.camera_);
    }

    // update spark system camera position
    _system->setCameraPosition(SPK::Vector3D(frame.camera_->GetView().m03_, frame.camera_->GetView().m13_, frame.camera_->GetView().m23_));
*/



    // Update information for renderer about this drawable
    const BoundingBox& worldBoundingBox = GetWorldBoundingBox();
    const Matrix3x4& worldTransform = node_->GetWorldTransform();
    distance_ = frame.camera_->GetDistance(worldBoundingBox.Center());

    for (unsigned i = 0; i < batches_.Size(); ++i)
    {
        batches_[i].distance_ = distance_;
        //batches_[i].worldTransform_ = &worldTransform;

        SPK::URHO::IUrho3DRenderer* renderer = reinterpret_cast<SPK::URHO::IUrho3DRenderer*>(_system->getGroup(i)->getRenderer().get());
        renderer->updateView(frame.camera_);
    }

    float scale = worldBoundingBox.Size().DotProduct(DOT_SCALE);
    float newLodDistance = frame.camera_->GetLodDistance(distance_, scale, lodBias_);

    if (newLodDistance != lodDistance_)
        lodDistance_ = newLodDistance;

    // update spark system camera position
    _system->setCameraPosition(SPK::Vector3D(frame.camera_->GetView().m03_, frame.camera_->GetView().m13_, frame.camera_->GetView().m23_));



}

void UrhoSparkSystem::UpdateGeometry(const FrameInfo& frame)
{
    if (bufferSizeDirty_)// || batches_[0].geometry_->GetIndexBuffer()->IsDataLost() )// indexBuffer_->IsDataLost()*/)
        UpdateBufferSize();

    if (bufferDirty_)// || batches_[0].geometry_->GetVertexBuffer(0)->IsDataLost() )//vertexBuffer_->IsDataLost()*/)
        UpdateVertexBuffer(frame);
}

UpdateGeometryType UrhoSparkSystem::GetUpdateGeometryType()
{
    if (bufferDirty_ || bufferSizeDirty_)// /*|| vertexBuffer_->IsDataLost() || indexBuffer_->IsDataLost())
        return UPDATE_MAIN_THREAD;
    else
        return UPDATE_NONE;
}



void UrhoSparkSystem::OnWorldBoundingBoxUpdate()
{
    BoundingBox worldBox;

    if(_system)
    {
        SPK::Vector3D AABBMin = _system->getAABBMin();
        SPK::Vector3D AABBMax = _system->getAABBMax();
        worldBox = BoundingBox(Vector3(AABBMin.x, AABBMin.y, AABBMin.z),
                               Vector3(AABBMax.x, AABBMax.y, AABBMax.z));
    }

    worldBoundingBox_ = worldBox;  
}

void UrhoSparkSystem::UpdateBufferSize()
{
    // resize index buffer and vertex buffer
    // set index buffer

    bufferSizeDirty_ = false;
    bufferDirty_ = true;
    forceUpdate_ = true;
}

void UrhoSparkSystem::UpdateVertexBuffer(const FrameInfo& frame)
{
    // set vertex buffer


    // If using animation LOD, accumulate time and see if it is time to update
    if (animationLodBias_ > 0.0f && lodDistance_ > 0.0f)
    {
        animationLodTimer_ += animationLodBias_ * frame.timeStep_ * ANIMATION_LOD_BASESCALE;
        if (animationLodTimer_ >= lodDistance_)
            animationLodTimer_ = fmodf(animationLodTimer_, lodDistance_);
        else
        {
            // No LOD if immediate update forced
            if (!forceUpdate_)
                return;
        }
    }


    // fill geometry buffers
    _system->renderParticles();

    // do only on first render
    if(!firstRenderSet_)
    {
        // for each group, set batches geometries
        for (size_t i = 0; i < _system->getNbGroups(); ++i)
        {
            SPK::URHO::IUrho3DBuffer* renderBuffer = (SPK::URHO::IUrho3DBuffer*)_system->getGroup(i)->getRenderBuffer();
            SPK::Ref<SPK::URHO::IUrho3DRenderer> rendere = _system->getGroup(i)->getRenderer();

            assert(renderBuffer);

            // link Drawable batches geometries to spark particle system renderBuffer geometries
            batches_[i].geometry_ = renderBuffer->getGeometry();
            //batches_[i].material_ = rendere->getMaterial();
        }

        firstRenderSet_ = true;
    }

    bufferDirty_ = false;
    forceUpdate_ = false;
}

void UrhoSparkSystem::Commit()
{
    MarkPositionsDirty();
    MarkNetworkUpdate();
}

void UrhoSparkSystem::MarkPositionsDirty()
{
    Drawable::OnMarkedDirty(node_);
    bufferDirty_ = true;
}

void UrhoSparkSystem::OnSceneSet(Scene* scene)
{
    Drawable::OnSceneSet(scene);

    if (scene && IsEnabledEffective())
        SubscribeToEvent(scene, E_SCENEPOSTUPDATE, URHO3D_HANDLER(UrhoSparkSystem, HandleScenePostUpdate));
    else if (!scene)
         UnsubscribeFromEvent(E_SCENEPOSTUPDATE);
}

void UrhoSparkSystem::OnSetEnabled()
{
    Drawable::OnSetEnabled();

    Scene* scene = GetScene();
    if (scene)
    {
        if (IsEnabledEffective())
            SubscribeToEvent(scene, E_SCENEPOSTUPDATE, URHO3D_HANDLER(UrhoSparkSystem, HandleScenePostUpdate));
        else
            UnsubscribeFromEvent(scene, E_SCENEPOSTUPDATE);
    }
}

void UrhoSparkSystem::SetUpdateInvisible(bool enable)
{
    updateInvisible_ = enable;
    MarkNetworkUpdate();
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
            batches_[i].worldTransform_ = &transforms_;
            batches_[i].numWorldTransforms_ = 1;

            SPK::URHO::IUrho3DRenderer* renderer = reinterpret_cast<SPK::URHO::IUrho3DRenderer*>(_system->getGroup(i)->getRenderer().get());
            batches_[i].material_ = renderer->getMaterial();

            //SPK::URHO::IUrho3DBuffer* renderBuffer = (SPK::URHO::IUrho3DBuffer*)_system->getGroup(i)->getRenderBuffer();
            //batches_[i].geometry_ = renderBuffer->getGeometry();
        }


        //Drawable::OnMarkedDirty(node_);
        //bufferSizeDirty_ = true;
        //MarkNetworkUpdate();

    }
}


}




