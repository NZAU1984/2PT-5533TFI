#pragma once
#include "geom.h"

class Cone : public Geometry
{
public:
	Cone(vec3 position, vec3 orientation, vec3 scaling, Material* material = new Material());

	virtual std::unique_ptr<struct Intersection> intersect(const struct Ray& ray, decimal &currentdepth) const override;
};