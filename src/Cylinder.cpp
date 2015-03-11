#include "Cylinder.h"
#include "tools.h"

/* Sphere constructor. There is not need to store the center or radius because we will transform the ray using the
transformation matrix. This way, we "enter the sphere's world" where the center is (0, 0, 0) and the radius is 1. */
Cylinder::Cylinder(vec3 position, vec3 orientation, vec3 scaling, Material* material)
	: Geometry(position, orientation, scaling, material)
{

}

std::unique_ptr<struct Intersection> Cylinder::intersect(const struct Ray& ray, decimal &currentdepth) const
{
	/* Let's transform the ray's origin/direction using the inverse of the transformation matrix. It is way simpler
	this way to take transformations into account. */
	vec3 transformedRayOrigin = vec3(_transformationMatrixInverse * vec4(ray.origin, 1.0f));		// e
	vec3 transformedRayDirection = vec3(_transformationMatrixInverse * vec4(ray.direction, 0.0f));	// d

	/* Taken from https://www.cl.cam.ac.uk/teaching/1999/AGraphHCI/SMAG/node2.html
		   - Swap 'y' and 'z'
		   - '_D' = direction, ex. 'x_D' means the 'x' coordinate of the ray direction
		   - '_E' = origin
		   */

	bool checkExtremities = false;

	float a = pow(transformedRayDirection.x, 2) + pow(transformedRayDirection.z, 2); // x_D^2 + z_D^2

	float b = 2 * transformedRayOrigin.x * transformedRayDirection.x
		+ 2 * transformedRayOrigin.z * transformedRayDirection.z; // 2 * x_E * x_D + 2 * z_E * z_D

	float c = pow(transformedRayOrigin.x, 2) + pow(transformedRayOrigin.z, 2) - 1; // x_E^2 + z_E^2 - 1

	float t0, t1; // will hold solutions of the quadratic if calculable

	if (!Tools::calculateQuadratic(a, b, c, t0, t1))
	{
		/* Quadratic can't be solved: the ray and the cylinder don't intersect on the side. But they might intersect
		on the top or bottom extremity. */

		checkExtremities = true;
	}
	else if (0 > t1)
	{
		/* t1 < 0 means that the intersection is behind the scene. Don't forget that Tools:calculateQuadratic() puts
		the smallest value in t0 and the biggest in t1, so if t1 < 0, automatically t0 < 0; */

		return nullptr;
	}

	glm::vec3 intersectionPoint;
	glm::vec3 intersectionNormal;

	if (checkExtremities)
	{
		std::unique_ptr<glm::vec3> intersectionWithExtremity = _findIntersectionWithPlane(ray, glm::vec3(0, 1, 0),
			glm::vec3(1, 1, 0), glm::vec3(0, 1, 1));

		if (nullptr != intersectionWithExtremity)
		{
			intersectionPoint = *intersectionWithExtremity.get();

			if (sqrt(pow(intersectionPoint.x, 2) + pow(intersectionPoint.z, 2)) >= 1.0001
				|| (glm::distance(intersectionPoint, ray.origin) > currentdepth))
			{
				/* Intersecting the top, but not inside the circle OR distance > currentdepth -> let's exit. */

				return nullptr;
			}
			else
			{
				/* Normal here is simply towards Y+ */

				intersectionNormal = glm::normalize(glm::vec3(0, 1, 0));
			}
		}
		else
		{
			intersectionWithExtremity = _findIntersectionWithPlane(ray, glm::vec3(0, -1, 0),
				glm::vec3(1, -1, 0), glm::vec3(0, -1, 1));

			if (nullptr != intersectionWithExtremity)
			{
				intersectionPoint = *intersectionWithExtremity.get();

				if (sqrt(pow(intersectionPoint.x, 2) + pow(intersectionPoint.z, 2)) >= 1.0001
					|| (glm::distance(intersectionPoint, ray.origin) > currentdepth))
				{
					/* Intersecting the bottom, but not inside the circle OR distance > currentDepth -> let's exit. */

					return nullptr;
				}
				else
				{
					/* Normal here is simply towards Y- */

					intersectionNormal = glm::normalize(glm::vec3(0, -1, 0));
				}
			}
		}
	}
	else
	{
		/* Let's calculate the intersection point. If t0 < 0, we must use t1. */
		intersectionPoint = transformedRayOrigin + ((double)((0 > t0) ? t1 : t0)) * transformedRayDirection;

		if ((intersectionPoint.y < -1.0001) || (intersectionPoint.y > 1.0001)
			|| (glm::distance(intersectionPoint, ray.origin) > currentdepth)) // epsilon
		{
			/* If the 'y' coordinate of the intersection is not in [-1, 1], we're not intersection the cylinder
			OR distance > currentdepth -> let's exit. */

			return nullptr;
		}

		intersectionNormal = glm::normalize(vec3(intersectionPoint.x, 0, intersectionPoint.z));
	}

	/* Now calculating uv coordinates. */

	float u, v;

	if (checkExtremities)
	{
		/* On an extremity, the center of the circle is the center of the texture map. We're using page 74 of the
		following reference :
		https://books.google.ca/books?id=YPblYyLqBM4C&pg=PA73&lpg=PA73&dq=texture+mapping+%22circle%22+formula&source=bl&ots=y_77VGkiQb&sig=UcAoLJLQWleyANUvBuC2NlPzpC8&hl=fr&sa=X&ei=acz_VLT_EIK1ggSTgoHoAQ&ved=0CIwCEOgBMCA#v=onepage&q=texture%20mapping%20%22circle%22%20formula&f=false */
		v = sqrt(pow(intersectionPoint.x, 2) + pow(intersectionPoint.z, 2));

		u = (float)(acos(intersectionPoint.x / v) / (2 * glm::pi<float>()));

		if (intersectionPoint.z < -0.0001) // epsilon
		{
			u = 1 - u;
		}
	}
	else
	{
		/* On the side. Using the course's notes (chap. 6, slide 16). Swap 'y' and 'z'. */
		
		if (fabs(intersectionPoint.z) < 0.0001) // epsilon
		{
			u = 0;
		}
		else
		{
			u = (float)(acos(intersectionPoint.x) / (2 * glm::pi<float>())); // acos(x/r)/2*pi but here r=1

			if (intersectionPoint.z < 0)
			{
				u = 1 - u;
			}
		}

		v = (float) ((intersectionPoint.y + 1) / 2);
	}

	vec2 uvCoordinates = vec2(u, v);

	intersectionNormal = glm::normalize(vec3(glm::transpose(_transformationMatrixInverse)
		* glm::vec4(intersectionNormal, 0.0f)));

	intersectionPoint = vec3(_transformationMatrixInverse * vec4(intersectionPoint, 1.0f));

	std::unique_ptr<struct Intersection> intersection(new Intersection{ ray, intersectionPoint, intersectionNormal,
		uvCoordinates, _material });

	return std::move(intersection);
}
