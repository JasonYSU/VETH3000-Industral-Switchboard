#include "string_utility.h"
#include<string.h>
int all_num = 0;
char *connectNumString(char* dest,  char*src,int start_loc,int num){
	char *cp;
	int i;
	//printf("pre char is %02X;start char is %02X;\n\r",dest[start_loc - 1],dest[start_loc]);
	cp = &dest[start_loc];
//	for(i = 0; i < num; i++){
//		*cp++ = *src++;
//	}
	memmove(cp,src,num);
	//cp[num] = 0;
	return dest;
}
int compareNumString(const void * ptr1, const void * ptr2, size_t num )
{
	 int ret = 0;
	 int i;
	 char *p1,*p2;
	 const char* ptmp1 = (char*)ptr1;
	 const char* ptmp2 = (char*)ptr2;
//	 p1 = (char*)ptr1;
//	 p2 = (char*)ptr2;
	 //assert(ptr1 != NULL && ptr2 != NULL && num>0);
	 while(num--)
	 {
	  if(*ptmp1 > *ptmp2)
	  {
	   ret = 1;
	   break;
	  }
	  if(*ptmp1 < *ptmp2)
	  {
	   ret = -1;
	   break;
	  }
	  ptmp1++;
	  ptmp2++;
	 }
	 
	 return ret;
}
