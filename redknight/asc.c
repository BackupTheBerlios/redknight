#include "includes.h"

          
void my_strcp(Uint8 *dest,const Uint8 * source)
{
	while(*source)
		{
			*dest++=*source++;
		}
	*dest='\0';
}

void my_strncp(Uint8 *dest,const Uint8 * source, Sint32 len)
{
	while(*source && --len > 0)
		{
			*dest++=*source++;
		}
	*dest='\0';
}

void my_strcat(Uint8 *dest,const Uint8 * source)
{
	int i,l,dl;
	l=strlen(source);
	dl=strlen(dest);
	for(i=0;i<l;i++)dest[dl+i]=source[i];
	dest[dl+i]='\0';
}

Sint32 my_strncompare(const Uint8 *dest, const Uint8 *src, Sint32 len)
{
	int i;
	Uint8 ch1,ch2;

	for(i=0;i<len;i++)
		{
			ch1=src[i];
			ch2=dest[i];
			if(ch1>=65 && ch1<=90)ch1+=32;//make lowercase
			if(ch2>=65 && ch2<=90)ch2+=32;//make lowercase
			if(ch1!=ch2)break;
		}
	if(i!=len)return 0;
	else return 1;
}

Sint32 my_strcompare(const Uint8 *dest, const Uint8 *src)
{
	Uint32 len;

	len=strlen(dest);
	if(len!=strlen(src))return 0;
	return(my_strncompare(dest, src, len));
}

// is this string more then one character and all alpha in it are CAPS?
Sint32 my_isupper(const Uint8 *src, int len)
{
	int alpha=0;
	if (len < 0)	len=strlen(src);
	if(!src || !src[0] || !src[1] || !src[2] || len == 0) return 0;
	while(*src && len > 0)
		{
			if(isalpha(*src)) alpha++;
			if((isdigit(*src)&&alpha<len/2) || *src != toupper(*src)) return 0;	//at least one lower
			src++;
			len--;
		}
	return 1;	// is all upper or all num
}
