#include <Urho3D/Urho3DAll.h>

#include <SPARK.h>
#include <SPARK_URHO3D.h>


/// Main application.
class MyApp : public Application
{
public:

    SharedPtr<Scene>                _scene;
    Node*                           _cameraNode;


    MyApp(Context * context) : Application(context)
    {
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


        Material* baseMaterial = cache->GetResource<Material>("Materials/Particle.xml");

        auto material1 = baseMaterial->Clone();
        material1->SetTexture(TU_DIFFUSE, cache->GetResource<Texture2D>("Textures/UrhoIcon.png"));

        auto material2 = baseMaterial->Clone();
        material2->SetTexture(TU_DIFFUSE, cache->GetResource<Texture2D>("Textures/Flare.dds"));


        ParticleEffect* baseEffect = cache->GetResource<ParticleEffect>("Particle/Fire.xml");

        auto effectCopy1 = baseEffect->Clone();
        effectCopy1->SetMaterial(material1);
        effectCopy1->SetMinTimeToLive(0.25);
        effectCopy1->SetMaxTimeToLive(0.25);


        auto effectCopy2 = baseEffect->Clone();
        effectCopy2->SetMaterial(material2);
        effectCopy2->SetMinTimeToLive(30.5);
        effectCopy2->SetMaxTimeToLive(30.5);
        effectCopy2->SetNumParticles(5);

        Node* node1 = _scene->CreateChild("node1");
        node1->SetPosition(Vector3(-1,0,0));
        ParticleEmitter* emitter1 = node1->CreateComponent<ParticleEmitter>();
        emitter1->SetEffect(effectCopy1);

        Node* node2 = _scene->CreateChild("node2");
        node2->SetPosition(Vector3(1,0,0));
        ParticleEmitter* emitter2 = node2->CreateComponent<ParticleEmitter>();
        emitter2->SetEffect(effectCopy2);



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

        GetSubsystem<Input>()->SetMouseVisible(true);
    }







    void HandleKeyDown(StringHash eventType,VariantMap& eventData)
    {
        using namespace KeyDown;
        int key=eventData[P_KEY].GetInt();

        if(key == KEY_ESCAPE)
            engine_->Exit();
    }

    void HandleUpdate(StringHash eventType,VariantMap& eventData)
    {
        using namespace Update;
        float timeStep = eventData[P_TIMESTEP].GetFloat();

        MoveCamera(timeStep);
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
};


URHO3D_DEFINE_APPLICATION_MAIN(MyApp)
