#include "delay.h"

void us(unsigned char t)
{   
	while(--t);
}


void ms(unsigned char t)
{
	while(t--)
	{
		us(245);
		us(245);
	}
}


void s(unsigned char t)
{
	while(t--)
	{
		ms(245);
		ms(245);
		ms(245);
		ms(245);
	}
}

