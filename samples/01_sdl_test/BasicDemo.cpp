#include <SDL.h>
#include <SDL_opengl.h>
#include <stdio.h>

#include <cmath>
#include <iostream>
#include <sstream>
#include <string>
#include <ctime>

#include <GL/gl.h>
#include <GL/glu.h>

#include <SPARK.h>
#include <SPARK_GL.h>

using namespace SPK;
using namespace SPK::GL;

//------------------------------------------------------------------------
// global variables
//------------------------------------------------------------------------

SDL_Window* gWindow = nullptr;      // The window we'll be rendering to
SDL_GLContext gContext = nullptr;   // OpenGL context

const float PI = 3.14159265358979323846f;

const int screenWidth = 640;
const int screenHeight = 480;
float screenRatio;

float angleY = 0.0f;
float angleX = 0.0f;
float camPosZ = 3.0f;
const float CAM_POS_Z = 10.0f;

bool paused = false;
unsigned int renderValue = 0;   // 0 : normal, 1 : basic render, 2 : no render

//int deltaTime = 0;
float step = 0.0f;

// Converts an int into a string
std::string int2Str(int a)
{
    std::ostringstream stm;
    stm << a;
    return stm.str();
}


#include <FTGL/ftgl.h>

FTGLTextureFont* fontPtr = nullptr;

const std::string STR_LOADING = "PLEASE WAIT...";
const std::string STR_NB_PARTICLES = "NB PARTICLES : ";
const std::string STR_FPS = "FPS : ";

std::string strNbParticles = STR_NB_PARTICLES;
std::string strFps = STR_FPS;

int drawText = 2;





//------------------------------------------------------------------------
// Function declarations
//------------------------------------------------------------------------

void InitSpark();
bool CreateScene();
bool loadTexture(GLuint& index, const char *path, GLuint type, GLuint clamp, bool mipmap); // Loads a texture
//void DrawBoundingBox(const Group& group);   // Draws the bounding box for a particle group
bool InitSDL();     // Starts up SDL, creates window, and initializes OpenGL
bool InitGL();      // Initializes matrices and clear color
int RunDemo();      // Create scene and enter Game loop
void Render(System *particleSystem);      // Renders quad to the screen
void Close();       // Frees media and shuts down SDL


//------------------------------------------------------------------------
// Code
//------------------------------------------------------------------------

bool InitSDL()
{
    // Initialization flag
    bool success = true;

    // Initialize SDL
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        success = false;
    }
    else
    {
        // Use OpenGL 2.1
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

        // Create window
        gWindow = SDL_CreateWindow("SPARK DEMO"
                                   , SDL_WINDOWPOS_UNDEFINED
                                   , SDL_WINDOWPOS_UNDEFINED
                                   , screenWidth
                                   , screenHeight
                                   , SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
        if(gWindow == NULL)
        {
            printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
            success = false;
        }
        else
        {
            // Create context
            gContext = SDL_GL_CreateContext(gWindow);
            if(gContext == NULL)
            {
                printf("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
                success = false;
            }
            else
            {
                // Don't use Vsync
                if(SDL_GL_SetSwapInterval(0) < 0)
                {
                    printf("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
                }

                // Initialize OpenGL
                if(!InitGL())
                {
                    printf("Unable to initialize OpenGL!\n");
                    success = false;
                }
            }
        }
    }

    return success;
}

bool InitGL()
{
    bool success = true;
    GLenum error = GL_NO_ERROR;

    //Initialize Projection Matrix
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    //Check for error
    error = glGetError();
    if( error != GL_NO_ERROR )
    {
        success = false;
    }

    //Initialize Modelview Matrix
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    //Check for error
    error = glGetError();
    if( error != GL_NO_ERROR )
    {
        success = false;
    }

    //Initialize clear color
    glClearColor( 0.f, 0.f, 0.f, 1.f );

    //Check for error
    error = glGetError();
    if( error != GL_NO_ERROR )
    {
        success = false;
    }

    // Compute ratio
    screenRatio = (float)screenWidth / (float)screenHeight;

    // Set viewport
    glClearColor(0.0f,0.0f,0.0f,1.0f);
    glViewport(0,0,screenWidth,screenHeight);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45,screenRatio,0.01f,20.0f);
    glEnable(GL_DEPTH_TEST);

    return success;
}

void Close()
{
    SDL_DestroyWindow( gWindow );
    gWindow = NULL;
    SDL_Quit();
}

// Draws the bounding box for a particle system
void DrawBoundingBox(const SPK::System& system)
{
    if (!system.isAABBComputationEnabled())
        return;

    SPK::Vector3D AABBMin = system.getAABBMin();
    SPK::Vector3D AABBMax = system.getAABBMax();

    glDisable(GL_TEXTURE_2D);
    glBegin(GL_LINES);
    glColor3f(1.0f,0.0f,0.0f);

    glVertex3f(AABBMin.x,AABBMin.y,AABBMin.z);
    glVertex3f(AABBMax.x,AABBMin.y,AABBMin.z);

    glVertex3f(AABBMin.x,AABBMin.y,AABBMin.z);
    glVertex3f(AABBMin.x,AABBMax.y,AABBMin.z);

    glVertex3f(AABBMin.x,AABBMin.y,AABBMin.z);
    glVertex3f(AABBMin.x,AABBMin.y,AABBMax.z);

    glVertex3f(AABBMax.x,AABBMax.y,AABBMax.z);
    glVertex3f(AABBMin.x,AABBMax.y,AABBMax.z);

    glVertex3f(AABBMax.x,AABBMax.y,AABBMax.z);
    glVertex3f(AABBMax.x,AABBMin.y,AABBMax.z);

    glVertex3f(AABBMax.x,AABBMax.y,AABBMax.z);
    glVertex3f(AABBMax.x,AABBMax.y,AABBMin.z);

    glVertex3f(AABBMin.x,AABBMin.y,AABBMax.z);
    glVertex3f(AABBMax.x,AABBMin.y,AABBMax.z);

    glVertex3f(AABBMin.x,AABBMin.y,AABBMax.z);
    glVertex3f(AABBMin.x,AABBMax.y,AABBMax.z);

    glVertex3f(AABBMin.x,AABBMax.y,AABBMin.z);
    glVertex3f(AABBMax.x,AABBMax.y,AABBMin.z);

    glVertex3f(AABBMin.x,AABBMax.y,AABBMin.z);
    glVertex3f(AABBMin.x,AABBMax.y,AABBMax.z);

    glVertex3f(AABBMax.x,AABBMin.y,AABBMin.z);
    glVertex3f(AABBMax.x,AABBMax.y,AABBMin.z);

    glVertex3f(AABBMax.x,AABBMin.y,AABBMin.z);
    glVertex3f(AABBMax.x,AABBMin.y,AABBMax.z);
    glEnd();
}

bool loadTexture(GLuint& index,const char* path,GLuint type,GLuint clamp,bool mipmap)
{
    SDL_Surface *particleImg;
    particleImg = SDL_LoadBMP(path);
    if (particleImg == NULL)
    {
        std::cout << "Unable to load bitmap :" << SDL_GetError() << std::endl;
        return false;
    }

    // converts from BGR to RGB
    if ((type == GL_RGB)||(type == GL_RGBA))
    {
        const int offset = (type == GL_RGB ? 3 : 4);
        unsigned char* iterator = static_cast<unsigned char*>(particleImg->pixels);
        unsigned char *tmp0,*tmp1;
        for (int i = 0; i < particleImg->w * particleImg->h; ++i)
        {
            tmp0 = iterator;
            tmp1 = iterator + 2;
            std::swap(*tmp0,*tmp1);
            iterator += offset;
        }
    }

    glGenTextures(1,&index);
    glBindTexture(GL_TEXTURE_2D,index);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,clamp);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,clamp);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    if (mipmap)
    {
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);

        gluBuild2DMipmaps(GL_TEXTURE_2D,
                          type,
                          particleImg->w,
                          particleImg->h,
                          type,
                          GL_UNSIGNED_BYTE,
                          particleImg->pixels);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     type,
                     particleImg->w,
                     particleImg->h,
                     0,
                     type,
                     GL_UNSIGNED_BYTE,
                     particleImg->pixels);
    }

    SDL_FreeSurface(particleImg);

    return true;
}


void InitSpark()
{
    // Sets the update step
    System::setClampStep(true,0.1f);			// clamp the step to 100 ms
    System::useAdaptiveStep(0.001f,0.01f);		// use an adaptive step from 1ms to 10ms (1000fps to 100fps)
}


int RunDemo()
{
    //-----------------------------------------------------------------------------------------------
    // Begin scene
    //-----------------------------------------------------------------------------------------------

    // Loads particle texture
    GLuint textureParticle;
    if (!loadTexture(textureParticle,"res/point.bmp",GL_ALPHA,GL_CLAMP,false))
        return 1;

    SPK::Ref<SPK::System> system_ = SPK::System::create(true);
    system_->setName("Test System");

    // Renderer
    SPK::Ref<SPK::GL::GLQuadRenderer> renderer = SPK::GL::GLQuadRenderer::create();
    renderer->setTexturingMode(SPK::TEXTURE_MODE_2D);
    renderer->setTexture(textureParticle);
    renderer->setBlendMode(SPK::BLEND_MODE_ADD);
    renderer->enableRenderingOption(SPK::RENDERING_OPTION_DEPTH_WRITE,false);
    renderer->setScale(0.2f,0.2f);

    // Emitter
    SPK::Ref<SPK::SphericEmitter> particleEmitter = SPK::SphericEmitter::create(Vector3D(0.0f,1.0f,0.0f),0.1f * PI,0.1f * PI);
    particleEmitter->setZone(Point::create(Vector3D(0.0f,0.015f,0.0f)));
    particleEmitter->setFlow(850);
    particleEmitter->setForce(1.5f,1.5f);

    // Obstacle
    SPK::Ref<SPK::Plane> groundPlane = SPK::Plane::create();
    SPK::Ref<SPK::Obstacle> obstacle = SPK::Obstacle::create(groundPlane,0.9f,1.0f);

    // Group
    SPK::Ref<SPK::Group> particleGroup = system_->createGroup(9000);
    particleGroup->addEmitter(particleEmitter);
    particleGroup->addModifier(obstacle);
    particleGroup->setRenderer(renderer);
    particleGroup->addModifier(SPK::Gravity::create(SPK::Vector3D(0.0f,-1.0f,0.0f)));
    particleGroup->setLifeTime(6.0f,6.0f);
    particleGroup->setColorInterpolator(SPK::ColorSimpleInterpolator::create(0xFFFF0000,0xFF0000FF));

    SPK::Ref<SPK::System> system = SPK::SPKObject::copy(system_);

    //-----------------------------------------------------------------------------------------------
    // End scene
    //-----------------------------------------------------------------------------------------------

    float deltaTime = 0.0f;

    std::deque<clock_t> frameFPS;
    frameFPS.push_back(clock());

    bool quit = false;
    SDL_Event event;

    // While application is running
    while( !quit )
    {
        // Handle events on queue
        while( SDL_PollEvent( &event ) != 0 )
        {
            // user requests quit
            if ((event.type == SDL_QUIT))
                quit = true;

            // if esc is pressed, exit
            if ((event.type == SDL_KEYDOWN)&&(event.key.keysym.sym == SDLK_ESCAPE))
                quit = true;         

            // if F1 is pressed, we display or not the text
            if ((event.type == SDL_KEYDOWN)&&(event.key.keysym.sym == SDLK_F1))
            {
                --drawText;
                if (drawText < 0)
                    drawText = 2;
            }

            // if F2 is pressed, we display or not the bounding boxes
            if ((event.type == SDL_KEYDOWN)&&(event.key.keysym.sym == SDLK_F2))
            {
                system->enableAABBComputation(!system->isAABBComputationEnabled());
            }           

            // if pause is pressed, the system is paused
            if ((event.type == SDL_KEYDOWN)&&(event.key.keysym.sym == SDLK_PAUSE))
                paused = !paused;

            // Moves the camera with the mouse
            if (event.type == SDL_MOUSEMOTION)
            {
                angleY += event.motion.xrel * 0.05f;
                angleX += event.motion.yrel * 0.05f;
                angleX = std::min(179.0f,std::max(1.0f,angleX)); // we cannot look under the ground
            }

            // Zoom in and out
            if (event.type == SDL_MOUSEWHEEL)
            {
                if (event.wheel.y>0)
                    camPosZ = std::min(10.0f,camPosZ + 0.5f);
                else if (event.wheel.y<0)
                    camPosZ = std::max(0.5f,camPosZ - 0.5f);
            }
        }

        if (!paused)
        {
            float t = deltaTime * 100000;
            SPK::Ref<SPK::SphericEmitter> emiter = system->getGroup(0)->getEmitter(0);
            emiter->setDirection(SPK::Vector3D(sin(t),1,cos(t)));

            system->updateParticles(deltaTime);
        }


        glDepthMask(GL_TRUE);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45,screenRatio,0.01f,80.0f);

        glMatrixMode(GL_MODELVIEW);

        glLoadIdentity();
        glTranslatef(0.0f,0.0f,-CAM_POS_Z);
        glRotatef(angleX,1.0f,0.0f,0.0f);
        glRotatef(angleY,0.0f,1.0f,0.0f);


        DrawBoundingBox(*system);
        SPK::GL::GLRenderer::saveGLStates();
        system->renderParticles();
        SPK::GL::GLRenderer::restoreGLStates();

        // draw text
        if (drawText != 0)
        {
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();

            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluOrtho2D(0,screenWidth,0,screenHeight);

            // Renders info strings
            glDisable(GL_DEPTH_TEST);
            glColor3f(1.0f,1.0f,1.0f);
            if (drawText == 2)
            {
                fontPtr->Render(strNbParticles.c_str(),-1,FTPoint(4.0f,40.0f));
            }
            fontPtr->Render(strFps.c_str(),-1,FTPoint(4.0f,8.0f));
        }


        // Computes delta time
        clock_t currentTick = clock();
        deltaTime = (float)(currentTick - frameFPS.back()) / CLOCKS_PER_SEC;
        frameFPS.push_back(currentTick);
        while((frameFPS.back() - frameFPS.front() > 1000)&&(frameFPS.size() > 2))
            frameFPS.pop_front();


        // Updates info strings
        strNbParticles = STR_NB_PARTICLES + int2Str(system->getNbParticles());
        int fps = static_cast<int>(((frameFPS.size() - 1) * 1000000.0f) / (frameFPS.back() - frameFPS.front()));
        if (drawText == 2)
            strFps = STR_FPS + int2Str(fps);
        else
            strFps = int2Str(fps);


        // Update screen
        SDL_GL_SwapWindow(gWindow);
    }

    return 0;
}


int main(int argc, char* args[])
{
    // Loads texture font
    FTGLTextureFont font = FTGLTextureFont("res/font.ttf");
    if(font.Error())
        return 1;
    font.FaceSize(24);
    fontPtr = &font;


    InitSpark();

    if(!InitSDL())
    {
        printf( "Failed to initialize SDL!\n" );
        return -1;
    }

    RunDemo();
    Close();

    return 0;
}
