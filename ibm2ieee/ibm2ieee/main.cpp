/*******************************************************************************
Conversion between IBM and IEEE floating point 

Reference:
https://en.wikipedia.org/wiki/IBM_hexadecimal_floating-point
https://www.crewes.org/Documents/ResearchReports/2017/CRR201725.pdf
*******************************************************************************/
#include <cmath>
#include <iostream>

float ibm2ieee_intuitive(unsigned int ibm)
{
	char bias = 64;
	char sign = (ibm & 0x80000000) >> 31;
	char expn = (ibm & 0x7F000000) >> 24;
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

float ibm2ieee(unsigned int ibm)
{
	// copy sign
	unsigned int signBit = (ibm & 0x80000000);

	// fraction
	int frac = (ibm & 0x00FFFFFF);
	int shift = 0;
	do
	{
		// According to IBM standard, the fraction part is normalized by shifting 
		// 4bits at a time, until the leftmost digit is 0
		// Meaning the first bit bust be 0, so a do-while is correct
		frac = frac << 1;
		shift++;
	} while ((frac & 0x00800000) != 0x00800000);

	// Handle implicit 1
	frac = frac & 0x007FFFFF;
	shift++;

	// exponent
	char bias16 = 64;
	char expn16 = (ibm & 0x7f000000) >> 24;
	expn16 = expn16 - bias16;

	char expn2 = 4 * expn16 - shift;
	char bias2 = 127;
	expn2 = expn2 + bias2;
	unsigned int expn = expn2;
	expn = (expn & 0x000000FF) << 23;

	unsigned int buffer = signBit | expn | frac;
	return reinterpret_cast<float&>(buffer);
}

unsigned int ieee2ibm(float ieee)
{
	unsigned int buffer = reinterpret_cast<unsigned int&>(ieee);

	// Get the sign bit
	unsigned int signBit = (buffer) & 0x80000000;
	
	// exponent
	int base2Expn = ((buffer) & 0x7F800000) >> 23;
	int ieeeBias = 127;
	base2Expn = base2Expn - ieeeBias; // remove ieee bias

	// fraction
	unsigned int frac = (buffer) & 0x007FFFFF;
	// add explicit 1
	frac = (frac | 0x00800000);

	// normalize
	// Because the first bit of the fraction in IEEE must be 1, 
	// right shift 4 bit will finish the normalization
	frac = frac >> 3;
	base2Expn = base2Expn + 4;

	// convert from base 2 to base 16
	unsigned int remainder = base2Expn % 4;
	unsigned base16Expn = (base2Expn - remainder) / 4;
	char ibmBias = 64; 
	base16Expn = base16Expn + ibmBias; // add ibm bias

	// multiply the remainder to the fraction 
	frac = frac << (remainder);

	unsigned int ibm = signBit | (base16Expn << 24) | frac;
	return ibm;
}

int main()
{
	unsigned int ibmFloat[2];
	float ieeeFloat[2];
	// Case 1: Wiki example
	// IEEE -118.625, 0xC2ED4000, 11000010 11101101 01000000 00000000
	// IBM            0xC276A000, 11000010 01110110 10100000 00000000
	ibmFloat[0] = 0xC276A000; 
	ieeeFloat[0] = -118.625;

	// Case 2: 
	// IEEE 0.15625
	// IBM 0x40280000
	ibmFloat[1] = 0x40280000;
	ieeeFloat[1] = 0.15625;

	// Test ibm to ieee
	for (int i = 0; i < sizeof(ibmFloat) / sizeof(ibmFloat[0]); i++)
	{
		unsigned int t = ibmFloat[i];
		float f1 = ibm2ieee_intuitive(t);
		float f2 = ibm2ieee(t);

		if (f1 != ieeeFloat[i])
			std::cout << "Test case " << i << " failed for ibm2ieee_intuitive()" << std::endl;
		if (f2 != ieeeFloat[i])
			std::cout << "Test case " << i << " failed for ibm2ieee()" << std::endl;
	}

	// Test ieee to ibm
	for (int i = 0; i < sizeof(ieeeFloat) / sizeof(ieeeFloat[0]); i++)
	{
		unsigned int ui1 = ieee2ibm(ieeeFloat[0]);
		if(ui1 != ibmFloat[i])
			std::cout << "Test case " << i << " failed for ieee2ibm()" << std::endl;
	}

	return 0;
}