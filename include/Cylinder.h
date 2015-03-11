#pragma once
#include "geom.h"

class Cylinder : public Geometry
{
public:
	Cylinder(vec3 position, vec3 orientation, vec3 scaling, Material* material = new Material());

	virtual std::unique_ptr<struct Intersection> intersect(const struct Ray& ray, decimal &currentdepth) const override;
};