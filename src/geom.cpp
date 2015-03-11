#include <geom.h>
#include <basic_structs.h>
#include <glm/gtx/euler_angles.hpp>
#include <iostream>
#include <array>


//https://code.google.com/p/pwsraytracer/source/browse/trunk/raytracer/
Geometry::Geometry(vec3 position, vec3 orientation, vec3 scaling, Material* mtl)
	:
	_material(mtl)
{
	_transformationMatrix =
		glm::translate(glm::mat4(), position)						// translation (last)
		* glm::rotate(glm::mat4(), orientation.x, vec3(1.0f, 0, 0)) // X rotation
		* glm::rotate(glm::mat4(), orientation.y, vec3(0, 1.0f, 0)) // Y rotation
		* glm::rotate(glm::mat4(), orientation.z, vec3(0, 0, 1.0f)) // Z rotation
		* glm::scale(glm::mat4(), scaling);							// scaling (first)

	_transformationMatrixInverse = glm::inverse(_transformationMatrix);
}

/* Calculate the intersection between a ray (a line) and a plane, given a point in it and two vectors, also in it.
Inspired from our own TP1 before we realized we simply had to apply the inverse of the transformation matrix to a
point to check for collisions... Please see lines 373+ of
https://github.com/NZAU1984/1PT-5533TFI/blob/34b2d400f66365800055bc506ba37d4475aa4cf6/TP1/src/glbase/scene.cpp */
std::unique_ptr<glm::vec3> Geometry::_findIntersectionWithPlane(const struct Ray& ray, glm::vec3 point, glm::vec3 vec1,
	glm::vec3 vec2) const
{
	glm::vec3 perpVec = glm::normalize(glm::cross(vec1, vec2));

	if (glm::dot(ray.direction, perpVec) == 0)
	{
		// Parallel, they won't meet. 

		return nullptr;
	}

	float t = (perpVec.x * point.x + perpVec.y * point.y + perpVec.z * point.z
		- perpVec.x * ray.origin.x - perpVec.y * ray.origin.y - perpVec.z * ray.origin.z)
		/ (perpVec.x * ray.direction.x + perpVec.y * ray.direction.y + perpVec.z * ray.direction.z);

	std::unique_ptr<glm::vec3> intersectionPoint(new glm::vec3(ray.origin.x + ray.direction.x * t, ray.origin.y + ray.direction.y * t,
		ray.origin.z + ray.direction.z * t));

	return std::move(intersectionPoint);
}