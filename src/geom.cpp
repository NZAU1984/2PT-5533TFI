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