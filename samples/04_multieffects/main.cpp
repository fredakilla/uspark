#include <Urho3D/Urho3DAll.h>

#include <SPARK.h>
#include <SPARK_URHO3D.h>




/// Main application.
class MyApp : public Application
{
public:

    SharedPtr<Scene>    _scene;
    SharedPtr<Text>     _textInfo;
    Node*               _cameraNode;
    bool                _drawDebug;
    int                 _xalign;

    float               _angleX;
    float               _angleZ;


    MyApp(Context * context) : Application(context)
    {
        context_->RegisterFactory<UrhoSparkSystem>();

        _drawDebug = false;
        _xalign = 0;
        _angleX = _angleZ = 0;
    }

    void Setup()
    {
        engineParameters_["FullScreen"]=false;
        engineParameters_["WindowWidth"]=1280;
        engineParameters_["WindowHeight"]=720;
        engineParameters_["WindowResizable"]=true;
        engineParameters_["vsync"]=false;
    }

    void Start()
    {
        ResourceCache* cache = GetSubsystem<ResourceCache>();

        _scene = new Scene(context_);
        _scene->CreateComponent<Octree>();
        _scene->CreateComponent<DebugRenderer>();

        // create spark particles
        CreateFountainParticles();
        CreateEffectCollision();
        CreateFlakesEffect();




        // Create a plane with a "stone" material.
        Node* planeNode = _scene->CreateChild("Plane");
        planeNode->SetScale(Vector3(10, 1.0f, 10));
        StaticModel* planeObject = planeNode->CreateComponent<StaticModel>();
        planeObject->SetModel(cache->GetResource<Model>("Models/Plane.mdl"));
        planeObject->SetMaterial(cache->GetResource<Material>("Materials/StoneTiled.xml"));

        // Create a text for stats.
        _textInfo = new Text(context_);
        _textInfo->SetText("");
        _textInfo->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 12);
        _textInfo->SetColor(Color(1.0f, 1.0f, 1.0f));
        _textInfo->SetHorizontalAlignment(HA_RIGHT);
        _textInfo->SetVerticalAlignment(VA_BOTTOM);
        GetSubsystem<UI>()->GetRoot()->AddChild(_textInfo);

        // Create debug HUD.
        DebugHud* debugHud = engine_->CreateDebugHud();
        XMLFile* xmlFile = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");
        debugHud->SetDefaultStyle(xmlFile);

        // Create camera.
        _cameraNode = _scene->CreateChild("Camera");
        Camera* camera = _cameraNode->CreateComponent<Camera>();
        _cameraNode->SetPosition(Vector3(0.0f, 1.0f, -10.0f));

        // Create viewport.
        Renderer* renderer=GetSubsystem<Renderer>();
        SharedPtr<Viewport> viewport(new Viewport(context_, _scene, camera));
        renderer->SetViewport(0,viewport);

        SubscribeToEvent(E_KEYDOWN,URHO3D_HANDLER(MyApp,HandleKeyDown));
        SubscribeToEvent(E_UPDATE,URHO3D_HANDLER(MyApp,HandleUpdate));
        SubscribeToEvent(E_POSTRENDERUPDATE, URHO3D_HANDLER(MyApp, HandlePostRenderUpdate));

        GetSubsystem<Input>()->SetMouseVisible(true);
    }

    void CreateFountainParticles()
    {
        ResourceCache* cache = GetSubsystem<ResourceCache>();

        // Create a new material from scratch
        Material* mat = new Material(context_);
        mat->SetNumTechniques(1);
        Technique* tec = new Technique(context_);
        Pass* pass = tec->CreatePass("alpha");
        pass->SetDepthWrite(false);
        pass->SetBlendMode(BLEND_ADDALPHA);
        pass->SetVertexShader("UnlitParticle");
        pass->SetPixelShader("UnlitParticle");
        pass->SetVertexShaderDefines("VERTEXCOLOR");
        pass->SetPixelShaderDefines("DIFFMAP VERTEXCOLOR");
        mat->SetTechnique(0, tec);
        mat->SetTexture(TU_DIFFUSE, cache->GetResource<Texture2D>("res/flare.png"));


        SPK::Ref<SPK::System> system_ = SPK::System::create(true);
        system_->setName("Test System");

        // Renderer
        SPK::Ref<SPK::URHO::IUrho3DQuadRenderer> renderer = SPK::URHO::IUrho3DQuadRenderer::create(context_);
        renderer->setTexturingMode(SPK::TEXTURE_MODE_2D);
        //renderer->setTexture(textureParticle);
        renderer->setBlendMode(SPK::BLEND_MODE_ADD);
        renderer->enableRenderingOption(SPK::RENDERING_OPTION_DEPTH_WRITE,false);
        renderer->setScale(0.08f,0.08f);
        renderer->setMaterial(mat);
        renderer->setOrientation(SPK::OrientationPreset::DIRECTION_ALIGNED);

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
        particleGroup->setLifeTime(4.0f,4.0f);
        particleGroup->setColorInterpolator(SPK::ColorSimpleInterpolator::create(0x88FFFF00,0x880000FF));

        SPK::Ref<SPK::Sphere> sphere = SPK::Sphere::create(SPK::Vector3D(0,2,1), 2);
        particleGroup->addModifier(SPK::Obstacle::create(sphere,0.8f,0.9f,SPK::ZONE_TEST_INTERSECT));





        Node* spkSystemNode = _scene->CreateChild("SparkEffect");
        UrhoSparkSystem* spkSystem = spkSystemNode->CreateComponent<UrhoSparkSystem>();
        spkSystem->SetSystem(system_);
        spkSystemNode->SetPosition(Vector3(-3.0f, 0.0f, 0.0f));

    }




    SPK::Ref<SPK::Gravity> gravityCollision;

    void CreateEffectCollision()
    {
        ResourceCache* cache = GetSubsystem<ResourceCache>();

        // Create a new material from scratch
        Material* mat = new Material(context_);
        mat->SetNumTechniques(1);
        Technique* tec = new Technique(context_);
        Pass* pass = tec->CreatePass("alpha");
        pass->SetDepthWrite(false);
        pass->SetAlphaToCoverage(true);
        pass->SetBlendMode(BLEND_ALPHA);
        pass->SetVertexShader("UnlitParticle");
        pass->SetPixelShader("UnlitParticle");
        pass->SetVertexShaderDefines("VERTEXCOLOR");
        pass->SetPixelShaderDefines("DIFFMAP VERTEXCOLOR");
        mat->SetTechnique(0, tec);
        mat->SetTexture(TU_DIFFUSE, cache->GetResource<Texture2D>("res/flare.png"));




        // Renderer
        SPK::Ref<SPK::URHO::IUrho3DQuadRenderer> particleRenderer = SPK::URHO::IUrho3DQuadRenderer::create(context_);
        particleRenderer->setTexturingMode(SPK::TEXTURE_MODE_2D);
        //particleRenderer->setTexture(textureParticle);
        particleRenderer->setBlendMode(SPK::BLEND_MODE_ADD);
        particleRenderer->enableRenderingOption(SPK::RENDERING_OPTION_DEPTH_WRITE,false);
        //particleRenderer->setScale(0.05f,0.05f);
        particleRenderer->setMaterial(mat);
        particleRenderer->setOrientation(SPK::OrientationPreset::CAMERA_PLANE_ALIGNED);
        particleRenderer->setBlendMode(SPK::BLEND_MODE_NONE);
        particleRenderer->enableRenderingOption(SPK::RENDERING_OPTION_ALPHA_TEST,true);
        particleRenderer->setAlphaTestThreshold(0.8f);



        float RADIUS = 0.06;
        unsigned NB_PARTICLES = 750;


        // Zone
        SPK::Ref<SPK::Sphere> sphere = SPK::Sphere::create();
        SPK::Ref<SPK::Box> cube = SPK::Box::create(SPK::Vector3D(),SPK::Vector3D(1.4f,1.4f,1.4f));

        // Gravity
        gravityCollision = SPK::Gravity::create(SPK::Vector3D(0,-1,0));

        // System
        SPK::Ref<SPK::System> particleSystem = SPK::System::create(true);

        SPK::Ref<SPK::Collider> collider = SPK::Collider::create(0.8f);

        // Obstacle
        SPK::Ref<SPK::Obstacle> obstacle = SPK::Obstacle::create(sphere,0.8f,0.9f,SPK::ZONE_TEST_INTERSECT);

        // Group
        SPK::Ref<SPK::Group> particleGroup = particleSystem->createGroup(NB_PARTICLES);
        particleGroup->setImmortal(true);
        particleGroup->setRadius(RADIUS);
        particleGroup->setRenderer(particleRenderer);
        particleGroup->addModifier(gravityCollision);
        particleGroup->addModifier(obstacle);
        particleGroup->addModifier(collider);
        particleGroup->addModifier(SPK::Friction::create(0.2f));


        particleGroup->addParticles(NB_PARTICLES,obstacle->getZone(),SPK::Vector3D());



        Node* spkSystemNode = _scene->CreateChild("SparkEffect");
        UrhoSparkSystem* spkSystem = spkSystemNode->CreateComponent<UrhoSparkSystem>();
        spkSystem->SetSystem(particleSystem);
        spkSystemNode->SetPosition(Vector3(0.0f, 1.0f, 0.0f));


    }


    SPK::Ref<SPK::Gravity> gravityFlakes;


    void CreateFlakesEffect()
    {
        ResourceCache* cache = GetSubsystem<ResourceCache>();

        // Create a new material from scratch
        Material* mat = new Material(context_);
        mat->SetNumTechniques(1);
        Technique* tec = new Technique(context_);
        Pass* pass = tec->CreatePass("alpha");
        pass->SetDepthWrite(false);
        pass->SetAlphaToCoverage(true);
        pass->SetBlendMode(BLEND_ALPHA);
        pass->SetVertexShader("UnlitParticle");
        pass->SetPixelShader("UnlitParticle");
        pass->SetVertexShaderDefines("VERTEXCOLOR");
        pass->SetPixelShaderDefines("DIFFMAP VERTEXCOLOR");
        mat->SetTechnique(0, tec);
        mat->SetTexture(TU_DIFFUSE, cache->GetResource<Texture2D>("res/flare.png"));




        // Renderer
        SPK::Ref<SPK::URHO::IUrho3DQuadRenderer> particleRenderer = SPK::URHO::IUrho3DQuadRenderer::create(context_);
        particleRenderer->setTexturingMode(SPK::TEXTURE_MODE_2D);
        //particleRenderer->setTexture(textureParticle);
        particleRenderer->setBlendMode(SPK::BLEND_MODE_ADD);
        particleRenderer->enableRenderingOption(SPK::RENDERING_OPTION_DEPTH_WRITE,false);
        //particleRenderer->setScale(0.05f,0.05f);
        particleRenderer->setMaterial(mat);
        particleRenderer->setOrientation(SPK::OrientationPreset::CAMERA_PLANE_ALIGNED);
        particleRenderer->setBlendMode(SPK::BLEND_MODE_NONE);
        particleRenderer->enableRenderingOption(SPK::RENDERING_OPTION_ALPHA_TEST,true);


        const unsigned int NB_PARTICLES_SIZE = 6;
        const unsigned int NB_PARTICLES[NB_PARTICLES_SIZE] =
        {
            10000,
            25000,
            50000,
            100000,
            200000,
            500000,
        };
        float RADIUS = 0.03;
        unsigned INDEX = 1;


        // Zone
        SPK::Ref<SPK::Sphere> sphere = SPK::Sphere::create(SPK::Vector3D(3,1,0),1.0f);

        // Gravity
        gravityFlakes = SPK::Gravity::create(SPK::Vector3D(0.0f,0.0f,0.0f));

        // System
        SPK::Ref<SPK::System> particleSystem = SPK::System::create(true);

        // Group
        SPK::Ref<SPK::Group> particleGroup = particleSystem->createGroup(NB_PARTICLES[NB_PARTICLES_SIZE - 1]);
        particleGroup->setRadius(RADIUS);
        particleGroup->setRenderer(particleRenderer);
        particleGroup->setColorInterpolator(SPK::ColorDefaultInitializer::create(0xFFCC4C66));
        particleGroup->addModifier(gravityFlakes);
        particleGroup->addModifier(SPK::Friction::create(0.2f));
        particleGroup->addModifier(SPK::Obstacle::create(sphere,0.9f,0.9f));
        particleGroup->setImmortal(true);

        // Particles are added to the group
        particleGroup->addParticles(NB_PARTICLES[INDEX],sphere,SPK::Vector3D());
        particleGroup->flushBufferedParticles();


        for(unsigned i=0; i<particleGroup->getNbModifiers(); ++i)
            particleGroup->getModifier(i)->setLocalToSystem(false);


        Node* spkSystemNode = _scene->CreateChild("SparkEffect");
        UrhoSparkSystem* spkSystem = spkSystemNode->CreateComponent<UrhoSparkSystem>();
        spkSystem->SetSystem(particleSystem);

    }




    void AnimateScene(float timeStep)
    {
        static float accumulator = 0.0f;
        accumulator += timeStep;



        _angleX += accumulator;
        _angleZ += accumulator;

        float cosX = cos(_angleX * M_PI / 180.0f);
        float sinX = sin(_angleX * M_PI / 180.0f);
        float cosZ = cos(_angleZ * M_PI / 180.0f);
        float sinZ = sin(_angleZ * M_PI / 180.0f);


        gravityFlakes->setValue(SPK::Vector3D(-10.5f * sinZ * cosX, -0.5 * cosZ * cosX, -10.5f * sinX));

        gravityCollision->setValue(SPK::Vector3D(10.5f * sinZ * cosX,-10.5f * cosZ * cosX,-10.5f * sinX));




        if(accumulator >= 1.0f)
        {
            String s;

            // display infos
            s= "FPS = ";
            s += String(1 / timeStep) + "\n";
            //s += "Particles c_systemEffectount = " + String(_systemCopy->getNbParticles()) + "\n";
            _textInfo->SetText(s);
            accumulator = 0.0f;
        }
    }

    void Stop()
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

    }

    void HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData)
    {
        if (_drawDebug)
            GetSubsystem<Renderer>()->DrawDebugGeometry(true);





        DebugRenderer * dbgRenderer = _scene->GetComponent<DebugRenderer>();
        if (dbgRenderer)
        {

            // dbgRenderer->AddSphere(Sphere(Vector3(SPK::URHO::spk2urho(obstacle->getTransform().getWorldPos())), sphere->getRadius()), Color::RED, true);
            //dbgRenderer->AddQuad(SPK::URHO::spk2urho(myGroup->getTransform().getWorldPos()), 10, 10, Color::RED, true);

        }






    }
};


URHO3D_DEFINE_APPLICATION_MAIN(MyApp)
