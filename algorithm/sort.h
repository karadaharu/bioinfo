//
//  main.c
//  sorting
//
//  Created by knekokz on 2015/04/27.
//  Copyright (c) 2015年 knekokz. All rights reserved.
//

#ifndef _SORT
#define _SORT

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "MT.h"
#include <stdbool.h>
#include "basic.h"

int selectRandomPosition(int * , int, int);
int digit(int , int , int );

/*
 * 演習問題 1.2
 * 挿入ソートの実装
 */
void insertionSort(int* target, int left, int right){
    for(int i = left+1; i <= right; i++){
        int j = i;
        while (j > left && target[j] < target[j-1]) {
            int tmp = target[j];
            target[j] = target[j-1];
            target[j-1] = tmp;
            j--;
        }
    }
}

/*
 * 演習問題 1.4
 * ternary quick sortの実装
 *
 */
void partitionTQS(int* target, int left, int right, int* next_l, int* next_r){
    // Select a number between left and right at random.
    int random = selectRandomPosition(target, left, right);
    // To select the maximum element, use:
    // int random = selectMaxPosition(target, left, right);
    // Exchange target[right] and target[random].
    int tmp = target[right];
    target[right] = target[random];
    target[random] = tmp;
    
    int pivot = target[right];
    int i = left-1;  // i scans the array from the left.
    int j = right;  // j scans the array from the right.
    
    bool l_same = false;
    bool r_same = false;
    
    for (;;) {
        bool l_to_same = false;
        bool r_to_same = false;
        
        // Move from the left until hitting a value no less than the pivot.
        for(i++; target[i] < pivot; i++){}
        // Move from the right until hitting a value no greater than the pivot.
        for(j--; pivot < target[j] && i < j; j--){}
        if (i >= j)  break;
        
        if(!l_same && target[i] == pivot){
            tmp = target[i];
            target[i] = target[left];
            target[left] = tmp;
            l_same = true;
            l_to_same = true;
        }
        if(!r_same && target[j] == pivot){
            tmp = target[j];
            target[j] = target[right-1];
            target[right-1] = tmp;
            r_same = true;
            r_to_same = true;
        }
        
        if(l_to_same && !r_to_same){
            j++;
        }else if (!l_to_same && r_to_same){
            i--;
        }
        if(l_to_same || r_to_same){
            continue;
        }
        
        // Exchange target[i] and target[j].
        tmp = target[i];  target[i] = target[j];  target[j] = tmp;
    }
    // Exchange target[i] and target[right].
    tmp = target[i];  target[i] = target[right];  target[right] = tmp;
    
    (*next_r) = i - 1;
    (*next_l) = i + 1;
    
    if(l_same && left < i-1){
        tmp = target[left];
        target[left] = target[i-1];
        target[i-1] = tmp;
        (*next_r) = i - 2;
    }
    if(r_same && i + 1 < right){
        tmp = target[right-1];
        target[right-1] = target[i+1];
        target[i+1] = tmp;
        (*next_l) = i + 2;
    }
}

void ternaryQuickSort(int* target, int left, int right) {
    int next_r = 0;
    int next_l = 0;
    if (left < right){
        partitionTQS(target,left,right, &next_l, &next_r);
        // i indicates the position of the pivot.
        ternaryQuickSort(target, left, next_r);
        ternaryQuickSort(target, next_l, right);
    }
}

/*
 * 演習問題1.5
 * 上位の桁からソートするradix sortの実装
 *
 */

void radixSort2Sub(int* target, int left, int right, int m, int j){
    int targetLen = right - left + 1;
    int* tmp = (int *) malloc(sizeof(int)* targetLen);
    if(tmp == NULL) exit(1);
    
    int* count = (int *) malloc( sizeof(int)* m);
    int* count_each = (int *) malloc( sizeof(int)* m);
    
    // init count
    for(int i = 0; i < m; i++){ count[i] = 0; }
    // count n of each number
    for(int i = 0; i < targetLen; i++){
        count[digit(target[left + i], j, m)]++;
    }
    
    for (int i = 0; i < m; i++) {
        count_each[i] = count[i];
    }
    
    // count the number of elements  <= i
    for (int i = 1; i < m; i++) {
        count[i] = count[i-1] + count[i];
    }
    
    // insert element in the proper position in temp
    for (int i = targetLen - 1; 0 <= i ; i--) {
        tmp[ --count[digit(target[left + i], j, m)]] = target[left + i];
    }
    
    // copy temp to target
    for (int i = 0; i < targetLen; i++) {
        target[left + i] = tmp[i];
    }
    
    free(tmp);
    free(count);
    
    // sort next digit recursively
    if(j > 1){
        j--;
        int next_l, next_r;
        next_r = left -1 ;
        for(int i = 0; i < m; i++){
            if(count_each[i] <= 1){
                next_r = next_r + count_each[i];
                next_l = next_r;
                continue;
            }
            next_l = next_r + 1;
            next_r = next_r + count_each[i];
            radixSort2Sub(target, next_l, next_r, m, j);
        }
    }
    free(count_each);
}


// m: cardinal number
// sort from most significant digit
void radixSort2(int* target, int left, int right, int m){
    int targetLen = right - left + 1;
    int max = getMax(target, 0, targetLen);
    int maxDigit = 0;
    for(; max>0; maxDigit++){
        max = max/m;
    }
    radixSort2Sub(target, left, right, m, maxDigit);
}

// return m進数でj桁目の値
int digit(int num, int j, int m){
    int quotient = num;
    int remainder = 0;
    for(int i = 0; i<j; i++){
        remainder = quotient%m;
        quotient = quotient/m;
    }
    return  remainder;
}

int selectRandomPosition(int* target, int left, int right){
    int length = right - left + 1;
    int r = left + genrand_int32()%length;
    return r;
}

int selectMaxPosition(int* target, int left, int right){
    int m = target[left];
    int r = left;
    for(int i=left+1; i<= right; i++){
        if( m < target[i]){
            m = target[i];
            r = i;
        }
    }
    return r;
}

int partition(int* target, int left, int right) {
    // Select a number between left and right at random.
    int random = selectRandomPosition(target, left, right);
    // To select the maximum element, use:
    // int random = selectMaxPosition(target, left, right);
    // Exchange target[right] and target[random].
    int tmp = target[right];
    target[right] = target[random];
    target[random] = tmp;
    
    int pivot = target[right];
    int i = left-1;  // i scans the array from the left.
    int j = right;  // j scans the array from the right.
    
    for (;;) {
        // Move from the left until hitting a value no less than the pivot.
        for(i++; target[i] < pivot; i++){}
        // Move from the right until hitting a value no greater than the pivot.
        for(j--; pivot < target[j] && i < j; j--){}
        if (i >= j)  break;
        // Exchange target[i] and target[j].
        tmp = target[i];  target[i] = target[j];  target[j] = tmp;
    }
    // Exchange target[i] and target[right].
    tmp = target[i];  target[i] = target[right];  target[right] = tmp;
    return i;
}


void randomQuickSort1(int* target, int left, int right ) {
    if (left < right){
        int i = partition(target,left,right);
        // i indicates the position of the pivot.
        randomQuickSort1(target, left, i - 1);
        randomQuickSort1(target, i + 1, right);
    }
}

void randomQuickSort2(int* target, int aLeft, int right ) {
    int left = aLeft;
    while (left < right) {
        int i = partition(target,left,right);
        // i indicates the position of the pivot.
        randomQuickSort2(target, left, i - 1);
        left = i + 1;
    }
}

void randomQuickSort3(int* target, int aLeft, int right ) {
    int left = aLeft;
    while (left < right) {
        int i = partition(target,left,right);
        // i indicates the position of the pivot.
        if( i - left <= right - 1){
            randomQuickSort3(target, left, i - 1);
            left = i + 1;
        }else{
            randomQuickSort3(target, i + 1, right);
            right = i - 1;
        }
    }
}

void randomQuickSort4(int* target, int aLeft, int aRight, int minSize) {
    int left = aLeft;
    int right = aRight;
    
    while (left + minSize < right) {
        int i = partition(target,left,right);
        // i indicates the position of the pivot.
        if( i - left <= right - 1){
            randomQuickSort4(target, left, i - 1, minSize);
            left = i + 1;
        }else{
            randomQuickSort4(target, i + 1, right, minSize);
            right = i - 1;
        }
    }
    insertionSort(target, left, right);
}

void randomQuickSort5Sub(int* target, int aLeft, int aRight, int minSize){
    int left = aLeft;
    int right = aRight;
    
    while (left + minSize < right) {
        int i = partition(target,left,right);
        // i indicates the position of the pivot.
        if( i - left <= right - 1){
            randomQuickSort5Sub(target, left, i - 1, minSize);
            left = i + 1;
        }else{
            randomQuickSort5Sub(target, i + 1, right, minSize);
            right = i - 1;
        }
    }
}


void randomQuickSort5(int* target, int left, int right, int minSize) {
    randomQuickSort5Sub(target, left, right, minSize);
    insertionSort(target, left, right);
}

// m: cardinal number
void radixSort(int* target, int targetLen, int m){
    int* tmp = (int *) malloc(sizeof(int)* targetLen);
    if(tmp == NULL) exit(1);
    
    int* count = (int *) malloc( sizeof(int)* m);
    
    int max = getMax(target, 0, targetLen);
    int maxDigit = 0;
    for(; max>0; maxDigit++){
        max = max/m;
    }
    
    // from the least significant digit, sort elements
    for(int j = 1; j <= maxDigit; j++){
        // j denotes the significant digit
        
        // init count
        for(int i = 0; i < m; i++){ count[i] = 0; }
        // count n of each number
        for(int i = 0; i < targetLen; i++){
            count[digit(target[i], j, m)]++;
        }
        
        // count the number of elements  <= i
        for (int i = 1; i < m; i++) {
            count[i] = count[i-1] + count[i];
        }
        
        // insert element in the proper position in temp
        for (int i = targetLen - 1; 0 <= i ; i--) {
            tmp[ --count[digit(target[i], j, m)]] = target[i];
        }
        
        // copy temp to target
        for (int i = 0; i < targetLen; i++) {
            target[i] = tmp[i];
        }
    }
    free(tmp);
    free(count);
}

#endif

//int main(int argc, char** argv){
// Generate an array of random numbers
//  int n = 10000; // array length
//  int* target;
//  target = (int *)malloc( sizeof(int) * n);  // Use the heap to generate a long array
//  init_genrand((unsigned long) time(NULL));
//
//  //////////////// insertion sort ////////////////
//  for(int j=0; j<n; j++) {
//    target[j] = genrand_int32()%(n/10);
//  }
//  insertionSort(target, 0, n-1);
//  int sorted = 1;
//  for(int i = 0; i < (n-1); i++){
//    if( target[i] > target[i+1]) sorted = 0;
//      }
//  if (sorted == 0){
//    printf("!!!!!!!!!!!!!not sorted!!!!!!!!!!!!!!!\n");
//  }else{
//    printf("insertion sort succeeded\n");
//  };
//  //////////////////////////////////////////////////////////
//
//
//  //////////////// ternary split quick sort ////////////////
//  for(int j=0; j<n; j++) {
//    target[j] = genrand_int32()%(n/10);
//  }
//  radixSort2(target, 0, n-1, n/2);
//  sorted = 1;
//  for(int i = 0; i < (n-1); i++){
//    if( target[i] > target[i+1]) sorted = 0;
//      }
//  if (sorted == 0){
//    printf("!!!!!!!!!!!!!not sorted!!!!!!!!!!!!!!!\n");
//  }else{
//    printf("ternary split quick sort succeeded\n");
//  };
//  //////////////////////////////////////////////////////////
//
//  //////////////// radix sort ////////////////
//  for(int j=0; j<n; j++) {
//    target[j] = genrand_int32()%(n/10);
//  }
//  radixSort2(target, 0, n-1, n/2);
//  sorted = 1;
//  for(int i = 0; i < (n-1); i++){
//    if( target[i] > target[i+1]) sorted = 0;
//      }
//  if (sorted == 0){
//    printf("!!!!!!!!!!!!!not sorted!!!!!!!!!!!!!!!\n");
//  }else{
//    printf("radix sort succeeded\n");
//  };
//  //////////////////////////////////////////////////////////
//
//  clock_t sTime, eTime;
//  int n_rep = 10;
//  sTime = clock();
//  for (int k = 0; k < n_rep; k++) {
//    for(int j=0; j<n; j++) {
//      target[j] = genrand_int32()%(n/10);
//    }
//    radixSort2(target, 0, n-1, n/2);
//    sorted = 1;
//    for(int i = 0; i < (n-1); i++){
//      if( target[i] > target[i+1]) sorted = 0;
//        }
//    if (sorted == 0){
//      printf("!!!!!!!!!!!!!not sorted!!!!!!!!!!!!!!!\n");
//    }else{
//      printf("succeeded\n");
//    };
//  }
//  eTime = clock();
//  printf("Elapsed time = %f sec.\n", (float)((eTime - sTime)/(float)CLOCKS_PER_SEC)/(float)n_rep );
//
//  free(target);
//  return 0;
//}
