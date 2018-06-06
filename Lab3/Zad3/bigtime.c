#include <stdlib.h>
#include <stdio.h>


void trying(int i){

int* array = malloc(i*sizeof(int));
for (int j = 0; j < i; j++)	
	array[j]=j;
	
printf("Array:%d\n", i);

free(array);

}


int main(){

int i = 0;
while(1)
	{
	trying(i);
	i++;
	}




return 0;
}
