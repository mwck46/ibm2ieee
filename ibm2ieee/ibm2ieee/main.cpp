/*******************************************************************************
Conversion between IBM and IEEE floating point 

Reference:
https://en.wikipedia.org/wiki/IBM_hexadecimal_floating-point
https://www.crewes.org/Documents/ResearchReports/2017/CRR201725.pdf
*******************************************************************************/
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
	char bias = 64;
	unsigned int buffer = reinterpret_cast<unsigned int&>(ieee);

	char sign = (buffer) >> 31;
	buffer = buffer & 0x80000000;
	
	int intg = static_cast<int>(ieee) * sign;

	return 0;
}

int main()
{
	//
	// Test ibm to ieee
	//
	unsigned int ibm2ieeeTests[10];
	// Case 1: Wiki example
	// IEEE -118.625
	// IBM 0xC276A000
	ibm2ieeeTests[0] = 0xC276A000; 

	// Case 2: 
	// IEEE 0.15625
	// IBM 0x40280000
	ibm2ieeeTests[1] = 0x40280000;

	unsigned int t = ibm2ieeeTests[0];
	float f2 = ibm2ieee(t);

	//
	// Test ieee to ibm
	//
	float ieee2ibmTests[10];
	ieee2ibmTests[0] = -118.625;
	ieee2ibmTests[1] = 0.15625;

	float f1 = reinterpret_cast<float&>(ieee2ibmTests[0]);
	ieee2ibm(f1);

	return 0;
}