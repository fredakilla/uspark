#include <Urho3D/Urho3DAll.h>

#include <SPARK.h>
#include <SPARK_URHO3D.h>


/// Main application.
class MyApp : public Application
{
public:

    SharedPtr<Scene>                _scene;
    SharedPtr<Text>                 _textInfo;
    Node*                           _cameraNode;
    bool                            _drawDebug;
    SPK::Ref<SPK::System>           _systemCopy;


    MyApp(Context * context) : Application(context)
    {
        UrhoSparkSystem::RegisterObject(context);
        context_->RegisterFactory<UrhoSparkSystem>();

        _drawDebug = false;
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
        CreateSheetTest();
        CreateFire();


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

    void CreateSheetTest()
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
        mat->SetTexture(TU_DIFFUSE, cache->GetResource<Texture2D>("res/numbers.bmp"));


        SPK::Ref<SPK::System> system_ = SPK::System::create(true);
        system_->setName("Test System");

        // Renderer
        SPK::Ref<SPK::URHO::IUrho3DQuadRenderer> renderer = SPK::URHO::IUrho3DQuadRenderer::create(context_);
        renderer->setTexturingMode(SPK::TEXTURE_MODE_2D);
        //renderer->setTexture(textureParticle);
        renderer->setBlendMode(SPK::BLEND_MODE_ADD);
        renderer->enableRenderingOption(SPK::RENDERING_OPTION_DEPTH_WRITE,false);
        renderer->setScale(0.5f,0.5f);
        renderer->setMaterial(mat);
        renderer->setOrientation(SPK::OrientationPreset::CAMERA_PLANE_ALIGNED);
        renderer->setAtlasDimensions(2,2);

        // Emitter 1
        SPK::Ref<SPK::StraightEmitter> particleEmitter = SPK::StraightEmitter::create(SPK::Vector3D(0.0f,2.0f,0.0f));
        particleEmitter->setZone(SPK::Point::create(SPK::Vector3D(0.0f,1.0f,0.0f)));
        particleEmitter->setFlow(1);
        particleEmitter->setForce(1.5f,1.5f);

        // Group 1
        SPK::Ref<SPK::Group> particleGroup = system_->createGroup(10);
        particleGroup->addEmitter(particleEmitter);
        particleGroup->setRenderer(renderer);
        particleGroup->setLifeTime(0.5, 0.5);
        particleGroup->setColorInterpolator(SPK::ColorSimpleInterpolator::create(0xFF0000FF,0x00FF00FF));
        particleGroup->setParamInterpolator(SPK::PARAM_TEXTURE_INDEX, SPK::FloatSimpleInterpolator::create(0.0f,4.0f));

        // Emitter 2
        SPK::Ref<SPK::SphericEmitter> emitter2 = SPK::SphericEmitter::create(SPK::Vector3D(-1.0f,0.0f,0.0f),0.1f * M_PI, 0.1f * M_PI);
        emitter2->setZone(SPK::Point::create(SPK::Vector3D(0.0f,1.0f,0.0f)));
        emitter2->setFlow(50);
        emitter2->setForce(1.5f,1.5f);

        // Group 2
        SPK::Ref<SPK::Group> group2 = system_->createGroup(40000);
        group2->setRadius(0.2);
        group2->addEmitter(emitter2);
        group2->setRenderer(renderer);
        group2->setLifeTime(3.0f, 3.0f);
        group2->setColorInterpolator(SPK::ColorSimpleInterpolator::create(0xFF0000FF,0x00FF00FF));
        group2->setParamInterpolator(SPK::PARAM_TEXTURE_INDEX, SPK::FloatSimpleInterpolator::create(0.0f,4.0f));
        group2->setParamInterpolator(SPK::PARAM_ANGLE, SPK::FloatSimpleInterpolator::create(0.0f, 1.0 * M_PI * 2));

        _systemCopy = SPK::SPKObject::copy(system_);

        // create spark particle component
        Node* spkSystemNode = _scene->CreateChild("SparkSystem");
        UrhoSparkSystem* spkSystem = spkSystemNode->CreateComponent<UrhoSparkSystem>();
        spkSystem->SetSystem(_systemCopy);
        spkSystemNode->SetPosition(Vector3(-2.0f, 0.0f, 0.0f));
    }


    void CreateFire()
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
        mat->SetTexture(TU_DIFFUSE, cache->GetResource<Texture2D>("res/sprite-flame.jpg"));


        SPK::Ref<SPK::System> system_ = SPK::System::create(true);
        system_->setName("Test System");

        // Renderer
        SPK::Ref<SPK::URHO::IUrho3DQuadRenderer> renderer = SPK::URHO::IUrho3DQuadRenderer::create(context_);
        renderer->setTexturingMode(SPK::TEXTURE_MODE_2D);
        //renderer->setTexture(textureParticle);
        renderer->setBlendMode(SPK::BLEND_MODE_ADD);
        renderer->enableRenderingOption(SPK::RENDERING_OPTION_DEPTH_WRITE,false);
        renderer->setScale(1.0f, 1.0f);
        renderer->setMaterial(mat);
        renderer->setOrientation(SPK::OrientationPreset::CAMERA_PLANE_ALIGNED);


        SPK::Ref<SPK::Point> point = SPK::Point::create(SPK::Vector3D(0.0f,0.0f,0.0f));

        // emitter
        SPK::Ref<SPK::StaticEmitter> particleEmitter = SPK::StaticEmitter::create(point);
        particleEmitter->setZone(SPK::Point::create(SPK::Vector3D(0.0f,0.0f,0.0f)));
        particleEmitter->setFlow(1);

        // Group
        SPK::Ref<SPK::Group> particleGroup = system_->createGroup(40000);
        particleGroup->addEmitter(particleEmitter);
        particleGroup->setRenderer(renderer);
        particleGroup->setLifeTime(1.0, 1.0f);


        unsigned NB_SPRITE_X = 4;
        unsigned NB_SPRITE_Y = 4;
        unsigned NB_SPRITE_TOTAL = NB_SPRITE_X * NB_SPRITE_Y;
        float SPEED = 1.0f;

        renderer->setAtlasDimensions(NB_SPRITE_X, NB_SPRITE_Y);
        SPK::Ref<SPK::FloatSimpleInterpolator> indexInterpolator = SPK::FloatSimpleInterpolator::create(0.0f, NB_SPRITE_TOTAL * SPEED);
        particleGroup->setParamInterpolator(SPK::PARAM_TEXTURE_INDEX, indexInterpolator);

        _systemCopy = SPK::SPKObject::copy(system_);

        // create spark particle component
        Node* spkSystemNode = _scene->CreateChild("SparkSystem");
        UrhoSparkSystem* spkSystem = spkSystemNode->CreateComponent<UrhoSparkSystem>();
        spkSystem->SetSystem(_systemCopy);
        spkSystemNode->SetPosition(Vector3(0.0f, 1.0f, 0.0f));
    }


    void AnimateScene(float timeStep)
    {
        static float accumulator = 0.0f;
        accumulator += timeStep;
        if(accumulator >= 1.0f)
        {
            String s;

            // display infos
            s= "FPS = ";
            s += String(1 / timeStep) + "\n";
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
    }
};


URHO3D_DEFINE_APPLICATION_MAIN(MyApp)
