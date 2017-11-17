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
        SparkParticleEffect::RegisterObject(context_);
        SparkParticle::RegisterObject(context_);

        _drawDebug = false;

        SPK::URHO::Urho3DContext::get().registerUrhoContext(context_);
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
        CreateParticles();

         // create spark particle component
        //Node* spkSystemNode = _scene->CreateChild("SparkSystem");
        //SparkParticle* spkSystem = spkSystemNode->CreateComponent<SparkParticle>();
        //spkSystem->SetSystem(_systemCopy);
        //spkSystemNode->SetPosition(Vector3(0.0f, 0.0f, 0.0f));


        // test load spark from file
        {
            Node* node = _scene->CreateChild();
            node->SetPosition(Vector3(-1,0,0));

            /*SparkParticle * sparkParticle = node->CreateComponent<SparkParticle>();
            //sparkParticle->SetSystem(cache->GetResource<SparkParticleEffect>("SparkParticle/test.spk"));

            SparkParticleEffect* sparkEffect = cache->GetResource<SparkParticleEffect>("SparkParticle/test.xml");
            sparkParticle->SetSystem(sparkEffect->GetSystem());*/

            //sparkEffect->Save("Data/SparkParticle/test_copy.xml");

            /* SparkParticle * sparkParticle = node->CreateComponent<SparkParticle>();
            sparkParticle->SetEffect(cache->GetResource<SparkParticleEffect>("SparkParticle/test_copy.xml"));

            Node* node2 = node->Clone();
            node2->SetPosition(Vector3(5,0,0));*/

            // serialize memory test

            {
                Node* node = _scene->CreateChild();
                node->SetPosition(Vector3(-2,0,0));



                // create manually spark effect resource
                SparkParticleEffect* effect = new SparkParticleEffect(context_);
                effect->SetSystem(_systemCopy);
                effect->SetName("MySparkParticleEffectResource");
                cache->AddManualResource(effect);   // ! important

                SparkParticle * sparkParticle = node->CreateComponent<SparkParticle>();
                //sparkParticle->SetSystem(_systemCopy); // from spark object (but clone will doesn't work)
                sparkParticle->SetEffect(effect);   // from resource, clone works

                // can now clone component using manual resource.
                Node* node2 = _scene->CreateChild();
                node2->SetPosition(Vector3(0,0,0));
                node2->CloneComponent(sparkParticle);

                // same for json or xml
                JSONValue jsonElement;
                node->SaveJSON(jsonElement);
                Node* node3 = _scene->InstantiateJSON(jsonElement, Vector3(2,0,0), Quaternion(0, Vector3(0,0,0)));
            }


        }




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

    void CreateParticles()
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
        mat->SetTexture(TU_DIFFUSE, cache->GetResource<Texture2D>("res/arrow.png"));


        SPK::Ref<SPK::System> system_ = SPK::System::create(true);
        system_->setName("Test System");

        // Renderer
        SPK::Ref<SPK::URHO::Urho3DQuadRenderer> renderer = SPK::URHO::Urho3DQuadRenderer::create(context_);
        renderer->setTexturingMode(SPK::TEXTURE_MODE_2D);
        //renderer->setTexture(textureParticle);
        renderer->setBlendMode(SPK::BLEND_MODE_ADD);
        renderer->enableRenderingOption(SPK::RENDERING_OPTION_DEPTH_WRITE,false);
        renderer->setScale(0.05f,0.05f);
        renderer->setMaterial(mat);
        //renderer->setOrientation(SPK::OrientationPreset::CAMERA_PLANE_ALIGNED);
        //renderer->setOrientation(SPK::OrientationPreset::CAMERA_POINT_ALIGNED);
        renderer->setOrientation(SPK::OrientationPreset::DIRECTION_ALIGNED);
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
            s += "Particles count = " + String(_systemCopy->getNbParticles()) + "\n";
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
