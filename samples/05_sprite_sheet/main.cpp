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

    SPK::Ref<SPK::System>           _effectExplosion;


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
        CreateExplosion();



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
        spkSystemNode->SetPosition(Vector3(0.0f, 1.0f, -1.0f));
    }


    void CreateExplosion()
    {
        ResourceCache* cache = GetSubsystem<ResourceCache>();

        // load base material
        Material* baseMaterial = cache->GetResource<Material>("Materials/Particle.xml");

        // create material clones and set textures

        SharedPtr<Material> textureExplosion = baseMaterial->Clone();
        textureExplosion->SetTexture(TU_DIFFUSE, cache->GetResource<Texture2D>("res/explosion.bmp"));

        SharedPtr<Material> textureFlash = baseMaterial->Clone();
        textureFlash->SetTexture(TU_DIFFUSE, cache->GetResource<Texture2D>("res/flash.bmp"));

        SharedPtr<Material> textureSpark1 = baseMaterial->Clone();
        textureSpark1->SetTexture(TU_DIFFUSE, cache->GetResource<Texture2D>("res/arrow.png"));

        SharedPtr<Material> textureSpark2 = baseMaterial->Clone();
        textureSpark2->SetTexture(TU_DIFFUSE, cache->GetResource<Texture2D>("res/point.bmp"));

        SharedPtr<Material> textureWave = baseMaterial->Clone();
        textureWave->SetTexture(TU_DIFFUSE, cache->GetResource<Texture2D>("res/wave.bmp"));


        ///////////////
        // Renderers //
        ///////////////

        // smoke renderer
        SPK::Ref<SPK::URHO::IUrho3DQuadRenderer> smokeRenderer = SPK::URHO::IUrho3DQuadRenderer::create(context_);
        smokeRenderer->setTexturingMode(SPK::TEXTURE_MODE_2D);
        smokeRenderer->setMaterial(textureExplosion);
        smokeRenderer->setAtlasDimensions(2,2); // uses 4 different patterns in the texture
        smokeRenderer->setBlendMode(SPK::BLEND_MODE_ALPHA);
        smokeRenderer->enableRenderingOption(SPK::RENDERING_OPTION_DEPTH_WRITE,false);
        smokeRenderer->setShared(true);

        // flame renderer
        SPK::Ref<SPK::URHO::IUrho3DQuadRenderer> flameRenderer = SPK::URHO::IUrho3DQuadRenderer::create(context_);
        flameRenderer->setTexturingMode(SPK::TEXTURE_MODE_2D);
        flameRenderer->setMaterial(textureExplosion);
        flameRenderer->setAtlasDimensions(2,2);
        flameRenderer->setBlendMode(SPK::BLEND_MODE_ADD);
        flameRenderer->enableRenderingOption(SPK::RENDERING_OPTION_DEPTH_WRITE,false);
        flameRenderer->setShared(true);

        // flash renderer
        SPK::Ref<SPK::URHO::IUrho3DQuadRenderer> flashRenderer = SPK::URHO::IUrho3DQuadRenderer::create(context_);
        flashRenderer->setTexturingMode(SPK::TEXTURE_MODE_2D);
        flashRenderer->setMaterial(textureFlash);
        flashRenderer->setBlendMode(SPK::BLEND_MODE_ADD);
        flashRenderer->enableRenderingOption(SPK::RENDERING_OPTION_DEPTH_WRITE,false);
        flashRenderer->setShared(true);

        // spark 1 renderer
        SPK::Ref<SPK::URHO::IUrho3DQuadRenderer> spark1Renderer = SPK::URHO::IUrho3DQuadRenderer::create(context_);
        spark1Renderer->setTexturingMode(SPK::TEXTURE_MODE_2D);
        spark1Renderer->setMaterial(textureSpark1);
        spark1Renderer->setBlendMode(SPK::BLEND_MODE_ADD);
        spark1Renderer->enableRenderingOption(SPK::RENDERING_OPTION_DEPTH_WRITE,false);
        spark1Renderer->setOrientation(SPK::DIRECTION_ALIGNED); // sparks are oriented function of their velocity
        spark1Renderer->setScale(1.05f,1.0f); // thin rectangles
        spark1Renderer->setShared(true);

        // spark 2 renderer
        SPK::Ref<SPK::URHO::IUrho3DQuadRenderer> spark2Renderer = SPK::URHO::IUrho3DQuadRenderer::create(context_);
        spark2Renderer->setTexturingMode(SPK::TEXTURE_MODE_2D);
        spark2Renderer->setMaterial(textureSpark2);
        spark2Renderer->setBlendMode(SPK::BLEND_MODE_ADD);
        spark2Renderer->enableRenderingOption(SPK::RENDERING_OPTION_DEPTH_WRITE,false);
        spark2Renderer->setShared(true);

        // wave renderer
        SPK::Ref<SPK::URHO::IUrho3DQuadRenderer> waveRenderer = SPK::URHO::IUrho3DQuadRenderer::create(context_);
        waveRenderer->setTexturingMode(SPK::TEXTURE_MODE_2D);
        waveRenderer->setMaterial(textureWave);
        waveRenderer->setBlendMode(SPK::BLEND_MODE_ALPHA);
        waveRenderer->enableRenderingOption(SPK::RENDERING_OPTION_DEPTH_WRITE,false);
        waveRenderer->enableRenderingOption(SPK::RENDERING_OPTION_ALPHA_TEST,true); // uses the alpha test
        waveRenderer->setAlphaTestThreshold(0.0f);
        waveRenderer->setOrientation(SPK::FIXED_ORIENTATION); // the orientation is fixed
        waveRenderer->lookVector.set(0.0f,1.0f,0.0f);
        waveRenderer->upVector.set(1.0f,0.0f,0.0f); // we dont really care about the up axis
        waveRenderer->setShared(true);

        //////////////
        // Emitters //
        //////////////

        // This zone will be used by several emitters
        SPK::Ref<SPK::Sphere> explosionSphere = SPK::Sphere::create(SPK::Vector3D(0.0f,0.0f,0.0f),0.4f);

        // smoke emitter
        SPK::Ref<SPK::RandomEmitter> smokeEmitter = SPK::RandomEmitter::create();
        smokeEmitter->setZone(SPK::Sphere::create(SPK::Vector3D(0.0f,0.0f,0.0f),0.6f),false);
        smokeEmitter->setTank(15);
        smokeEmitter->setFlow(-1);
        smokeEmitter->setForce(0.02f,0.04f);

        // flame emitter
        SPK::Ref<SPK::NormalEmitter> flameEmitter = SPK::NormalEmitter::create();
        flameEmitter->setZone(explosionSphere);
        flameEmitter->setTank(15);
        flameEmitter->setFlow(-1);
        flameEmitter->setForce(0.06f,0.1f);

        // flash emitter
        SPK::Ref<SPK::StaticEmitter> flashEmitter = SPK::StaticEmitter::create();
        flashEmitter->setZone(SPK::Sphere::create(SPK::Vector3D(0.0f,0.0f,0.0f),0.1f));
        flashEmitter->setTank(3);
        flashEmitter->setFlow(-1);

        // spark 1 emitter
        SPK::Ref<SPK::NormalEmitter> spark1Emitter = SPK::NormalEmitter::create();
        spark1Emitter->setZone(explosionSphere);
        spark1Emitter->setTank(20);
        spark1Emitter->setFlow(-1);
        spark1Emitter->setForce(2.0f,3.0f);
        spark1Emitter->setInverted(true);

        // spark 2 emitter
        SPK::Ref<SPK::NormalEmitter> spark2Emitter = SPK::NormalEmitter::create();
        spark2Emitter->setZone(explosionSphere);
        spark2Emitter->setTank(400);
        spark2Emitter->setFlow(-1);
        spark2Emitter->setForce(0.4f,1.0f);
        spark2Emitter->setInverted(true);

        // wave emitter
        SPK::Ref<SPK::StaticEmitter> waveEmitter = SPK::StaticEmitter::create();
        waveEmitter->setZone(SPK::Point::create());
        waveEmitter->setTank(1);
        waveEmitter->setFlow(-1);

        ////////////
        // Groups //
        ////////////

        _effectExplosion = SPK::System::create(false); // not initialized as it is the base system
        _effectExplosion->setName("Explosion");

        SPK::Ref<SPK::ColorGraphInterpolator> colorInterpolator;
        SPK::Ref<SPK::FloatGraphInterpolator> paramInterpolator;

        // smoke group
        colorInterpolator = SPK::ColorGraphInterpolator::create();
        colorInterpolator->addEntry(0.0f,0x33333300);
        colorInterpolator->addEntry(0.4f,0x33333366,0x33333399);
        colorInterpolator->addEntry(0.6f,0x33333366,0x33333399);
        colorInterpolator->addEntry(1.0f,0x33333300);

        SPK::Ref<SPK::Group> smokeGroup = _effectExplosion->createGroup(15);
        smokeGroup->setName("Smoke");
        smokeGroup->setPhysicalRadius(0.0f);
        smokeGroup->setLifeTime(2.5f,3.0f);
        smokeGroup->setRenderer(smokeRenderer);
        smokeGroup->addEmitter(smokeEmitter);
        smokeGroup->setColorInterpolator(colorInterpolator);
        smokeGroup->setParamInterpolator(SPK::PARAM_SCALE,SPK::FloatRandomInterpolator::create(0.3f,0.4f,0.5f,0.7f));
        smokeGroup->setParamInterpolator(SPK::PARAM_TEXTURE_INDEX,SPK::FloatRandomInitializer::create(0.0f,4.0f));
        smokeGroup->setParamInterpolator(SPK::PARAM_ANGLE,SPK::FloatRandomInterpolator::create(0.0f,M_PI * 0.5f,0.0f,M_PI * 0.5f));
        smokeGroup->addModifier(SPK::Gravity::create(SPK::Vector3D(0.0f,0.05f,0.0f)));

        // flame group
        colorInterpolator = SPK::ColorGraphInterpolator::create();
        colorInterpolator->addEntry(0.0f,0xFF8033FF);
        colorInterpolator->addEntry(0.5f,0x995933FF);
        colorInterpolator->addEntry(1.0f,0x33333300);

        paramInterpolator = SPK::FloatGraphInterpolator::create();
        paramInterpolator->addEntry(0.0f,0.125f);
        paramInterpolator->addEntry(0.02f,0.3f,0.4f);
        paramInterpolator->addEntry(1.0f,0.5f,0.7f);

        SPK::Ref<SPK::Group> flameGroup = _effectExplosion->createGroup(15);
        flameGroup->setName("Flame");
        flameGroup->setLifeTime(1.5f,2.0f);
        flameGroup->setRenderer(flameRenderer);
        flameGroup->addEmitter(flameEmitter);
        flameGroup->setColorInterpolator(colorInterpolator);
        flameGroup->setParamInterpolator(SPK::PARAM_SCALE,paramInterpolator);
        flameGroup->setParamInterpolator(SPK::PARAM_TEXTURE_INDEX,SPK::FloatRandomInitializer::create(0.0f,4.0f));
        flameGroup->setParamInterpolator(SPK::PARAM_ANGLE,SPK::FloatRandomInterpolator::create(0.0f,M_PI * 0.5f,0.0f,M_PI * 0.5f));

        // flash group
        paramInterpolator = SPK::FloatGraphInterpolator::create();
        paramInterpolator->addEntry(0.0f,0.1f);
        paramInterpolator->addEntry(0.25f,0.5f,1.0f);

        SPK::Ref<SPK::Group> flashGroup = _effectExplosion->createGroup(3);
        flashGroup->setName("Flash");
        flashGroup->setLifeTime(0.2f,0.2f);
        flashGroup->addEmitter(flashEmitter);
        flashGroup->setRenderer(flashRenderer);
        flashGroup->setColorInterpolator(SPK::ColorSimpleInterpolator::create(0xFFFFFFFF,0xFFFFFF00));
        flashGroup->setParamInterpolator(SPK::PARAM_SCALE,paramInterpolator);
        flashGroup->setParamInterpolator(SPK::PARAM_ANGLE,SPK::FloatRandomInitializer::create(0.0f,2.0f * M_PI));

        // spark 1 group
        SPK::Ref<SPK::Group> spark1Group = _effectExplosion->createGroup(20);
        spark1Group->setName("Spark 1");
        spark1Group->setPhysicalRadius(0.0f);
        spark1Group->setLifeTime(0.2f,1.0f);
        spark1Group->addEmitter(spark1Emitter);
        spark1Group->setRenderer(spark1Renderer);
        spark1Group->setColorInterpolator(SPK::ColorSimpleInterpolator::create(0xFFFFFFFF,0xFFFFFF00));
        spark1Group->setParamInterpolator(SPK::PARAM_SCALE,SPK::FloatRandomInitializer::create(0.1f,0.2f));
        spark1Group->addModifier(SPK::Gravity::create(SPK::Vector3D(0.0f,-0.75f,0.0f)));

        // spark 2 group
        SPK::Ref<SPK::Group> spark2Group = _effectExplosion->createGroup(400);
        spark2Group->setName("Spark 2");
        spark2Group->setGraphicalRadius(0.01f);
        spark2Group->setLifeTime(1.0f,3.0f);
        spark2Group->addEmitter(spark2Emitter);
        spark2Group->setRenderer(spark2Renderer);
        spark2Group->setColorInterpolator(SPK::ColorRandomInterpolator::create(0xFFFFB2FF,0xFFFFB2FF,0xFF4C4C00,0xFFFF4C00));
        spark2Group->setParamInterpolator(SPK::PARAM_MASS,SPK::FloatRandomInitializer::create(0.5f,2.5f));
        spark2Group->addModifier(SPK::Gravity::create(SPK::Vector3D(0.0f,-0.1f,0.0f)));
        spark2Group->addModifier(SPK::Friction::create(0.4f));

        // wave group
        paramInterpolator = SPK::FloatGraphInterpolator::create();
        paramInterpolator->addEntry(0.0f,0.0f);
        paramInterpolator->addEntry(0.2f,0.0f);
        paramInterpolator->addEntry(1.0f,3.0f);

        SPK::Ref<SPK::Group> waveGroup = _effectExplosion->createGroup(1);
        waveGroup->setName("Wave");
        waveGroup->setLifeTime(0.8f,0.8f);
        waveGroup->addEmitter(waveEmitter);
        waveGroup->setRenderer(waveRenderer);
        waveGroup->setColorInterpolator(SPK::ColorSimpleInterpolator::create(0xFFFFFF20,0xFFFFFF00));
        waveGroup->setParamInterpolator(SPK::PARAM_SCALE,paramInterpolator);

    }


    void AddExplosion(Vector3 pos)
    {
        SPK::Ref<SPK::System> effectExplosionCopy = SPK::SPKObject::copy(_effectExplosion);

        effectExplosionCopy->initialize();
        //effectExplosionCopy->getTransform().setPosition(SPK::URHO::urho2spk(pos));
        //effectExplosionCopy->updateTransform(); // updates the world transform of system and its children
        //effectExplosionCopy->enableAABBComputation(true);

        // create spark particle component
        Node* spkSystemNode = _scene->CreateChild("Explosion");
        UrhoSparkSystem* spkSystem = spkSystemNode->CreateComponent<UrhoSparkSystem>();
        spkSystem->SetSystem(effectExplosionCopy);
        spkSystemNode->SetPosition(pos);
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
            s += "Press Space bar to lauch explosion\n";
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

        // Add explosion when press space
        if (input->GetKeyPress(KEY_SPACE))
        {
            float x = Random(-4,4);
            float y = Random(1,3);
            float z = Random(-4,4);

            AddExplosion(Vector3(x,y,z));
        }

    }

    void HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData)
    {
        if (_drawDebug)
            GetSubsystem<Renderer>()->DrawDebugGeometry(true);
    }
};


URHO3D_DEFINE_APPLICATION_MAIN(MyApp)
