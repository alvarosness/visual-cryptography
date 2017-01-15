#include "utilities.h"

int alphatoint(char* alpha)
{
	int result = 0;
	int n;

	n = strlen(alpha) - 1;
	while (*alpha != '\0')
	{
		result += power(10, n) * (*alpha - '0');
		n--;
		alpha++;
	}

	return result;
}
/*
int strlen(char* text)
{
	int size = 0;
	char* temp = text;

	while (*temp != '\0')
	{
		temp++;
		size++;
	}

	return size;
}
*/
int power(int base, int exponent)
{
	int result = 1;

	for (int i = 0; i < exponent; i++)
		result *= base;
	return result;
}

//number generator
//use static variables
//lookup mean square sum
int numberGenerator(int num, int range)
{
	return (1013 * num + 4111) % range;
}
