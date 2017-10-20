#ifndef H_SPK_URHO3D_DEF
#define H_SPK_URHO3D_DEF

#include <Urho3D/Math/Vector3.h>
#include <Urho3D/Math/Color.h>

// for windows platform only
#if defined(WIN32) || defined(_WIN32)
#ifdef SPK_IRR_EXPORT
#define SPK_IRR_PREFIX __declspec(dllexport)
#elif defined(SPK_IMPORT) || defined(SPK_IRR_IMPORT)
#define SPK_IRR_PREFIX __declspec(dllimport) 
#else
#define SPK_IRR_PREFIX
#endif

#ifdef _MSC_VER
#pragma warning(disable : 4275) // disables the warning about exporting DLL classes children of non DLL classes
#endif

#else
#define SPK_URHO_PREFIX
#endif

namespace SPK {
namespace URHO {

	//////////////////////////
    // Conversion functions //
	//////////////////////////

    /**
    * @brief Converts a SPARK Vector3D to an Urho3D Vector3
	* @param v : the Vector3D to convert
    * @return the Urho3D Vector3
	*/
    inline Urho3D::Vector3 spk2urho(const Vector3D& v)
    {
        return Urho3D::Vector3(v.x, v.y, v.z);
    }

	/**
    * @brief Converts an Urho3D Vector3 to a SPARK Vector3D
    * @param v : the Vector3 to convert
	* @return the SPARK Vector3D
	*/
    inline Vector3D urho2spk(const Urho3D::Vector3& v)
    {
        return Vector3D(v.x_, v.y_, v.z_);
    }

    /**
    * @brief Gets an Urho3D SColor from rgba values
	* @param a : the alpha value
	* @param r : the red value
	* @param g : the green value
	* @param b : the blue value
    * @return the Urho3D SColor
	*/
    inline Urho3D::Color spk2urho(unsigned char a, unsigned char  r, unsigned char  g, unsigned char  b)
    {
        return Urho3D::Color(r,g,b,a);
    }



    inline const Urho3D::Color spk2urho(SPK::Color c)
    {
        return Urho3D::Color(c.r, c.g, c.g, c.a);
    }


}}

#endif
