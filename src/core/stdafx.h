// header file for building precompiled headers under windows
// a no-op on other architectures
 
// fix mingw build: fileutil (and maybe others?) rely on this header to define PBRT_IS_WINDOWS
//#if defined(_MSC_VER)

#include "pbrt.h"
#include "camera.h"
#include "scene.h"
#include "imageio.h"
#include "intersection.h"
#include "montecarlo.h"
#include "sampler.h"
#include "texture.h"
#include "integrator.h"

//#endif // _MSC_VER
