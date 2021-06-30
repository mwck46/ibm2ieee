/*******************************************************************************
Conversion between IBM and IEEE floating point 

Reference:
https://en.wikipedia.org/wiki/IBM_hexadecimal_floating-point
https://www.crewes.org/Documents/ResearchReports/2017/CRR201725.pdf
*******************************************************************************/
#include <cmath>

float ibm2ieee_fast(unsigned int ibm)
{
	// copy sign
	unsigned int buffer = (ibm & 0x80000000);

	// fraction
	int frac = (ibm << 8);
	int shift = 0;
	do
	{
		// According to IBM standard, the fraction part is normalized by shifting 
		// 4bits at a time, until the leftmost digit is 0
		// Meaning the first bit bust be 0, so a do-while is correct
		frac = frac << 1;
		shift++;
	} while (frac & 0x80000000 != 1);

	// Handle implicit 1
	frac = frac << 1;
	shift++;

	frac = (frac >> 9) & 0x007FFFFF;
	buffer = buffer | frac;

	// exponent
	char bias16 = 64;
	char expn16 = (ibm & 0x7f000000) >> 24;
	expn16 = expn16 - bias16;

	char expn2 = 4 * expn16 - shift;
	char bias2 = 127;
	expn2 = expn2 + bias2;
	unsigned int expn = expn2;
	expn = (expn & 0x000000FF) << 23;

	buffer = buffer | expn;
	
	return reinterpret_cast<float&>(buffer);
}

float ibm2ieee_intuitive(unsigned int ibm)
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
	char bias = 64;
	unsigned int buffer = reinterpret_cast<unsigned int&>(ieee);

	char sign = (buffer) >> 31;
	buffer = buffer & 0x80000000;
	
	int intg = static_cast<int>(ieee) * pow(-1, sign);


	return 0;
}

int main()
{
	//
	// Test ibm to ieee
	//
	unsigned int ibm2ieeeTests[10];
	// Case 1: Wiki example
	// IEEE -118.625, 0xC2ED4000, 11000010 11101101 01000000 00000000
	// IBM            0xC276A000, 11000010 01110110 10100000 00000000
	ibm2ieeeTests[0] = 0xC276A000; 

	// Case 2: 
	// IEEE 0.15625
	// IBM 0x40280000
	ibm2ieeeTests[1] = 0x40280000;

	unsigned int t = ibm2ieeeTests[0];
	float f1 = ibm2ieee_intuitive(t);
	float f2 = ibm2ieee_fast(t);

	//
	// Test ieee to ibm
	//
	float ieee2ibmTests[10];
	ieee2ibmTests[0] = -118.625;
	ieee2ibmTests[1] = 0.15625;

	unsigned int f3 = reinterpret_cast<unsigned int&>(ieee2ibmTests[0]);
	ieee2ibm(ieee2ibmTests[1]);

	return 0;
}