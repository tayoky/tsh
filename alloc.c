#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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

char *___strndup(const char *src,size_t size){
	allocated_seg++;
	return strndup(src,size);
}

char *___strdup(const char *src){
	allocated_seg++;
	return strdup(src);
}
void start_malloc_check(void){
	allocated_seg = 0;
	freed_seg = 0;
}
void malloc_check(void){
	printf("memory leak check :\n");
	printf("%zu seg allocated\n",allocated_seg);
	printf("%zu seg freed\n",freed_seg);
	printf("%zu seg lost\n",allocated_seg - freed_seg);
}
