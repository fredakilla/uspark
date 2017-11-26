#
# Copyright (c) 2008-2017 the Urho3D project.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#

# Define target name
set (TARGET_NAME Spark)

# Define source files
set (SOURCE_FILES
 	src/Core/IO/SPK_IO_Buffer.cpp
    src/Core/IO/SPK_IO_Descriptor.cpp
    src/Core/IO/SPK_IO_Loader.cpp
    src/Core/IO/SPK_IO_Manager.cpp
    src/Core/IO/SPK_IO_Saver.cpp
    src/Core/SPK_DataSet.cpp
    src/Core/SPK_DEF.cpp
    src/Core/SPK_MemoryTracer.cpp
    src/Core/SPK_Emitter.cpp
    src/Core/SPK_Group.cpp
    src/Core/SPK_Logger.cpp
    src/Core/SPK_Object.cpp
    src/Core/SPK_Octree.cpp
    src/Core/SPK_Renderer.cpp
    src/Core/SPK_System.cpp
    src/Core/SPK_Transform.cpp
    src/Core/SPK_Transformable.cpp
    src/Core/SPK_Zone.cpp
    src/Core/SPK_ZonedModifier.cpp
    src/Extensions/Actions/SPK_ActionSet.cpp
    src/Extensions/Actions/SPK_SpawnParticlesAction.cpp
    src/Extensions/Emitters/SPK_NormalEmitter.cpp
    src/Extensions/Emitters/SPK_RandomEmitter.cpp
    src/Extensions/Emitters/SPK_SphericEmitter.cpp
    src/Extensions/Emitters/SPK_StraightEmitter.cpp
    src/Extensions/IOConverters/SPK_IO_SPKLoader.cpp
    src/Extensions/IOConverters/SPK_IO_SPKSaver.cpp
    src/Extensions/IOConverters/SPK_IO_XMLLoader.cpp
    src/Extensions/IOConverters/SPK_IO_XMLSaver.cpp
    src/Extensions/Modifiers/SPK_BasicModifiers.cpp
    src/Extensions/Modifiers/SPK_Collider.cpp
    src/Extensions/Modifiers/SPK_EmitterAttacher.cpp
    src/Extensions/Modifiers/SPK_LinearForce.cpp
    src/Extensions/Modifiers/SPK_Obstacle.cpp
    src/Extensions/Modifiers/SPK_PointMass.cpp
    src/Extensions/Modifiers/SPK_RandomForce.cpp
    src/Extensions/Modifiers/SPK_Rotator.cpp
    src/Extensions/Modifiers/SPK_Vortex.cpp
    src/Extensions/Renderers/SPK_Oriented3DRenderBehavior.cpp
    src/Extensions/Renderers/SPK_QuadRenderBehavior.cpp
    src/Extensions/Zones/SPK_Box.cpp
    src/Extensions/Zones/SPK_Cylinder.cpp
    src/Extensions/Zones/SPK_Plane.cpp
    src/Extensions/Zones/SPK_Ring.cpp
    src/Extensions/Zones/SPK_Sphere.cpp
    src/Rendering/Urho3D/SPK_Urho3D_Buffer.cpp
    src/Rendering/Urho3D/SPK_Urho3D_QuadRenderer.cpp
    src/Rendering/Urho3D/SPK_Urho3D_Renderer.cpp
    src/Rendering/Urho3D/UrhoSparkSystem.cpp
    src/Rendering/Urho3D/SparkParticleEffect.cpp
    src/Rendering/Urho3D/SPK_Urho3D_DEF.cpp)

# Define dependency libs
set (INCLUDE_DIRS include ${PROJECT_BINARY_DIR}/include)

# Setup target
setup_library ()

# Install headers for building the Urho3D library
install_header_files (DIRECTORY include/ DESTINATION ${DEST_INCLUDE_DIR}/ThirdParty/Spark FILES_MATCHING PATTERN *.h BUILD_TREE_ONLY)  # Note: the trailing slash is significant
