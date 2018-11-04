#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

void printarr(char * * name,int size){
  int c = 0;
  for(;c < size;c++){
    printf("%s \n",name[c]);
  }
}


char * * alphaboi(char * * list,int size){
  int c = 0;
  int i = 0;
  for(;c < size-1;c++){
    for(i = c ;i < size;i++){
      if(0<strcmp(list[c],list[i])){
        char * temp = list[c];
        list[c] = list[i];
        list[i] = temp;
      }
    }
  }

  return list;
}


int main(){
  char * testboi[5] = {"ceta","alpha","zeda", "beta", "ass"};
  printf("UNALPHA\n");
  printarr(testboi,5);
  printf("\n");
  printf("ALPHABOI\n");
  printarr(alphaboi(testboi,5),5);
  return 0;
}
