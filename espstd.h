#ifndef _ESPSTD_H_
#define _ESPSTD_H_ 1

// ------------------------

#include <stdio.h>
#include <stdlib.h>

char* convertbin(unsigned int sayi, char bit_len)
{
	char *str = (char *)malloc(sizeof(char) * bit_len);
	int bit_pos =  1 << (bit_len - 1);
	
	int i;
	
	for(i = 0; i <  bit_len; i++) {
		
		if( bit_pos & sayi )
			str[i] = '1';
		else 
			str[i] = '0';
		
		sayi <<= 1;

	}
	
	str[i] = '\0';

	return str;
}


void printbin(unsigned int sayi, char bit_len)
{	
	int bit_pos =  1 << (bit_len - 1);
	
	int i;
	
	for(i = 0; i <  bit_len; i++) {
		
		if( bit_pos & sayi )
			printf("1");
		else 
			printf("0");
		
		sayi <<= 1;

	}	
	
}





// -------------------------
#endif
