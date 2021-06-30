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

	float rst = sign * fracNoPadding * pow(16, expn - shift);
	return rst;
}

int main()
{
	unsigned int testList[10];
	testList[0] = (unsigned int)(+1);
	testList[1] = (unsigned int)(-1);
	testList[2] = (unsigned int)(+0);
	testList[3] = (unsigned int)(-0);
	testList[4] = (unsigned int)(+0.15625f);
	testList[5] = (unsigned int)(-0.15625f);
	testList[6] = (unsigned int)(0xC276A000);
	testList[7] = unsigned int(0xc1180000);
	//testList[] = (unsigned int)();


	unsigned int t = testList[7];

	float f2 = ibm2ieee(t);


	return 0;
}