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

#pragma once

#include "../Graphics/GraphicsDefs.h"
#include "../Resource/Resource.h"

#include <SPARK_Core.h>
#include <SPARK_URHO3D.h>

namespace Urho3D
{

//class Material;
class XMLFile;
class XMLElement;

/// %Particle effect definition.
class URHO3D_API SparkParticleEffect : public Resource
{
    URHO3D_OBJECT(SparkParticleEffect, Resource);

public:
    /// Construct.
    SparkParticleEffect(Context* context);
    /// Destruct.
    virtual ~SparkParticleEffect() override;
    /// Register object factory.
    static void RegisterObject(Context* context);

    /// Load resource from stream. May be called from a worker thread. Return true if successful.
    virtual bool BeginLoad(Deserializer& source) override;
    /// Finish resource loading. Always called from the main thread. Return true if successful.
    virtual bool EndLoad() override;


    /// Helper function for loading XML files.
    bool BeginLoadSPK(Deserializer& source);

    const SPK::Ref<SPK::System> GetSystem() const;

    /// Clone the particle system.
    SharedPtr<SparkParticleEffect> Clone(const String& cloneName = String::EMPTY) const;



private:
    SPK::Ref<SPK::System> loadedSystem_;

};

}
