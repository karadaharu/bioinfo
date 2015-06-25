#ifndef algorithm_basic_h
#define algorithm_basic_h
#include "MT.h"
#include <time.h>

struct linkedListD {
  double v;
  struct linkedListD * next;
};

struct linkedListI {
  int v;
  struct linkedListD * next;
};

int getMax(int* target, int left, int right){
  int targetLen = right - left;
    int max = target[left];
    for(int i = 1; i < targetLen; i++){
        if(max < target[left + i]) max = target[left + i];
    }
    return max;
}

int getMin(int* target, int left , int right){
  int targetLen = right - left;
    int min = target[left];
    for(int i = 1; i < targetLen; i++){
        if(min > target[left+i]) min = target[left+i];
    }
    return min;
}

double getLarger(double a, double b){
  if (a > b){
    return  a;
  } else {
    return b;
  }
}

void initRandmInt(int* target, int len, int max){
  for(int i = 0; i < len; i++){
    target[i] = genrand_int32()%max;
  }
}


void printIntAr(int* target,int n){
  for (int i = 0; i < n; i++) {
    printf("%d,",target[i]);
  }
  printf("\n");
}



#endif
