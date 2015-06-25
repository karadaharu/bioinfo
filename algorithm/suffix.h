#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "MT.h"
#include <time.h>
#include "basic.h"
#include <stdbool.h>

#ifndef algorithm_suffix_h
#define algorithm_suffix_h

#define S 0
#define L 1

#define LESS -1
#define EQUAL 0

// Larsson Sadakane algorithm
void constructSAbyLS(char* target, int n, int* sa);
void createBuckets(int n, int* rank, int* buc, int l);
void createSumBuckets(int n, int* rank, int* buc_use,int* buc_sum, int l);
void sortByRank(int* sa, int left, int right, int* rank, int* buc, int l);
void setISA(int* sa, int* isa, int* buc, int* rank,int left,int right);
void constructSASub(int* sa, int* isa, int left, int right,int k, int n_sa,  int l);
void constructSA(char* target, int n, int* sa);
void initRankByChar(char* target, int* rank, int n);

// Induced Sorting
void constructSAbyIS(char* target, int n, int* sa);
void sortLMS(int* sa,int* t,int* rank,int n,int* buc,int l);
void sortLTypeSuffixes(int*sa, int*t, int* rank, int n, int* buc, int l);
void sortSTypeSuffixes(int*sa, int*t, int* rank, int n, int* buc, int l);
void setEquality(int* sa, int* rank, int* ineq, int i, int i_v, int v, int n);


/**
 * select random value from min to max;
 */
int selectRandomValue(int min, int max){
  int length = max - min + 1;
  int r = min + genrand_int32()%length;
  return r;
}

/**
 * set partition for 2 array
 */
int partitionForTwoArray(int* target, int* targetSub, int left, int right) {
  // Select a number between left and right at random.
  int random = selectRandomValue(left, right);
  // To select the maximum element, use:
  // int random = selectMaxPosition(target, left, right);
  // Exchange target[right] and target[random].
  int tmp = target[right];
  target[right] = target[random];
  target[random] = tmp;
  
  int tmpSub = targetSub[right];
  targetSub[right] = targetSub[random];
  targetSub[random] = tmpSub;
  
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
    tmpSub = targetSub[i]; targetSub[i] = targetSub[j]; targetSub[j] = tmpSub;
  }
  // i indicates the position of the pivot.
  // Exchange target[i] and target[right].
  tmp = target[i];  target[i] = target[right];  target[right] = tmp;
  tmpSub = targetSub[i]; targetSub[i] = targetSub[right]; targetSub[right] = tmpSub;
  return i;
}

/**
 * random quick sort
 * by the values of target, sort 2 array
 */
void randomQSForTwoArray(int* target,int* targetSub, int aLeft, int right ) {
  int left = aLeft;
  while (left < right) {
    int i = partitionForTwoArray(target,targetSub,left,right);
    if( i - left <= right - 1){
      randomQSForTwoArray(target, targetSub, left, i - 1);
      left = i + 1;
    }else{
      randomQSForTwoArray(target, targetSub, i + 1, right);
      right = i - 1;
    }
  }
}

/**
 * construct suffix array by induced sorting for int array
 */
void constructSAInt(int * target, int n, int* sa){
  int i;
  int t[n]; // type of i-th char
  int rank[n];
  
  // init SA
  for (i = 0; i < n; i++) {
    sa[i] = -1;
  }
  sa[0] = n-1; // 0-th suffix is $
  
  // init type
  t[n-1] = S; // $ (end point of string) is S
  t[n-2] = L; // before $ is always L
  for (i = n-3; i >= 0; i--) {
    if (target[i] == target[i+1]) {
      t[i] = t[i+1];
    } else if (target[i] < target[i+1]) {
      t[i] = S;
    } else {
      t[i] = L;
    }
  }
  
  rank[n-1] = 0;
  for (i = 0; i < n - 1; i++) {
    rank[i] = target[i]+1;
  }
  
  // create buckets with length of max value of rank
  int max = getMax(rank,0, n);
  int l = max + 1;
  int buc[l];
  
  // bucket sort
  createBuckets(n, rank, buc, l);
  
  // sort LMS(LeftMost S-type)
  sortLMS(sa,t,rank,n,buc,l);

  // sort L type suffixes
  sortLTypeSuffixes(sa, t, rank, n, buc, l);

  // sort S type suffixes
  sortSTypeSuffixes(sa, t, rank, n, buc, l);
}

/**
* Construct suffix array by induced sorting
* target: pointer of input character array
* n: length of suffix array
* sa: pointer of suffix array
*/
void constructSAbyIS(char* target, int n, int* sa){
  int i;
  int t[n]; // type of i-th char
  int rank[n];
  
  // init SA
  for (i = 0; i < n; i++) {
    sa[i] = -2;
  }
  sa[0] = n-1; // 0-th suffix is $
  
  // init type
  t[n-1] = S; // $ (end point of string) is S
  t[n-2] = L; // before $ is always L
  for (i = n-3; i >= 0; i--) {
    if (target[i] == target[i+1]) {
      t[i] = t[i+1];
    } else if (target[i] < target[i+1]) {
      t[i] = S;
    } else {
      t[i] = L;
    }
  }
  
  // init rank by char code
  initRankByChar(target, rank, n);

  // create buckets with length of max(length of array, max value of rank)
  int max = (int)getLarger((double)n,  (double)getMax(rank,0, n));
  int l = max + 1;
  int buc[l];

  // bucket sort
  createBuckets(n, rank, buc, l);
  
  // sort LMS(LeftMost S-type)
  sortLMS(sa,t,rank,n,buc,l);

  // sort L type suffixes
  sortLTypeSuffixes(sa, t, rank, n, buc, l);

  // sort S type suffixes
  sortSTypeSuffixes(sa, t, rank, n, buc, l);

  // print the result
  printf("\nprint suffix array by IS\n");
  for (int i = 0 ; i < n; i++) {
    if (sa[i] < n-1 ){
      printf("%d:%s\n",sa[i],target+sa[i]);
    } else {
      printf("%d:$\n",sa[i]);
    }
  }
  printf("print suffix array end\n");
}

/**
 * Construct suffix array by Larson Sadakane algorithm
 * target: pointer of input character array
 * n: length of suffix array
 * sa: pointer of suffix array
 */
void constructSAbyLS(char* target, int n, int* sa){
  int rank[n]; // rank when word from target[i] to k-th are sorted
  int i;
  int isa[n]; // inversed suffix array
  
  // init SA and ISA
  for (i = 0; i < n; i++) {
    isa[i] = 0;
    sa[i] = i;
  }

  // init rank by char code
  initRankByChar(target, rank, n);
  
  // create buckets with length of max(length of array, max value of rank)
  int max = (int)getLarger((double)n,(double) getMax(rank,0, n));
  int l = max + 1;
  int buc[l];
  int buc_next[l];
  
  // bucket sort
  createBuckets(n, rank, buc, l);
  // sort sa by rank
  sortByRank(sa, 0, n, rank, buc, l);

  // init inversed suffix array
  for (i = 0; i < n; i++) {
    isa[ i ] = buc[ rank[i] ];
  }
  

  bool hasSame = true;
  int k;
  k = 1;
  // sorting until ISA array doesn't have the same value
  while (hasSame) {
    int left = 0;
    int right = 0;
    hasSame = false;
    for (i = 0; i < l; i++) {
      right = buc[i];
      if (right - left < 2) { // means already sorted
        left = right;
        continue;
      }
      constructSASub(sa, isa, left, right, k, n, l);
      left = right;
      hasSame = true; // if sorting is never executed, it's done
    }
    createBuckets(n, isa, buc, l);
    k = k * 2; // can skip next k chars
  }

  // print the result
  printf("\nprint suffix array by LS\n");
  for (int i = 0 ; i < n; i++) {
    if (sa[i] < n-1 ){
      printf("%d:%s\n",sa[i],target+sa[i]);
    } else {
      printf("%d:$\n",sa[i]);
    }
  }
  printf("print suffix array end\n");
}

/**
 * sort L type suffixes
 * buc: buckets
 * l: length of buc
 */
void sortLTypeSuffixes(int*sa, int*t, int* rank, int n, int* buc, int l){
  int i;
  int cursor[l];
  for (i = 0; i < l; i++) {
    cursor[i] = 0;
  }
  // sort L type suffixes
  for (i = 0; i < n; i++) {
    int v = sa[i] - 1;
    if ( v < 0 || t[v] == S) { continue; }
    sa[ buc[rank[v]-1] + cursor[rank[v]]++ ] = v;
  }
}

/**
 * sort S type suffixes
 * buc: buckets
 * l: length of buc
 */
void sortSTypeSuffixes(int*sa, int*t, int* rank, int n, int* buc, int l){
  int i;
  int cursor[l];
  for (i = 0; i < l; i++) {
    cursor[i] = 0;
  }
  // sort S type suffixes
  for (i = n-1; i >= 0; i--) {
    int v = sa[i] - 1;
    if ( v < 0 || t[v] == L) { continue; }
    sa[ buc[rank[v]]-1 + cursor[rank[v]]-- ] = v;
  }
}

/**
 * set equality for next to sa[i_v]
 * v == sa[i] - 1
 * sa[i_v] == v
 * ineq: array of equality
 */
void setEquality(int* sa, int* rank, int* ineq, int i, int i_v, int v, int n){

  int s;
  bool l_eq = false;
  bool r_eq = false;
  bool l_done = false;
  bool r_done = false;
  int l_v = sa[i_v - 1];
  int r_v;
  if (i_v == n-1) { // right of right end doesn't exist
    r_v = -2;
  } else {
    r_v = sa[i_v + 1];
  }
  
  // if next to i_v is empty, equality cannot be set
  if (l_v == -2){
    l_done = true;
  }
  if (r_v == -2){
    r_done = true;
  }
  
  // if the rank is different, LESS
  if (rank[v] != rank[l_v]) {
    ineq[i_v - 1] = LESS;
    l_done = true;
  }
  if (rank[v] != rank[r_v]) {
    ineq[i_v] = LESS;
    r_done = true;
  }
  // if both side is set, skip the following
  if (r_done && l_done) {
    return;
  }
  
  // for left side of s[i]
  for (s = i-1; s >= 0; s--) {
    // if next to s[i] is LESS, it's LESS
    if (ineq[s] == LESS) { break; }
    // if next to s[i]-1 is next to s[i_v], it's EQUAL
    if (sa[s]-1 == r_v) {
      r_eq = true;
    }
    if (sa[s]-1 == l_v) {
      l_eq = true;
    }
  }
  
  // for right side of s[i]
  for (s = i; s < n; s++) {
    // if next to s[i] is LESS, it's LESS
    if (ineq[s] == LESS) { break; }
    // if next to s[i]-1 is next to s[i_v], it's EQUAL
    if (sa[s+1]-1 == r_v) {
      r_eq = true;
    }
    if (sa[s+1]-1 == l_v) {
      l_eq = true;
    }
  }

  // set equality
  if (r_eq) {
    ineq[i_v] = EQUAL;
  } else {
    ineq[i_v] = LESS;
  }
  if (l_eq) {
    ineq[i_v-1] = EQUAL;
  } else {
    ineq[i_v-1] = LESS;
  }
}

/**
 * sort LMS
 * buc: buckets
 * l: length of buckets
 */
void sortLMS(int* sa,int* t,int* rank,int n,int* buc,int l){
  int i;
  int ineq[n];
  int cursor[l];
  for (i = 0; i < l; i++) {
    cursor[i] = 0;
  }
  for (i = 0; i < n; i++) {
    ineq[i] = LESS;
  }
  
  // Set LMS temporarily
  for (i = 1; i < n; i++) { // i=1: left end cannot be LMS
    if (t[i] == L || t[i-1] == S) { continue; } // if not LMS, continue
    sa[buc[rank[i]]-1 + cursor[rank[i]]] = i;
    // if the right is LMS, it's equal
    if (cursor[rank[i]] < 0) {
      ineq[buc[rank[i]]-1 + cursor[rank[i]]] = EQUAL;
    } else {
      ineq[buc[rank[i]]-1 + cursor[rank[i]]] = LESS;
    }
    cursor[rank[i]]--;
  }
  
  // sort L type suffixes
  for (i = 0; i < l; i++) {
    cursor[i] = 0;
  }
  for (i = 0; i < n; i++) {
    if (sa[i] <= 0) {continue;} // before 0 doesn't exist and <0 means empty
    int v = sa[i] - 1;
    if ( v < 0 || t[v] == S) { continue; }
    int i_v = buc[rank[v]-1] + cursor[rank[v]]; // get positon for v
    sa[ i_v ] = v;
    cursor[rank[v]]++;
    setEquality(sa, rank, ineq, i, i_v, v, n);
  }
  
  // sort S-type
  for (i = 0; i < l; i++) {
    cursor[i] = 0;
  }
  for (i = n-1; i >= 0; i--) {
    if (sa[i] <= 0) {continue;} // before 0 doesn't exist and <0 means empty
    int v = sa[i] - 1;
    if ( v < 0 || t[v] == L) { continue; }
    int i_v = buc[rank[v]]-1 + cursor[rank[v]]; // get positon for v
    sa[ i_v ] = v;
  
    cursor[rank[v]]--;
    setEquality(sa, rank, ineq, i, i_v, v, n);
  }
  
  // if EQUAL is exist between LMS, construct SA only for LMS
  // else, set sorted LMS
  // init cursor and temporary SA
  for (i = 0; i < l; i++) {
    cursor[i] = 0;
  }
  int sa_tmp[n];
  for (i = 0; i < n; i++) {
    sa_tmp[i] = -1;
  }
  sa_tmp[0] = n - 1; // 0-th suffix is $
  
  bool has_equal = false;
  int n_lms = 1;
  
  for (i = n-1; i > 0; i--) { // i > 0:0-th suffix is $
    if (sa[i] == 0){continue;} // if left end of string, it's not LMS
    if (t[sa[i]] == L || t[sa[i]-1] == S) {
      continue; } // if not LMS, continue
    
    n_lms++;
    
    // if at least one is LMS and sign is equal, sort LMS substrings
    if ( (ineq[i-1] == EQUAL && t[sa[i-1]] == S && t[sa[i-1]-1]==L) ||
        (ineq[i] == EQUAL  && t[sa[i+1]] == S && t[sa[i+1]-1]==L) ) {
      has_equal = true;
    }

    // put sa[i] to the correct position
    sa_tmp[ buc[rank[sa[i]]] -1 + cursor[rank[sa[i]]]-- ] = sa[i];
  }
  
  // if EQUAL is exist between LMS, construct SA only for LMS
  if (has_equal) {
    // sort LMS substrings
    // variable for SA of LMS
    int sa_lms[n_lms];
    int rank_lms[n_lms];
    
    // init SA and rank
    // if next is EQUAL, don't count up rank
    sa_lms[0] = n-1;
    rank_lms[0] = 0;
    int r = 0;
    int c_lms = 1;
    for (i = 1; i < n; i++) {
      if (t[sa[i]] == L || t[sa[i]-1] == S) { continue; }
      if (ineq[i-1] == LESS) {
        r++;
      }
      sa_lms[c_lms] = sa[i];
      rank_lms[c_lms] = r;
      c_lms++;
    }

    // sort SA and rank by SA
    randomQSForTwoArray(sa_lms, rank_lms, 0,n_lms-1);
    
    // construct SA of rank
    int rank_lms_sa[n_lms+1];
    constructSAInt(rank_lms, n_lms+1, rank_lms_sa);
    
    // sort SA as SA of rank
    int sa_lms_tmp[n_lms];
    for (i = 0; i < n_lms; i++) {
      sa_lms_tmp[i] = sa_lms[ rank_lms_sa[i+1] ];
    }
    for (i = 0; i < n_lms; i++) {
      sa_lms[i] = sa_lms_tmp[i];
    }

    for (i = 0; i < l; i++) {
      cursor[i] = 0;
    }
    for (i = n_lms-1; i > 0; i--) { // i > 0:0-th suffix is $
      // put sa[i] to the correct position
      sa_tmp[ buc[rank[sa_lms[i]]] -1 + cursor[rank[sa_lms[i]]]-- ] = sa_lms[i];
    }
  }

  // copy temporary SA to SA
  for (i = 0; i < n; i++ ) {
    sa[i] = sa_tmp[i];
  }
  
}

/**
 * init rank by char code
 * n: length of rank
 */
void initRankByChar(char* target, int* rank, int n){
  // ending code '$' is -1
  for (int i = 0; i < n ; i++) {
    if ( i < n-1 ) {
      rank[i] = target[i]+1;
    } else {
      rank[i] = 0;
    }
  }
}


/*
 * set inversed suffix array
 * sa: suffix array
 * isa: inversed suffix array
 * left: left end of sa changed
 * right: right end of sa changed
 * k: use next k-th ISA as rank
 * n_sa: length of SA
 */
void constructSASub(int* sa, int* isa, int left, int right,int k, int n_sa, int l){
  int i;
  int n = right - left;
  int rank[n];
  
  // calculate rank from ISA
  for (i = 0; i < n; i++) {
    if ( sa[i+left] + k >= n_sa ) {
      rank[i] = 0;
    } else {
      rank[i] = isa[ sa[i+left] + k];
    }
  }
  
  int buc_use[l];
  createBuckets(n, rank, buc_use, l);

  // set ISA before sorting so that orders of rank and SA matched
  setISA(sa, isa, buc_use, rank, left, right);
  sortByRank(sa, left, right, rank, buc_use, l);
}

/*
 * create buckets for sorting from rank
 * n: length of rank
 * rank: value to eval
 * buc: buckets
 * l: number of buckets
 */
void createBuckets(int n, int* rank, int*buc, int l){
  int i;
  // init bucket
  for (i = 0; i < l; i++) {
    buc[i] = 0;
  }
  // create bucket
  for (i = 0; i < n; i++) {
    buc[ rank[i] ]++;
  }
  for (i = 1; i < l; i++) {
    buc[i] = buc[i-1] + buc[i];
  }
}




/*
 * Sort suffix array by bucket sort
 * sa: pointer of suffix array
 * left: left end of suffix array to sort
 * right: right end of suffix array to sort
 * rank: pointer of rank to eval
 * buc: buckets already counted
 * l: number of buckets
 */
void sortByRank(int* sa, int left, int right, int* rank, int* buc, int l){
  int n = right - left;
  int i;
  int buc_use[l];

  for (i = 0; i < l; i++) {
    buc_use[i] = buc[i];
  }
  
  // sort by bucket
  int temp[n];
  for (i = 0; i < n; i++) {
   temp[ -1 + buc_use[rank[i]]-- ] = sa[left + i];
  }
  for (i = 0; i < n; i++) {
    sa[left + i] = temp[i];
  }
}

/*
 * set inversed suffix array
 * sa: suffix array
 * isa: inversed suffix array
 * buc: buckets
 * rank: rank
 * left: left end of sa changed
 * right: right end of sa changed
 */
void setISA(int* sa, int* isa, int* buc, int* rank,int left,int right){
  int i;
  int n = right - left;
  for (i = 0; i < n; i++) {
    isa[ sa[i+left] ] += buc[ rank[i] ] - n;
  }
}

#endif

//int main(int argc, const char * argv[]) {
//  int i = 0;
//  init_genrand((unsigned long) time(NULL));
//  
//  // init string
//  char seed[4] = {'A','T','G','C'};
//  int n = 100;
//  char* target = (char*)malloc(sizeof(char) * n );
//  for (i = 0 ; i < n; i++) {
//    target[i] = seed[genrand_int32()%4];
//  }
//  
//  // TODO:
//  // check if any letters except alphabets exists
//  // convert lowercase to uppercase
//  
//  int suffixArr1[n+1];
//  int suffixArr2[n+1];
//  
//  constructSAbyIS(target, n+1, suffixArr1);
//  constructSAbyLS(target, n+1, suffixArr2);
//  
//  bool ng = false;
//  for (int i = 0; i < n+1; i++) {
//    if (suffixArr1[i] != suffixArr2[i]) {
//      ng = true;
//    }
//  }
//  if (ng) {
//    printf("////////////////////NG///////////////////\n");
//  }else {
//    printf("////////////////////OK///////////////////\n");
//  }
//  
//  free(target);
//  return 0;
//}