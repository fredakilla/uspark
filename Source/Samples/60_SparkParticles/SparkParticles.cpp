//
// Copyright (c) 2008-2017 the Urho3D project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UI.h>

#include <Urho3D/Graphics/Technique.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/ThirdParty/Spark/SPARK.h>
#include <Urho3D/ThirdParty/Spark/SPARK_URHO3D.h>

#include "SparkParticles.h"

#include <Urho3D/DebugNew.h>

URHO3D_DEFINE_APPLICATION_MAIN(SparkParticles)


SPK::Ref<SPK::System> _systemCopy;

SparkParticles::SparkParticles(Context* context) :
    Sample(context)
{
    // Register Urho3D Spark Objects.
    SparkParticleEffect::RegisterObject(context_);
    SparkParticle::RegisterObject(context_);
}

void SparkParticles::Start()
{
    // Execute base class startup
    Sample::Start();

    // Build Spark Effect from scratch
    BuildSparkEffectFromScratch();

    // Create the scene content
    CreateScene();

    // Create the UI content
    CreateInstructions();

    // Setup the viewport for displaying the scene
    SetupViewport();

    // Hook up to the frame update events
    SubscribeToEvents();

    // Set the mouse mode to use in the sample
    Sample::InitMouseMode(MM_RELATIVE);
}

void SparkParticles::CreateScene()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    scene_ = new Scene(context_);
    scene_->CreateComponent<Octree>();

    // There is 2 ways of using Spark Particles

    // 1. Load a spark effect file.

    // Create a new child scene node and a create a SparkParticle component into it.
    // Set effect by loading a SparkParticleEffect resource.
    // SparkParticleEffect load .xml or .spk files.
    Node* node2 = scene_->CreateChild("Spark2");
    node2->SetPosition(Vector3(2.0f, 0.0f, 0.0f));
    SparkParticle * sparkComponent2 = node2->CreateComponent<SparkParticle>();
    sparkComponent2->SetEffect(cache->GetResource<SparkParticleEffect>("Spark/Effects/FireAtlasAnim.xml"));


    // 2. Build manually a spark effect.

    // Once effect is builded, assign it to a SparkParticleEffect and add it to the cache.
    SparkParticleEffect* effect = new SparkParticleEffect(context_);
    effect->SetSystem(_systemCopy);
    effect->SetName("MyNewSparkParticleEffectResource");
    cache->AddManualResource(effect); // ! important for clones

    // Now, we can use this effect in a SparkParticle component.
    Node* node1 = scene_->CreateChild("Spark1");
    node1->SetPosition(Vector3(-1.0f, 0.0f, 0.0f));
    SparkParticle * sparkComponent1 = node1->CreateComponent<SparkParticle>();
    sparkComponent1->SetEffect(effect);


    // Create some new particles nodes.
    {
        Node* node = scene_->CreateChild("Spark2");
        node->SetPosition(Vector3(0.0f, 0.0f, 0.0f));
        SparkParticle * spk = node->CreateComponent<SparkParticle>();
        spk->SetEffect(cache->GetResource<SparkParticleEffect>("Spark/Effects/Fire1.xml"));
    }


    // Create a plane.
    Node* planeNode = scene_->CreateChild("Plane");
    planeNode->SetScale(Vector3(10.0f, 1.0f, 10.0f));
    StaticModel* planeObject = planeNode->CreateComponent<StaticModel>();
    planeObject->SetModel(cache->GetResource<Model>("Models/Plane.mdl"));
    planeObject->SetMaterial(cache->GetResource<Material>("Materials/StoneTiled.xml"));

    // Create a directional light.
    Node* lightNode = scene_->CreateChild("DirectionalLight");
    lightNode->SetDirection(Vector3(0.6f, -1.0f, 0.8f));
    Light* light = lightNode->CreateComponent<Light>();
    light->SetLightType(LIGHT_DIRECTIONAL);    

    // Create a camera.
    cameraNode_ = scene_->CreateChild("Camera");
    cameraNode_->CreateComponent<Camera>();
    cameraNode_->SetPosition(Vector3(0.0f, 1.0f, -8.0f));
}

void SparkParticles::BuildSparkEffectFromScratch()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    SPK::Ref<SPK::System> system_ = SPK::System::create(true);
    system_->setName("Test System");

    // Renderer
    SPK::Ref<SPK::URHO::Urho3DQuadRenderer> renderer = SPK::URHO::Urho3DQuadRenderer::create(context_);
    renderer->setTexturingMode(SPK::TEXTURE_MODE_2D);
    //renderer->setTexture(textureParticle);
    renderer->setBlendMode(SPK::BLEND_MODE_ADD);
    renderer->enableRenderingOption(SPK::RENDERING_OPTION_DEPTH_WRITE,false);
    renderer->setScale(0.05f,0.05f);
    renderer->setMaterial(cache->GetResource<Material>("Materials/Particle.xml"));
    renderer->setOrientation(SPK::OrientationPreset::CAMERA_PLANE_ALIGNED);
    //renderer->setOrientation(SPK::OrientationPreset::CAMERA_POINT_ALIGNED);
    //renderer->setOrientation(SPK::OrientationPreset::DIRECTION_ALIGNED);
    //renderer->setOrientation(SPK::OrientationPreset::AROUND_AXIS);
    //renderer->setOrientation(SPK::OrientationPreset::TOWARDS_POINT);
    //renderer->setOrientation(SPK::OrientationPreset::FIXED_ORIENTATION);

    // Emitter
    SPK::Ref<SPK::SphericEmitter> particleEmitter = SPK::SphericEmitter::create(SPK::Vector3D(0.0f,1.0f,0.0f),0.1f * M_PI, 0.1f * M_PI);
    particleEmitter->setZone(SPK::Point::create(SPK::Vector3D(0.0f,0.015f,0.0f)));
    particleEmitter->setFlow(800);
    particleEmitter->setForce(1.5f,1.5f);

    // Obstacle
    SPK::Ref<SPK::Plane> groundPlane = SPK::Plane::create();
    SPK::Ref<SPK::Obstacle> obstacle = SPK::Obstacle::create(groundPlane,0.9f,1.0f);

    // Group
    SPK::Ref<SPK::Group> particleGroup = system_->createGroup(40000);
    particleGroup->addEmitter(particleEmitter);
    particleGroup->addModifier(obstacle);
    particleGroup->setRenderer(renderer);
    particleGroup->addModifier(SPK::Gravity::create(SPK::Vector3D(0.0f,-1.0f,0.0f)));
    particleGroup->setLifeTime(18.0f,18.0f);
    //particleGroup->setColorInterpolator(SPK::ColorSimpleInterpolator::create(0xFFFF0000,0xFF0000FF));
    particleGroup->setColorInterpolator(SPK::ColorSimpleInterpolator::create(0xFFFF00FF,0xFF0000FF));

    _systemCopy = SPK::SPKObject::copy(system_);

    // Spark IO test
    // -------------------------------------------------------------------
    SPK::IO::IOManager::get().save("test.xml", system_);
    SPK::IO::IOManager::get().save("test.spk", system_);
    // -------------------------------------------------------------------
    //_systemCopy = SPK::IO::IOManager::get().load("Data/Spark/Effects/Fire1.xml");
    //-------------------------------------------------------------------

}

void SparkParticles::CreateInstructions()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    UI* ui = GetSubsystem<UI>();

    // Construct new Text object, set string to display and font to use
    Text* instructionText = ui->GetRoot()->CreateChild<Text>();
    instructionText->SetText("Use WASD keys and mouse/touch to move");
    instructionText->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 15);

    // Position the text relative to the screen center
    instructionText->SetHorizontalAlignment(HA_CENTER);
    instructionText->SetVerticalAlignment(VA_CENTER);
    instructionText->SetPosition(0, ui->GetRoot()->GetHeight() / 4);
}

void SparkParticles::SetupViewport()
{
    Renderer* renderer = GetSubsystem<Renderer>();

    // Set up a viewport to the Renderer subsystem so that the 3D scene can be seen. We need to define the scene and the camera
    // at minimum. Additionally we could configure the viewport screen size and the rendering path (eg. forward / deferred) to
    // use, but now we just use full screen and default render path configured in the engine command line options
    SharedPtr<Viewport> viewport(new Viewport(context_, scene_, cameraNode_->GetComponent<Camera>()));
    renderer->SetViewport(0, viewport);
}

void SparkParticles::MoveCamera(float timeStep)
{
    // Do not move if the UI has a focused element (the console)
    if (GetSubsystem<UI>()->GetFocusElement())
        return;

    Input* input = GetSubsystem<Input>();

    // Movement speed as world units per second
    const float MOVE_SPEED = 20.0f;
    // Mouse sensitivity as degrees per pixel
    const float MOUSE_SENSITIVITY = 0.1f;

    // Use this frame's mouse motion to adjust camera node yaw and pitch. Clamp the pitch between -90 and 90 degrees
    IntVector2 mouseMove = input->GetMouseMove();
    yaw_ += MOUSE_SENSITIVITY * mouseMove.x_;
    pitch_ += MOUSE_SENSITIVITY * mouseMove.y_;
    pitch_ = Clamp(pitch_, -90.0f, 90.0f);

    // Construct new orientation for the camera scene node from yaw and pitch. Roll is fixed to zero
    cameraNode_->SetRotation(Quaternion(pitch_, yaw_, 0.0f));

    // Read WASD keys and move the camera scene node to the corresponding direction if they are pressed
    // Use the Translate() function (default local space) to move relative to the node's orientation.
    if (input->GetKeyDown(KEY_W))
        cameraNode_->Translate(Vector3::FORWARD * MOVE_SPEED * timeStep);
    if (input->GetKeyDown(KEY_S))
        cameraNode_->Translate(Vector3::BACK * MOVE_SPEED * timeStep);
    if (input->GetKeyDown(KEY_A))
        cameraNode_->Translate(Vector3::LEFT * MOVE_SPEED * timeStep);
    if (input->GetKeyDown(KEY_D))
        cameraNode_->Translate(Vector3::RIGHT * MOVE_SPEED * timeStep);
}

void SparkParticles::SubscribeToEvents()
{
    // Subscribe HandleUpdate() function for processing update events
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(SparkParticles, HandleUpdate));
}

void SparkParticles::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
    using namespace Update;

    // Take the frame time step, which is stored as a float
    float timeStep = eventData[P_TIMESTEP].GetFloat();

    // Move the camera, scale movement with time step
    MoveCamera(timeStep);
}
