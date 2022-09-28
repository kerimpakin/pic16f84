/*
 * Written by Kerim BASOL
 *
 */
#ifndef __PIC_STACK__H_
#define __PIC_STACK__H_ 1

#include <iostream>
#include <cstdio>
#include <cstdlib>

using namespace std;

class pic_stack {
	
private:
	unsigned short stk[8];
	int level;
	int top_level;
	int base_level;
public:
	
	pic_stack()
	{	
		level = 0;
		top_level = 0;
		base_level = 0;
	}

	void push(unsigned short);
	int pop();


};

void
pic_stack::push(unsigned short val)
{	
	stk[level%8] = val;
	level++;

	if( level > 7 ) {
			top_level = level;
			base_level = level - 8;
	}

}

int 
pic_stack::pop()
{

	if(base_level == level)
		return stk[(top_level%8)];

	--level;
	return stk[level%8];

}

#endif
