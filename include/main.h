#pragma once

#include <memory>
#include <vector>

#define GLM_SWIZZLE
#define GLM_FORCE_RADIANS
#define GLM_PRECISION_HIGHP_FLOAT
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <log.h>

#ifndef _MSC_VER
#if __GNUC__ != 4 || __GNUC_MINOR__ < 7
#define override
#define nullptr NULL
#endif
#include <strings.h>
#define _stricmp strcasecmp
#endif

#define abstract = 0

using namespace glm;

typedef glm::float_t decimal;

extern bool use_fresnel;

inline decimal pi() { return glm::pi<decimal>(); }
inline decimal epsilon() { return glm::epsilon<decimal>(); }
inline decimal one_over_pi() { return glm::one_over_pi<decimal>(); }

#if defined(_MSC_VER) && _MSC_VER <= 1600
typedef unsigned char uint8_t;
#include <float.h>
#define INFINITY 1e+300 * 1e+300
namespace std
{
	inline bool isfinite(double d)
	{
		return _finite(d) != 0;
	}
};
#endif
