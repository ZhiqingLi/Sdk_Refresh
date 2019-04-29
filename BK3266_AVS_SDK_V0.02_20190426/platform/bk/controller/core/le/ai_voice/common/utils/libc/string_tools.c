#include "driver_beken_includes.h"
#include "app_beken_includes.h"
#include "beken_external.h"
#include "string_tools.h"

char *lp_strcat(char *dst,const char *src) 
 {

     char *p=dst; 

     if(dst==NULL || src==NULL){
            os_printf("%s:dst or src is null!!!",__func__);
            return NULL;
     }

     while(*dst!='\0')dst++;
     while(*src!='\0')*dst++=*src++;

     *dst='\0';

    return p;  
}

char *lp_strncat(char *dest,const char *str,int n)
{
	char *cp=dest;

    if(dest==NULL || str==NULL){
		os_printf("%s:dst or src is null!!!",__func__);
		return NULL;
    }
 
	while(*cp!='\0') ++cp;
	while(n&&(*cp++=*str++)!='\0')
	{
		--n;
	}
 
	return dest;
}

int lp_strncmp (const char * s1, const char * s2, size_t n) 
{
	if ( !n )
		return(0);
	
	while (--n && *s1 && *s1 == *s2) 
	{ 
		s1++;
		s2++;
	}
	return( *s1 - *s2 );
}

