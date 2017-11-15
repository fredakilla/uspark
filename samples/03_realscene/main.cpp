#include <Urho3D/Urho3DAll.h>
#include <SPARK.h>
#include <SPARK_URHO3D.h>

///---------------------------------------------------------------------------------------------
/// template spark object controller to control independently spk objects in a system.
/// This example move the object around a circle on X,Z plane
///---------------------------------------------------------------------------------------------
template<typename T>
class SpkObjectController : public LogicComponent
{
    URHO3D_OBJECT(SpkObjectController, LogicComponent)

    public:
        SpkObjectController(Context *context) :
      LogicComponent(context)
    {
        elapsedTime_ = 0;
        speed_ = 2.0f;
        radius_ = 2.0f;
    }

    void SetSpkObject(SPK::Ref<T> object)
    {
        spkObject_ = object;
    }

    void SetSpeedAndRadius(float speed, float radius)
    {
        speed_ = speed;
        radius_ = radius;
    }

    void Start()
    {
    }

    void Update(float deltaTime)
    {
        Vector3 pos = node_->GetPosition();
        pos.x_ += sin(elapsedTime_ * speed_) * radius_;
        pos.y_ += 1;
        pos.z_ += cos(elapsedTime_ * speed_) * radius_;

        Matrix3x4 m(pos, Quaternion(25, pos), 1);
        spkObject_->getTransform().setNC(m.Data());

        elapsedTime_ += deltaTime;
    }

private:
    float speed_;               // moving speed
    float radius_;              // circle radius
    float elapsedTime_;         // elasped time
    SPK::Ref<T> spkObject_;     // spark object (need object that inherit from SPK::Transformable)
};

// specialize template controller for emitter and group
typedef SpkObjectController<SPK::Emitter> SpkEmitterController;
typedef SpkObjectController<SPK::Group> SpkGroupController;



///---------------------------------------------------------------------------------------------
/// Custom logic component for moving particles emitters.
///---------------------------------------------------------------------------------------------
class FxMover : public LogicComponent
{
    URHO3D_OBJECT(FxMover, LogicComponent)

public:

        /// Construct.
    FxMover(Context* context) :
      LogicComponent(context),
        moveSpeed_(0.0f),
        rotationSpeed_(0.0f),
        life_(0.0f)
    {
        // Only the scene update event is needed: unsubscribe from the rest for optimization
        SetUpdateEventMask(USE_UPDATE);
    }

    /// Set motion parameters: forward movement speed, rotation speed
    void SetParameters(float moveSpeed, float rotationSpeed)
    {
        moveSpeed_ = moveSpeed;
        rotationSpeed_ = rotationSpeed;
    }

    /// Handle scene update. Called by LogicComponent base class.
    void Update(float timeStep)
    {
        node_->Translate(Vector3::FORWARD * moveSpeed_ * timeStep);
        node_->Roll(rotationSpeed_ * timeStep);

        // kill after max life
        if(life_ > 3.0f)
        {
            node_->Remove();
        }
        life_ += timeStep;
    }

    /// Return forward movement speed.
    float GetMoveSpeed() const { return moveSpeed_; }
    /// Return rotation speed.
    float GetRotationSpeed() const { return rotationSpeed_; }

private:
    /// Forward movement speed.
    float moveSpeed_;
    /// Rotation speed.
    float rotationSpeed_;
    /// Current life time.
    float life_;
};



///---------------------------------------------------------------------------------------------
/// Main application.
///---------------------------------------------------------------------------------------------
class MyApp : public Application
{
public:

    Node*                           _heroNode;
    SharedPtr<Scene>                _scene;
    SharedPtr<Text>                 _textInfo;
    Node*                           _cameraNode;
    SharedPtr<Node>                 _rearCameraNode;
    Node*                           _mushroomNodes[25];
    bool                            _drawDebug;
    int                             _motionDemo;
    unsigned                        _maxEntities;
    bool                            _enableMushrooms;
    SPK::Ref<SPK::System>           _sparkSystem;


    MyApp(Context * context) : Application(context)
    {
        _drawDebug = false;
        _motionDemo = 2;
        _maxEntities = 0;
        _enableMushrooms = false;
    }

    virtual void Setup()
    {
        engineParameters_["FullScreen"]=false;
        engineParameters_["WindowWidth"]=1280;
        engineParameters_["WindowHeight"]=720;
        engineParameters_["WindowResizable"]=true;
        engineParameters_["vsync"]=false;
        engineParameters_["ResourcePaths"] = "Data;CoreData;res";

        context_->RegisterFactory<FxMover>();
        context_->RegisterFactory<SparkParticle>();
        context_->RegisterFactory<SpkEmitterController>();
        context_->RegisterFactory<SpkGroupController>();

        // register urho3d context inside spark lib
        SPK::URHO::Urho3DContext::get().registerUrhoContext(context_);
    }

    void SetupViewports()
    {
        Graphics* graphics = GetSubsystem<Graphics>();
        Renderer* renderer = GetSubsystem<Renderer>();

        renderer->SetNumViewports(2);

        // Set up the front camera viewport
        SharedPtr<Viewport> viewport(new Viewport(context_, _scene, _cameraNode->GetComponent<Camera>()));
        renderer->SetViewport(0, viewport);

        // Clone the default render path so that we do not interfere with the other viewport, then add
        // bloom and FXAA post process effects to the front viewport. Render path commands can be tagged
        // for example with the effect name to allow easy toggling on and off. We start with the effects
        // disabled.
        ResourceCache* cache = GetSubsystem<ResourceCache>();
        SharedPtr<RenderPath> effectRenderPath = viewport->GetRenderPath()->Clone();
        effectRenderPath->Append(cache->GetResource<XMLFile>("PostProcess/Bloom.xml"));
        effectRenderPath->Append(cache->GetResource<XMLFile>("PostProcess/FXAA2.xml"));
        // Make the bloom mixing parameter more pronounced
        effectRenderPath->SetShaderParameter("BloomMix", Vector2(0.9f, 0.6f));
        effectRenderPath->SetEnabled("Bloom", true);
        effectRenderPath->SetEnabled("FXAA2", false);


        // Set up the rear camera viewport on top of the front view ("rear view mirror")
        // The viewport index must be greater in that case, otherwise the view would be left behind
        SharedPtr<Viewport> rearViewport(new Viewport(context_, _scene, _rearCameraNode->GetComponent<Camera>(),
                                                      IntRect(graphics->GetWidth() * 2 / 3, 32, graphics->GetWidth() - 32, graphics->GetHeight() / 3)));
        renderer->SetViewport(1, rearViewport);

        rearViewport->SetRenderPath(effectRenderPath);
    }

    void CreateSparkParticles()
    {
        ResourceCache* cache = GetSubsystem<ResourceCache>();
        Material * mat = cache->GetResource<Material>("Materials/Particle.xml");

        // Spark system
        _sparkSystem = SPK::System::create(true);
        _sparkSystem->setName("SparkSystem");

        // 1st Renderer
        SPK::Ref<SPK::URHO::Urho3DQuadRenderer> quadRenderer = SPK::URHO::Urho3DQuadRenderer::create(context_);
        quadRenderer->setBlendMode(SPK::BLEND_MODE_ADD);
        quadRenderer->enableRenderingOption(SPK::RENDERING_OPTION_DEPTH_WRITE,false);
        //quadRenderer->setTexture(driver->getTexture("res\\flare.bmp"));
        quadRenderer->setTexturingMode(SPK::TEXTURE_MODE_2D);
        quadRenderer->setScale(3.0f, 3.0f);
        quadRenderer->setMaterial(mat);

        // 1st emitter
        SPK::Ref<SPK::Emitter> emitter1 = SPK::RandomEmitter::create(SPK::Point::create());
        emitter1->setForce(0.4f,0.6f);
        emitter1->setFlow(200);

        // 2nd emitter
        SPK::Ref<SPK::Emitter> emitter2 = SPK::StraightEmitter::create(SPK::Vector3D(0,1,0), SPK::Point::create());
        emitter2->setFlow(250);
        emitter2->setForce(2,4);
        emitter2->setZone(SPK::Sphere::create());

        // Color graph interpolator
        SPK::Ref<SPK::ColorGraphInterpolator> graphInterpolator = SPK::ColorGraphInterpolator::create();
        graphInterpolator->addEntry(0.0f,0xFFFF00FF);
        graphInterpolator->addEntry(0.5f,0xFF00FFFF);
        graphInterpolator->addEntry(1.0f,0xFF0000FF);

        // 1st group
        SPK::Ref<SPK::Group> group = _sparkSystem->createGroup(30000);
        group->setRadius(0.15f);
        group->setLifeTime(0.5f,1.0f);
        group->setColorInterpolator(graphInterpolator);
        group->setParamInterpolator(SPK::PARAM_SCALE,SPK::FloatRandomInterpolator::create(0.8f,1.2f,0.0f,0.0f));
        group->setParamInterpolator(SPK::PARAM_ANGLE,SPK::FloatRandomInitializer::create(0.0f,2 * M_PI));
        group->addEmitter(emitter1);
        group->addEmitter(emitter2);
        group->addModifier(SPK::Gravity::create(SPK::Vector3D(0.0f,-0.5f,0.0f)));
        group->addModifier(SPK::Friction::create(0.2f));
        group->setRenderer(quadRenderer);




        // Create a new material from scratch
        /*Material* mat2 = new Material(context_);
        mat2->SetNumTechniques(1);
        Technique* tec = new Technique(context_);
        Pass* pass = tec->CreatePass("alpha");
        pass->SetDepthWrite(false);
        pass->SetBlendMode(BLEND_ADDALPHA);
        pass->SetVertexShader("UnlitParticle");
        pass->SetPixelShader("UnlitParticle");
        pass->SetVertexShaderDefines("VERTEXCOLOR");
        pass->SetPixelShaderDefines("DIFFMAP VERTEXCOLOR");
        mat2->SetTechnique(0, tec);
        mat2->SetTexture(TU_DIFFUSE, cache->GetResource<Texture2D>("Textures/UrhoIcon.png"));*/


        // 2nd Renderer
        SPK::Ref<SPK::URHO::Urho3DQuadRenderer> quadRenderer2 = SPK::URHO::Urho3DQuadRenderer::create(context_);
        quadRenderer2->setBlendMode(SPK::BLEND_MODE_ADD);
        quadRenderer2->enableRenderingOption(SPK::RENDERING_OPTION_DEPTH_WRITE,false);
        //quadRenderer->setTexture(driver->getTexture("res\\flare.bmp"));
        quadRenderer2->setTexturingMode(SPK::TEXTURE_MODE_2D);
        quadRenderer2->setScale(0.5f, 0.5f);
        quadRenderer2->setMaterial(mat);

        // another color graph interpolator
        SPK::Ref<SPK::ColorGraphInterpolator> graphInterpolator2 = SPK::ColorGraphInterpolator::create();
        graphInterpolator2->addEntry(0.0f,0x88FF0000);
        graphInterpolator2->addEntry(0.5f,0x8800FF00);
        graphInterpolator2->addEntry(1.0f,0x880000FF);

        // 2nd group
        SPK::Ref<SPK::Group> group2 = _sparkSystem->createGroup(3000);
        group2->setRadius(0.5f);
        group2->setLifeTime(0.5f,1.0f);
        group2->setColorInterpolator(graphInterpolator2);
        //group2->setParamInterpolator(SPK::PARAM_SCALE,SPK::FloatRandomInterpolator::create(0.8f,1.2f,0.0f,0.0f));
        //group2->setParamInterpolator(SPK::PARAM_ANGLE,SPK::FloatRandomInitializer::create(0.0f,2 * M_PI));
        group2->addEmitter(emitter1);
        group2->addEmitter(emitter2);
        group2->addModifier(SPK::Gravity::create(SPK::Vector3D(0.0f,-13.5f,0.0f)));
        group2->addModifier(SPK::Friction::create(0.2f));
        group2->setRenderer(quadRenderer2);



        //SPK::URHO::Urho3DContext::get();
        //SPK::URHO::Urho3DContext::get().registerUrhoContext(context_);

        // Spark IO test
        /// -------------------------------------------------------------------
        SPK::IO::IOManager::get().save("test.xml", _sparkSystem);
        SPK::IO::IOManager::get().save("test.spk", _sparkSystem);
        /// -------------------------------------------------------------------

        SPK_DUMP_MEMORY

        SPK::Ref<SPK::System> loadedSystem = SPK::IO::IOManager::get().load("test.xml");
        if(loadedSystem)
        {
            printf("Renderer set\n");
            //loadedSystem->getGroup(0)->setRenderer(quadRenderer);
            //loadedSystem->getGroup(1)->setRenderer(quadRenderer2);
        }

        // use loaded system
        _sparkSystem = loadedSystem;
        /// -------------------------------------------------------------------
    }

    virtual void Start()
    {
        // Create Spark particle system
        CreateSparkParticles();

        ResourceCache* cache = GetSubsystem<ResourceCache>();

        _scene = new Scene(context_);
        _scene->CreateComponent<Octree>();
        _scene->CreateComponent<DebugRenderer>();

        // create a copy of the spark system
        SPK::Ref<SPK::System> systemCopy = SPK::SPKObject::copy(_sparkSystem);

        // create a spark particle node and component
        Node* spkSystemNode = _scene->CreateChild("SparkSystem");
        spkSystemNode->SetPosition(Vector3(3.0f, 2.0f, 30.0f));
        SparkParticle* spkSystem = spkSystemNode->CreateComponent<SparkParticle>();
        spkSystem->SetSystem(systemCopy);

        // create an emitter controller to control emitter 0 from group 0
        SpkEmitterController* emitterController = spkSystemNode->CreateComponent<SpkEmitterController>();
        emitterController->SetSpkObject(systemCopy->getGroup(0)->getEmitter(0));
        emitterController->SetSpeedAndRadius(10.0f, 2.0f);

        unsigned gridX = 0;
        unsigned gridY = 0;
        float offset = 8.0f;
        _maxEntities = 16;

        // create multiple entities (each entity has a mushroom static mesh + a particle emitter as components)
        for (unsigned i=0; i<_maxEntities; ++i)
        {
            float XPos = offset * gridX - 12;
            float YPos = offset * gridY - 12;

            // create a node
            _mushroomNodes[i] = _scene->CreateChild("Mushroom");
            _mushroomNodes[i]->SetPosition(Vector3(XPos, 2.0f, YPos));
            _mushroomNodes[i]->SetRotation(Quaternion(0.0f, Random() * 360.0f, 0.0f));
            _mushroomNodes[i]->SetScale(1.0f);
            _mushroomNodes[i]->SetEnabled(_enableMushrooms);

            // add a static mesh component
            StaticModel* mushroomObject = _mushroomNodes[i]->CreateComponent<StaticModel>();
            mushroomObject->SetModel(cache->GetResource<Model>("Models/Mushroom.mdl"));
            mushroomObject->SetMaterial(cache->GetResource<Material>("Materials/Mushroom.xml"));
            mushroomObject->SetCastShadows(true);

            // add urho3D particle component
            //ParticleEmitter* emitter = _mushroomNodes[i]->CreateComponent<ParticleEmitter>();
            //emitter->SetEffect(cache->GetResource<ParticleEffect>("Particle/Fire.xml"));

            // add a spark particle component
            SPK::Ref<SPK::System> systemCopy = SPK::SPKObject::copy(_sparkSystem);
            SparkParticle* spkSystem = _mushroomNodes[i]->CreateComponent<SparkParticle>();
            spkSystem->SetSystem(systemCopy);


            gridX++;
            if(gridX >= _maxEntities/4)
            {
                gridY++;
                gridX = 0;
            }
        }



        // Create a plane with a "stone" material.
        Node* planeNode = _scene->CreateChild("Plane");
        planeNode->SetScale(Vector3(_maxEntities * offset, 1.0f, _maxEntities * offset));
        StaticModel* planeObject = planeNode->CreateComponent<StaticModel>();
        planeObject->SetModel(cache->GetResource<Model>("Models/Plane.mdl"));
        planeObject->SetMaterial(cache->GetResource<Material>("Materials/StoneTiled.xml"));

        // Create the hero model
        _heroNode = _scene->CreateChild("Hero");
        _heroNode->SetDirection(Vector3::RIGHT);
        StaticModel* heroModel = _heroNode->CreateComponent<StaticModel>();
        heroModel->SetModel(cache->GetResource<Model>("Models/Kachujin/Kachujin.mdl"));
        heroModel->SetMaterial(cache->GetResource<Material>("Models/Kachujin/Materials/Kachujin.xml"));
        heroModel->SetCastShadows(true);

        // Create a text for stats.
        _textInfo = new Text(context_);
        _textInfo->SetText("");
        _textInfo->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 12);
        _textInfo->SetColor(Color(1.0f, 1.0f, 1.0f));
        _textInfo->SetHorizontalAlignment(HA_LEFT);
        _textInfo->SetVerticalAlignment(VA_TOP);
        GetSubsystem<UI>()->GetRoot()->AddChild(_textInfo);

        // Create debug HUD.
        DebugHud* debugHud = engine_->CreateDebugHud();
        XMLFile* xmlFile = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");
        debugHud->SetDefaultStyle(xmlFile);

        // Create a Zone component for ambient lighting & fog control
        Node* zoneNode = _scene->CreateChild("Zone");
        Zone* zone = zoneNode->CreateComponent<Zone>();
        zone->SetBoundingBox(BoundingBox(-1000.0f, 1000.0f));
        zone->SetAmbientColor(Color(0.1f, 0.1f, 0.1f));
        zone->SetFogColor(Color(0.2f, 0.2f, 0.2f));
        zone->SetFogStart(100.0f);
        zone->SetFogEnd(300.0f);

        // Create the cameras. Limit far clip distance to match the fog
        _cameraNode = _scene->CreateChild("Camera");
        _cameraNode->SetPosition(Vector3(0.0f, 5.0f, -20.0f));
        Camera* camera = _cameraNode->CreateComponent<Camera>();
        camera->SetFarClip(300.0f);

        // Parent the rear camera node to the front camera node and turn it 180 degrees to face backward
        // Here, we use the angle-axis constructor for Quaternion instead of the usual Euler angles
        _rearCameraNode = _cameraNode->CreateChild("RearCamera");
        _rearCameraNode->Rotate(Quaternion(180.0f, Vector3::UP));
        Camera* rearCamera = _rearCameraNode->CreateComponent<Camera>();
        rearCamera->SetFarClip(300.0f);
        // Because the rear viewport is rather small, disable occlusion culling from it. Use the camera's
        // "view override flags" for this. We could also disable eg. shadows or force low material quality
        // if we wanted
        rearCamera->SetViewOverrideFlags(VO_DISABLE_OCCLUSION);

        // Create a directional light to the world. Enable cascaded shadows on it
        Node* lightNode = _scene->CreateChild("DirectionalLight");
        lightNode->SetDirection(Vector3(0.6f, -1.0f, 0.8f));
        Light* light = lightNode->CreateComponent<Light>();
        light->SetLightType(LIGHT_DIRECTIONAL);
        light->SetCastShadows(true);
        light->SetColor(Color(0.5f, 0.5f, 0.5f));
        light->SetShadowBias(BiasParameters(0.00025f, 0.5f));
        light->SetShadowCascade(CascadeParameters(10.0f, 50.0f, 200.0f, 0.0f, 0.8f));

        // Create viewport.
        SetupViewports();

        SubscribeToEvent(E_KEYDOWN,URHO3D_HANDLER(MyApp,HandleKeyDown));
        SubscribeToEvent(E_MOUSEBUTTONDOWN,URHO3D_HANDLER(MyApp,HandleMouseButtonDown));
        SubscribeToEvent(E_UPDATE,URHO3D_HANDLER(MyApp,HandleUpdate));
        SubscribeToEvent(E_POSTRENDERUPDATE, URHO3D_HANDLER(MyApp, HandlePostRenderUpdate));

        // Only for debugging (else cursor is not visible on breakpoints)
        GetSubsystem<Input>()->SetMouseVisible(true);
    }

    void AnimateScene(float timeStep)
    {
        if(_enableMushrooms)
        {
            const float moveSpeed = 7.0f;
            const float rotationSpeed = 100.0f;
            for (unsigned i=0; i<_maxEntities; ++i)
            {
                switch(_motionDemo)
                {
                case 0:
                    // translate around a circle
                    _mushroomNodes[i]->Translate(Vector3::FORWARD * moveSpeed * timeStep);
                    _mushroomNodes[i]->Yaw(rotationSpeed * timeStep);
                    break;

                case 1:
                    // rotate inplace
                    _mushroomNodes[i]->Roll(rotationSpeed * timeStep);
                    _mushroomNodes[i]->Yaw(rotationSpeed * timeStep * 1);
                    _mushroomNodes[i]->Pitch(rotationSpeed * timeStep * 3);
                    break;

                case 2:
                    // reset rotation
                    _mushroomNodes[i]->SetRotation(Quaternion(0.0f, 0.0f, 0.0f));
                    break;
                }
            }
        }

        static float accumulator = 0.0f;
        accumulator += timeStep;
        if(accumulator >= 1.0f)
        {
            String s;
            accumulator = 0.0f;

            // count particles in scene
            unsigned totalParticlesCount = 0;
            unsigned totalSparkSystem = 0;
            PODVector<SparkParticle*> dest;
            _scene->GetComponents<SparkParticle>(dest, true);
            for(unsigned i=0; i<dest.Size(); ++i)
            {
                if(dest[i]->IsInView())
                {
                    totalParticlesCount += dest[i]->GetSystem()->getNbParticles();
                    totalSparkSystem++;
                }
            }

            // display infos
            s += "FPS: " + String(1.0f / timeStep) + "\n";
            s += "visible spark particle system: " + String(totalSparkSystem) + "\n";
            s += "visible Particles count = " + String(totalParticlesCount);
            s += "\nPress 'B' to show bounding boxes";
            s += "\nPress 'E' to show/hide all emitters";
            s += "\nPress 'T' to change motion type";
            s += "\nPress 'Mouse left button' to spawn emitter from hero";
            s += "\nPress 'Right/Left' to turn hero";

            _textInfo->SetText(s);
        }

    }

    virtual void Stop()
    {
    }

    void HandleKeyDown(StringHash eventType,VariantMap& eventData)
    {
        using namespace KeyDown;
        int key=eventData[P_KEY].GetInt();

        if(key == KEY_ESCAPE)
            engine_->Exit();
        else if (key == KEY_F2)
            GetSubsystem<DebugHud>()->ToggleAll();
        else if(key == KEY_B)
        {
            _drawDebug = !_drawDebug;
        }
        else if(key == KEY_T)
        {
            _motionDemo++;
            if(_motionDemo >= 3)
                _motionDemo = 0;
        }
        else if(key == KEY_E)
        {
            _enableMushrooms = !_enableMushrooms;

            for (unsigned i=0; i<_maxEntities; ++i)
            {
                _mushroomNodes[i]->SetEnabled(_enableMushrooms);
            }
        }
    }

    void HandleMouseButtonDown(StringHash eventType,VariantMap& eventData)
    {
        using namespace MouseButtonDown;
        int button = eventData[P_BUTTON].GetInt();

        if(button == MOUSEB_LEFT)
        {
            // create projectile node from hero pos and direction
            Node* projectile = _scene->CreateChild("Projectile");
            projectile->SetPosition(_heroNode->GetPosition() + Vector3(0,2,0));
            projectile->SetDirection(_heroNode->GetDirection());

            // add particle to projectile
            //ResourceCache* cache = GetSubsystem<ResourceCache>();
            //ParticleEmitter* emitter = projectile->CreateComponent<ParticleEmitter>();
            //emitter->SetEffect(cache->GetResource<ParticleEffect>("Particle/Fire.xml"));

            // add spark particle to projectile
            SparkParticle* spksystem3 = projectile->CreateComponent<SparkParticle>();
            spksystem3->SetSystem(SPK::SPKObject::copy(_sparkSystem));

            // add mover logic component
            FxMover* mover = projectile->CreateComponent<FxMover>();
            mover->SetParameters(18, 0);
        }
    }

    void HandleUpdate(StringHash eventType,VariantMap& eventData)
    {
        using namespace Update;
        float timeStep = eventData[P_TIMESTEP].GetFloat();
        MoveCamera(timeStep);
        AnimateScene(timeStep);
    }

    void MoveCamera(float timeStep)
    {
        Input* input = GetSubsystem<Input>();

        const float MOVE_SPEED = 20.0f;
        const float MOUSE_SENSITIVITY = 0.1f;

        static float yaw_ = 0;
        static float pitch_ = 0;

        IntVector2 mouseMove = input->GetMouseMove();
        yaw_ += MOUSE_SENSITIVITY * mouseMove.x_;
        pitch_ += MOUSE_SENSITIVITY * mouseMove.y_;
        pitch_ = Clamp(pitch_, -90.0f, 90.0f);

        _cameraNode->SetRotation(Quaternion(pitch_, yaw_, 0.0f));

        if (input->GetKeyDown(KEY_W))
            _cameraNode->Translate(Vector3::FORWARD * MOVE_SPEED * timeStep);
        if (input->GetKeyDown(KEY_S))
            _cameraNode->Translate(Vector3::BACK * MOVE_SPEED * timeStep);
        if (input->GetKeyDown(KEY_A))
            _cameraNode->Translate(Vector3::LEFT * MOVE_SPEED * timeStep);
        if (input->GetKeyDown(KEY_D))
            _cameraNode->Translate(Vector3::RIGHT * MOVE_SPEED * timeStep);

        // Rotate hero direction
        const float heroRotationSpeed = 250 * timeStep;
        if(input->GetKeyDown(KEY_RIGHT))
        {
            _heroNode->Rotate(Quaternion(heroRotationSpeed, Vector3(0,1,0)));
        }
        if(input->GetKeyDown(KEY_LEFT))
        {
            _heroNode->Rotate(Quaternion(-heroRotationSpeed, Vector3(0,1,0)));
        }
    }

    void HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData)
    {
        if (_drawDebug)
            GetSubsystem<Renderer>()->DrawDebugGeometry(true);
    }
};


URHO3D_DEFINE_APPLICATION_MAIN(MyApp)
