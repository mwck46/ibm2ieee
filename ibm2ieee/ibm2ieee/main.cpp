#include <cmath>

float ibm2ieee(unsigned int ibm)
{
	char bias = 64;
	char sign = (ibm & 0x80000000) >> 31;
	char expn = (ibm & 0x7f000000) >> 24;
	expn = expn - bias;

	int frac = (ibm << 8);

	int shift = 0;
	do
	{
		frac = frac << 4;
		shift++;
	} while (frac != 0);

	int fracNoPadding = (ibm << 8);
	fracNoPadding = fracNoPadding >> (sizeof(unsigned int) * 8 - shift * 4);

	float rst = pow(-1, sign) * fracNoPadding * pow(16, expn - shift);
	return rst;
}

unsigned int ieee2ibm(float ieee)
{

	return 0;
}

int main()
{
	unsigned int testList[10];
	float f;

	f = +1;
	testList[0] = reinterpret_cast<unsigned int&>(f);
	f = -1;
	testList[1] = reinterpret_cast<unsigned int&>(f);
	f = +0;
	testList[2] = reinterpret_cast<unsigned int&>(f);
	f = -0;
	testList[3] = reinterpret_cast<unsigned int&>(f);
	f = +0.15625f;
	testList[4] = reinterpret_cast<unsigned int&>(f);
	f = -0.15625f;
	testList[5] = reinterpret_cast<unsigned int&>(f);

	// Test ibm to ieee
	// Case 1: Wiki example, IEEE float value -118.625, IBM encoding is 0xC276A000
	// see https://en.wikipedia.org/wiki/IBM_hexadecimal_floating-point
	testList[6] = static_cast<unsigned int>(0xC276A000); 

	// Case 2: 
	// IEEE float value 0.15625
	// IBM encoding 0x40280000
	testList[7] = static_cast<unsigned int>(0x40280000);
	//testList[] =reinterpret_cast< (unsigned int)();


	unsigned int t = testList[7];


	float f1 = reinterpret_cast<float&>(t);

	float f2 = ibm2ieee(t);


	return 0;
}