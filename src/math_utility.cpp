#include <cstdlib>
#include <math_utility.h>

int RandomRange(const int start, const int end)
{
	const int random = rand();
	return (random % (abs(start) + abs(end))) + start;
}
