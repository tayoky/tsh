#include <stdlib.h>
#include <stdio.h>

//memory leak detector

size_t allocated_seg = 0;
size_t freed_seg     = 0;


void *___malloc(size_t amount){
	if(!amount)return NULL;
	allocated_seg++;
	return malloc(amount);
}

void ___free(void *ptr){
	if(!ptr)return;
	freed_seg++;
	return free(ptr);
}

void *___realloc(void *ptr,size_t amount){
	if(ptr)freed_seg++;
	if(amount)allocated_seg++;
	return realloc(ptr,amount);
}

void malloc_check(void){
	printf("memory leak check :\n");
	printf("%zu seg allocated\n",allocated_seg);
	printf("%zu seg freed\n",freed_seg);
	printf("%zu seg lost\n",allocated_seg - freed_seg);
}
