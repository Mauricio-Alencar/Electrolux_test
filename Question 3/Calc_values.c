/**
 * @file Calc_values.c
 * @author José Maurício Alencar Filho
 * @brief Example to solve question 3 in electrolux test
 * @version 0.1
 * @date 2022-07-25
 * 
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>

//struct to get values min, max, avarage
struct xCalc_value
{
  int32_t lMin;
  int32_t lMax;
  int32_t lAvarage;
}; 

//function to take all values in variables and pointers
struct xCalc_value xGetValues(int32_t * lArray, int32_t n, int32_t * lPair_array, int32_t * lNew_length)
{
  struct xCalc_value values;    
  int32_t i;
  int32_t lSum = 0, lResult =0;
  
  //logic to get maximum and minimum values
  if (n == 1)
  {
     values.lMax = lArray[0];
     values.lMin = lArray[0];    
     return values;
  }   
 
  if (lArray[0] > lArray[1]) 
  {
      values.lMax = lArray[0];
      values.lMin = lArray[1];
  } 
  else
  {
      values.lMax = lArray[1];
      values.lMin = lArray[0];
  }   
 
  for (i = 2; i<n; i++)
  {
    if (lArray[i] >  values.lMax)     
      values.lMax = lArray[i];
   
    else if (lArray[i] <  values.lMin)     
      values.lMin = lArray[i];
  }
  
  //logic to get avarage value
  for(i=0; i<n; i++){
      lSum = lSum + lArray[i];
  }
   lResult = lSum/n;
   values.lAvarage = lResult;
   
  //logic to get pair numbers into a new int vet
  int32_t lAux_length=0;
  
  for(i=0; i<n; i++){
     if(lArray[i] % 2 == 0)
     {
        lPair_array[lAux_length] = lArray[i];
        lAux_length++;
     }
  }
  *lNew_length = lAux_length;
  
  return values;
}

int main()
{
    //initializate vector with values
    int32_t lArray[] = {35, 50, 100, 3, 1000, 350, 79};
    //get dynamic length of vector in compilation 
    int32_t lArr_size = sizeof(lArray)/sizeof(lArray[0]), lPair_length = 0;
    
    //initializate array of pair values and zero values
    int32_t lArray_pair[lArr_size];
    memset(lArray_pair, 0, lArr_size*sizeof(lArray_pair[0]));
    
    //calculate all values like a min value, max value, avarage, pair vector and length of pair vector.
    struct xCalc_value xCalc_Values = xGetValues (lArray, lArr_size, lArray_pair, &lPair_length);
    
    //print all values
    printf("Min is %d\r\n", xCalc_Values.lMin);
    printf("Max value is %d\r\n", xCalc_Values.lMax);
    printf("Avarage value is %d\r\n", xCalc_Values.lAvarage); 

    printf("New pair array length is %d and New values is:\r\n", lPair_length);
    for(int i = 0; i < lPair_length; i++)
    {
        printf("%d\r\n",lArray_pair[i]);
    }
    
    return 0;
}
