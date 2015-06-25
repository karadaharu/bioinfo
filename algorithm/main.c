//
//  main.c
//  algorithm
//
//  Created by knekokz on 2015/05/13.
//  Copyright (c) 2015年 knekokz. All rights reserved.
//

#include <stdio.h>
#include "MT.h"
#include "suffix.h"
#include "sort.h"

int main(int argc, const char * argv[]) {
  
      // Generate an array of random numbers
      int n = 10000; // array length
      int* target;
      target = (int *)malloc( sizeof(int) * n);  // Use the heap to generate a long array
      init_genrand((unsigned long) time(NULL));
  
      //////////////// insertion sort ////////////////
      for(int j=0; j<n; j++) {
          target[j] = genrand_int32()%(n/10);
      }
      insertionSort(target, 0, n-1);
      int sorted = 1;
      for(int i = 0; i < (n-1); i++){
          if( target[i] > target[i+1]) sorted = 0;
      }
      if (sorted == 0){
          printf("!!!!!!!!!!!!!not sorted!!!!!!!!!!!!!!!\n");
      }else{
          printf("insertion sort succeeded\n");
      };
      //////////////////////////////////////////////////////////
  
  
      //////////////// ternary split quick sort ////////////////
      for(int j=0; j<n; j++) {
          target[j] = genrand_int32()%(n/10);
      }
      radixSort2(target, 0, n-1, n/2);
      sorted = 1;
      for(int i = 0; i < (n-1); i++){
          if( target[i] > target[i+1]) sorted = 0;
      }
      if (sorted == 0){
          printf("!!!!!!!!!!!!!not sorted!!!!!!!!!!!!!!!\n");
      }else{
          printf("ternary split quick sort succeeded\n");
      };
      //////////////////////////////////////////////////////////
  
      //////////////// radix sort ////////////////
      for(int j=0; j<n; j++) {
          target[j] = genrand_int32()%(n/10);
      }
      radixSort2(target, 0, n-1, n/2);
      sorted = 1;
      for(int i = 0; i < (n-1); i++){
          if( target[i] > target[i+1]) sorted = 0;
      }
      if (sorted == 0){
          printf("!!!!!!!!!!!!!not sorted!!!!!!!!!!!!!!!\n");
      }else{
          printf("radix sort succeeded\n");
      };
      //////////////////////////////////////////////////////////
  
      clock_t sTime, eTime;
      int n_rep = 10;
      sTime = clock();
      for (int k = 0; k < n_rep; k++) {
          for(int j=0; j<n; j++) {
              target[j] = genrand_int32()%(n/10);
          }
          radixSort2(target, 0, n-1, n/2);
          sorted = 1;
          for(int i = 0; i < (n-1); i++){
              if( target[i] > target[i+1]) sorted = 0;
          }
          if (sorted == 0){
              printf("!!!!!!!!!!!!!not sorted!!!!!!!!!!!!!!!\n");
          }else{
              printf("succeeded\n");
          };
      }
      eTime = clock();
      printf("Elapsed time = %f sec.\n", (float)((eTime - sTime)/(float)CLOCKS_PER_SEC)/(float)n_rep );
  
      free(target);
      return 0;

}
