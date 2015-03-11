#include "tools.h"

/* Tries to solve the quadratic equation. If the discriminant can't be calculated (sqrt(x) where x < 0), then it
simply returns false. Otherwise, it returns true and calculate both answers and stores them in t0 and t1. */
bool Tools::calculateQuadratic(float a, float b, float c, float& t0, float& t1)
{
	float discriminant = pow(b, 2) - 4 * a * c;

	if (0 > discriminant)
	{
		// Negative discriminant, imaginary number, let's return false.

		return false;
	}
	else if (0 == discriminant)
	{
		// No need to calcule both values separately because +0 and -0 yield the same answer.
		t0 = t1 = -b / (2 * a);

		return true;
	}

	// The sqrt can be calculated so let's do it.
	discriminant = sqrt(discriminant);

	// Getting both answers.
	t0 = (-b - discriminant) / (2 * a);
	t1 = (-b + discriminant) / (2 * a);

	// Taking the min for t0, and the max for t1.
	t0 = fmin(t0, t1);
	t1 = fmax(t0, t1);

	return true;
}