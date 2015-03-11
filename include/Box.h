#pragma once
#include "geom.h"

class Box : public Geometry
{
public:
	Box(vec3 position, vec3 orientation, vec3 scaling, Material* material = new Material());

	virtual std::unique_ptr<struct Intersection> intersect(const struct Ray& ray, decimal &currentdepth) const override;

protected:
	void _findIntersectionWithSide(const struct Ray& ray, glm::vec3 point, glm::vec3 vec1, glm::vec3 vec2,
		glm::vec3& intersectionPoint, bool& found, float& distance, uint& faceId, uint thisFaceId) const;
};