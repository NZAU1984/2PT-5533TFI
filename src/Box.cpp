#include "Box.h"
#include "tools.h"

/* Sphere constructor. There is not need to store the center or radius because we will transform the ray using the
transformation matrix. This way, we "enter the sphere's world" where the center is (0, 0, 0) and the radius is 1. */
Box::Box(vec3 position, vec3 orientation, vec3 scaling, Material* material)
	: Geometry(position, orientation, scaling, material)
{

}

std::unique_ptr<struct Intersection> Box::intersect(const struct Ray& ray, decimal &currentdepth) const
{
	/* Let's transform the ray's origin/direction using the inverse of the transformation matrix. It is way simpler
	this way to take transformations into account. */
	vec3 transformedRayOrigin = vec3(_transformationMatrixInverse * vec4(ray.origin, 1.0f));		// e
	vec3 transformedRayDirection = vec3(_transformationMatrixInverse * vec4(ray.direction, 0.0f));	// d

	glm::vec3 intersectionPoint(1.0f, 1.0f, 1.0f); // any value, coordinates will be replaced if intersection found
	glm::vec3 intersectionNormal;
	bool found     = false;
	float distance = 0;
	uint faceId    = 0;

	/* Let's check top face Y=1. */
	_findIntersectionWithSide(ray, glm::vec3(0, 1, 0), glm::vec3(1, 1, 0), glm::vec3(0, 1, 1), intersectionPoint,
		found, distance, faceId, 1);

	/* Let's check front face Z=-1. */
	_findIntersectionWithSide(ray, glm::vec3(0, 0, -1), glm::vec3(1, 0, -1), glm::vec3(0, 1, -1), intersectionPoint,
		found, distance, faceId, 2);

	/* Let's check bottom face Y=-1. */
	_findIntersectionWithSide(ray, glm::vec3(0, -1, 0), glm::vec3(1, -1, 0), glm::vec3(0, -1, 1), intersectionPoint,
		found, distance, faceId, 1);

	/* Let's check back face Z=1. */
	_findIntersectionWithSide(ray, glm::vec3(0, 0, 1), glm::vec3(1, 0, 1), glm::vec3(0, 1, 1), intersectionPoint,
		found, distance, faceId, 2);

	/* Let's check left face X=-1. */
	_findIntersectionWithSide(ray, glm::vec3(-1, 0, 0), glm::vec3(-1, 0, 1), glm::vec3(-1, 1, 0), intersectionPoint,
		found, distance, faceId, 3);

	/* Let's check right face X=1. */
	_findIntersectionWithSide(ray, glm::vec3(1, 0, 0), glm::vec3(1, 0, 1), glm::vec3(1, 1, 0), intersectionPoint,
		found, distance, faceId, 3);

	if (!found || distance > currentdepth)
	{
		/* If intersection not found or further away than currentdepth, let's exit. */
		return nullptr;
	}
	
	float u, v;


	if (faceId == 1)
	{
		/* If on top or bottom, Y is constant, but X and Z change so they'll define u and v. */

		u = (intersectionPoint.x + 1) / 2;
		v = (intersectionPoint.z + 1) / 2;
	}
	else if (faceId == 2)
	{
		/* If on front or back, Z is constant, but X and Y change so they'll define u and v. */

		u = (intersectionPoint.x + 1) / 2;
		v = (intersectionPoint.y + 1) / 2;
	}
	else
	{
		/* Otherwise, on left or right, X is constant, but Z and Y change so they'll define u and v. */

		u = (intersectionPoint.z + 1) / 2;
		v = (intersectionPoint.y + 1) / 2;
	}

	vec2 uvCoordinates = vec2(u, v);

	intersectionNormal = glm::normalize(vec3(glm::transpose(_transformationMatrixInverse)
		* glm::vec4(intersectionNormal, 0.0f)));

	intersectionPoint = vec3(_transformationMatrixInverse * vec4(intersectionPoint, 1.0f));

	std::unique_ptr<struct Intersection> intersection(new Intersection{ ray, intersectionPoint, intersectionNormal,
		uvCoordinates, _material });

	return std::move(intersection);
}

void Box::_findIntersectionWithSide(const struct Ray& ray, glm::vec3 point, glm::vec3 vec1, glm::vec3 vec2,
	glm::vec3& intersectionPoint, bool& found, float& distance, uint& faceId, uint thisFaceId) const
{
	/* Let's check if intersection the side's plane. */
	std::unique_ptr<glm::vec3> intersectionWithSide = _findIntersectionWithPlane(ray, point, vec1, vec2);

	if (nullptr != intersectionWithSide)
	{
		glm::vec3 intersectionPoint1 = *intersectionWithSide.get();

		/* Let's check if -1 <= [xyz] <= 1 ([xyz] reads as "x OR y or Z") */
		if ((abs(intersectionPoint1.x) <= 1.0001) && (abs(intersectionPoint1.y) <= 1.0001)
			&& (abs(intersectionPoint1.z) <= 1.0001))
		{
			/* Inside the side */
			float distance1 = glm::distance(intersectionPoint1, ray.origin);

			/* If not already found -> update values ; if already found BUT new distance is smaller -> update values */
			if (!found || (distance1 < distance))
			{
				found               = true;
				distance            = distance1;
				intersectionPoint.x = intersectionPoint1.x;
				intersectionPoint.y = intersectionPoint1.y;
				intersectionPoint.z = intersectionPoint1.z;
				faceId              = thisFaceId;
			}
		}
	}
}
