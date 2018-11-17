#include <stdio.h>

const double sample_freq = 48000.0;

const double root_freq[] =
{
	6.875,
	7.28380877372013,
	7.71692658212691,
	8.17579891564368,
	8.66195721802722,
	9.17702399741896,
	9.722718241315,
	10.3008611535272,
	10.9133822322813,
	11.5623257097385,
	12.2498573744296,
	12.9782717993732
};

int main(int argc, char** argv)
{
	int i;
	int total = 0;

	for ( i=0; i<12; i++ )
	{
		double f = sample_freq/root_freq[i];
		int nsamp = (int)(f+0.5);
		total += nsamp;
		printf("% 2d - %f - %d\n", i, sample_freq/root_freq[i], nsamp);
	}
	printf("Total: %d\n", total);
	
	return 0;
}
