#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main()
{
	void *ptr1 = malloc(24*1024*1024);
	printf("ptr1: %p\n", ptr1);
	assert(ptr1 != 0);
	void *ptr2 = malloc(24*1024*1024);
	printf("ptr2: %p\n", ptr2);
	assert(ptr2 == 0);
#ifdef REPORT_RESULT
	int result = 1;
	REPORT_RESULT();
#endif
}
