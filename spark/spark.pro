#--------------------------------------------------------------------
# global var
#--------------------------------------------------------------------

unix:!macx: URHO3D = /home/fred/Documents/Urho3D/BUILD

#--------------------------------------------------------------------
# target
#--------------------------------------------------------------------

TEMPLATE = lib
CONFIG -= qt
CONFIG += staticlib
CONFIG += c++11
CONFIG -= debug_and_release
CONFIG -= debug_and_release_target

#--------------------------------------------------------------------
# output directory
#--------------------------------------------------------------------

CONFIG(debug,debug|release){
    DESTDIR = $$PWD/../build
} else {
    DESTDIR = $$PWD/../build
}
QMAKE_CLEAN += $$DESTDIR/$$TARGET

#--------------------------------------------------------------------
# compilation flags
#--------------------------------------------------------------------

unix:!macx: QMAKE_CXXFLAGS_WARN_ON -= -Wall
unix:!macx: QMAKE_CFLAGS_WARN_ON -= -Wall
unix:!macx: QMAKE_CXXFLAGS += -Wall
unix:!macx: QMAKE_CXXFLAGS += -Wno-comment
unix:!macx: QMAKE_CXXFLAGS += -Wno-ignored-qualifiers
unix:!macx: QMAKE_CXXFLAGS += -Wno-unused-parameter
unix:!macx: QMAKE_CXXFLAGS += -std=c++11
unix:!macx: QMAKE_CXXFLAGS += -fpermissive
unix:!macx: QMAKE_CXXFLAGS += -Wno-unused-function
unix:!macx: QMAKE_CXXFLAGS += -Wno-reorder
#unix:!macx: QMAKE_CXXFLAGS += -Wfatal-errors
#unix:!macx: QMAKE_CXXFLAGS += -m32

CONFIG(debug,debug|release) {
    QMAKE_CXXFLAGS_RELEASE += -O0
} else {
    unix:!macx: QMAKE_CXXFLAGS += -Wno-strict-aliasing
    win32:QMAKE_CXXFLAGS_RELEASE -= -Zc:strictStrings
    QMAKE_CXXFLAGS_RELEASE += -O3
}

#--------------------------------------------------------------------
# pre-processor definitions
#--------------------------------------------------------------------

CONFIG(debug,debug|release) {
    #debug
    DEFINES +=  \
        _DEBUG \
        SPK_WITH_XML \

} else {
    # release
    DEFINES +=  \
}

#--------------------------------------------------------------------
# libraries includes
#--------------------------------------------------------------------

INCLUDEPATH += include
INCLUDEPATH += $${URHO3D}/include
INCLUDEPATH += $${URHO3D}/include/Urho3D/ThirdParty

#--------------------------------------------------------------------
# project files
#--------------------------------------------------------------------

HEADERS += \
    include/Core/IO/SPK_IO_Attribute.h \
    include/Core/IO/SPK_IO_Buffer.h \
    include/Core/IO/SPK_IO_Descriptor.h \
    include/Core/IO/SPK_IO_Loader.h \
    include/Core/IO/SPK_IO_Manager.h \
    include/Core/IO/SPK_IO_Saver.h \
    include/Core/SPK_Action.h \
    include/Core/SPK_ArrayData.h \
    include/Core/SPK_Color.h \
    include/Core/SPK_DataHandler.h \
    include/Core/SPK_DataSet.h \
    include/Core/SPK_DEF.h \
    include/Core/SPK_Emitter.h \
    include/Core/SPK_Enum.h \
    include/Core/SPK_Group.h \
    include/Core/SPK_Interpolator.h \
    include/Core/SPK_Iterator.h \
    include/Core/SPK_Logger.h \
    include/Core/SPK_MemoryTracer.h \
    include/Core/SPK_Modifier.h \
    include/Core/SPK_Object.h \
    include/Core/SPK_Octree.h \
    include/Core/SPK_Particle.h \
    include/Core/SPK_Reference.h \
    include/Core/SPK_RenderBuffer.h \
    include/Core/SPK_Renderer.h \
    include/Core/SPK_System.h \
    include/Core/SPK_Transform.h \
    include/Core/SPK_Transformable.h \
    include/Core/SPK_Vector3D.h \
    include/Core/SPK_Zone.h \
    include/Core/SPK_ZonedModifier.h \
    include/Extensions/Actions/SPK_ActionSet.h \
    include/Extensions/Actions/SPK_SpawnParticlesAction.h \
    include/Extensions/Emitters/SPK_NormalEmitter.h \
    include/Extensions/Emitters/SPK_RandomEmitter.h \
    include/Extensions/Emitters/SPK_SphericEmitter.h \
    include/Extensions/Emitters/SPK_StaticEmitter.h \
    include/Extensions/Emitters/SPK_StraightEmitter.h \
    include/Extensions/Interpolators/SPK_DefaultInitializer.h \
    include/Extensions/Interpolators/SPK_GraphInterpolator.h \
    include/Extensions/Interpolators/SPK_RandomInitializer.h \
    include/Extensions/Interpolators/SPK_RandomInterpolator.h \
    include/Extensions/Interpolators/SPK_SimpleInterpolator.h \
    include/Extensions/IOConverters/SPK_IO_SPKLoader.h \
    include/Extensions/IOConverters/SPK_IO_SPKSaver.h \
    include/Extensions/IOConverters/SPK_IO_XMLLoader.h \
    include/Extensions/IOConverters/SPK_IO_XMLSaver.h \
    include/Extensions/Modifiers/SPK_BasicModifiers.h \
    include/Extensions/Modifiers/SPK_Collider.h \
    include/Extensions/Modifiers/SPK_Destroyer.h \
    include/Extensions/Modifiers/SPK_EmitterAttacher.h \
    include/Extensions/Modifiers/SPK_LinearForce.h \
    include/Extensions/Modifiers/SPK_Obstacle.h \
    include/Extensions/Modifiers/SPK_PointMass.h \
    include/Extensions/Modifiers/SPK_RandomForce.h \
    include/Extensions/Modifiers/SPK_Rotator.h \
    include/Extensions/Modifiers/SPK_Vortex.h \
    include/Extensions/Renderers/SPK_LineRenderBehavior.h \
    include/Extensions/Renderers/SPK_Oriented3DRenderBehavior.h \
    include/Extensions/Renderers/SPK_PointRenderBehavior.h \
    include/Extensions/Renderers/SPK_QuadRenderBehavior.h \
    include/Extensions/Zones/SPK_Box.h \
    include/Extensions/Zones/SPK_Cylinder.h \
    include/Extensions/Zones/SPK_Plane.h \
    include/Extensions/Zones/SPK_Point.h \
    include/Extensions/Zones/SPK_Ring.h \
    include/Extensions/Zones/SPK_Sphere.h \
    include/Rendering/DX9/SPK_DX9_Buffer.h \
    include/Rendering/DX9/SPK_DX9_DEF.h \
    include/Rendering/DX9/SPK_DX9_Info.h \
    include/Rendering/DX9/SPK_DX9_LineRenderer.h \
    include/Rendering/DX9/SPK_DX9_LineTrailRenderer.h \
    include/Rendering/DX9/SPK_DX9_PointRenderer.h \
    include/Rendering/DX9/SPK_DX9_QuadRenderer.h \
    include/Rendering/DX9/SPK_DX9_Renderer.h \
    include/Rendering/Irrlicht/CSPKObjectNode.h \
    include/Rendering/Irrlicht/CSPKParticleSystemNode.h \
    include/Rendering/Irrlicht/SPK_IRR_Buffer.h \
    include/Rendering/Irrlicht/SPK_IRR_DEF.h \
    include/Rendering/Irrlicht/SPK_IRR_LineRenderer.h \
    include/Rendering/Irrlicht/SPK_IRR_PointRenderer.h \
    include/Rendering/Irrlicht/SPK_IRR_QuadRenderer.h \
    include/Rendering/Irrlicht/SPK_IRR_Renderer.h \
    include/Rendering/OpenGL/SPK_GL_Buffer.h \
    include/Rendering/OpenGL/SPK_GL_DEF.h \
    include/Rendering/OpenGL/SPK_GL_LineRenderer.h \
    include/Rendering/OpenGL/SPK_GL_LineTrailRenderer.h \
    include/Rendering/OpenGL/SPK_GL_PointRenderer.h \
    include/Rendering/OpenGL/SPK_GL_QuadRenderer.h \
    include/Rendering/OpenGL/SPK_GL_Renderer.h \
    include/SPARK.h \
    include/SPARK_Core.h \
    include/SPARK_DX9.h \
    include/SPARK_GL.h \
    include/SPARK_IRR.h \
    include/Rendering/Urho3D/SPK_Urho3D_Buffer.h \
    include/Rendering/Urho3D/SPK_Urho3D_DEF.h \
    include/Rendering/Urho3D/SPK_Urho3D_PointRenderer.h \
    include/Rendering/Urho3D/SPK_Urho3D_QuadRenderer.h \
    include/Rendering/Urho3D/SPK_Urho3D_Renderer.h \
    include/Rendering/Urho3D/UrhoSparkSystem.h \
    include/SPARK_URHO3D.h \
    include/Rendering/Urho3D/SparkParticleEffect.h \
    external/pugixml/src/pugiconfig.hpp \
    external/pugixml/src/pugixml.hpp

SOURCES += \
    src/Core/IO/SPK_IO_Buffer.cpp \
    src/Core/IO/SPK_IO_Descriptor.cpp \
    src/Core/IO/SPK_IO_Loader.cpp \
    src/Core/IO/SPK_IO_Manager.cpp \
    src/Core/IO/SPK_IO_Saver.cpp \
    src/Core/SPK_DataSet.cpp \
    src/Core/SPK_DEF.cpp \
    src/Core/SPK_MemoryTracer.cpp \
    src/Core/SPK_Emitter.cpp \
    src/Core/SPK_Group.cpp \
    src/Core/SPK_Logger.cpp \
    src/Core/SPK_Object.cpp \
    src/Core/SPK_Octree.cpp \
    src/Core/SPK_Renderer.cpp \
    src/Core/SPK_System.cpp \
    src/Core/SPK_Transform.cpp \
    src/Core/SPK_Transformable.cpp \
    src/Core/SPK_Zone.cpp \
    src/Core/SPK_ZonedModifier.cpp \
    src/Extensions/Actions/SPK_ActionSet.cpp \
    src/Extensions/Actions/SPK_SpawnParticlesAction.cpp \
    src/Extensions/Emitters/SPK_NormalEmitter.cpp \
    src/Extensions/Emitters/SPK_RandomEmitter.cpp \
    src/Extensions/Emitters/SPK_SphericEmitter.cpp \
    src/Extensions/Emitters/SPK_StraightEmitter.cpp \
    src/Extensions/IOConverters/SPK_IO_SPKLoader.cpp \
    src/Extensions/IOConverters/SPK_IO_SPKSaver.cpp \
    src/Extensions/IOConverters/SPK_IO_XMLLoader.cpp \
    src/Extensions/IOConverters/SPK_IO_XMLSaver.cpp \
    src/Extensions/Modifiers/SPK_BasicModifiers.cpp \
    src/Extensions/Modifiers/SPK_Collider.cpp \
    src/Extensions/Modifiers/SPK_EmitterAttacher.cpp \
    src/Extensions/Modifiers/SPK_LinearForce.cpp \
    src/Extensions/Modifiers/SPK_Obstacle.cpp \
    src/Extensions/Modifiers/SPK_PointMass.cpp \
    src/Extensions/Modifiers/SPK_RandomForce.cpp \
    src/Extensions/Modifiers/SPK_Rotator.cpp \
    src/Extensions/Modifiers/SPK_Vortex.cpp \
    src/Extensions/Renderers/SPK_Oriented3DRenderBehavior.cpp \
    src/Extensions/Renderers/SPK_QuadRenderBehavior.cpp \
    src/Extensions/Zones/SPK_Box.cpp \
    src/Extensions/Zones/SPK_Cylinder.cpp \
    src/Extensions/Zones/SPK_Plane.cpp \
    src/Extensions/Zones/SPK_Ring.cpp \
    src/Extensions/Zones/SPK_Sphere.cpp \
    src/Rendering/OpenGL/SPK_GL_Buffer.cpp \
    src/Rendering/OpenGL/SPK_GL_LineRenderer.cpp \
    src/Rendering/OpenGL/SPK_GL_LineTrailRenderer.cpp \
    src/Rendering/OpenGL/SPK_GL_PointRenderer.cpp \
    src/Rendering/OpenGL/SPK_GL_QuadRenderer.cpp \
    src/Rendering/OpenGL/SPK_GL_Renderer.cpp \
    src/Rendering/Urho3D/SPK_Urho3D_Buffer.cpp \
    src/Rendering/Urho3D/SPK_Urho3D_PointRenderer.cpp \
    src/Rendering/Urho3D/SPK_Urho3D_QuadRenderer.cpp \
    src/Rendering/Urho3D/SPK_Urho3D_Renderer.cpp \
    src/Rendering/Urho3D/UrhoSparkSystem.cpp \
    src/Rendering/Urho3D/SparkParticleEffect.cpp \
    src/Rendering/Urho3D/SPK_Urho3D_DEF.cpp







