/*
 * utils.c
 *
 *  Created on: Dec 26, 2015
 *      Author: Karl
 */

#include <Utils/utils.h>

char stopChar(char value);

unsigned int strLen(char *value)
{
	unsigned int i = 0;

	while ((i < 512) && (!stopChar(value[i])))
		i++;

	return i;
}

char stopChar(char value)
{
	return (value == EOS);
}

// que pasa si encuentro mas de los que estan dimensionados en la respuesta

int find(char *text, char *word, int *rtas)
{
	unsigned int indexRtas = 0;
	unsigned int index = 0;
	unsigned int equals = 0;
	unsigned int wordLen = strLen(word);
	unsigned char count = 0;
	//unsigned int textLen = length(text);

	rtas[indexRtas] = EOIL;
	while ((!stopChar(text[index])))
	{
		equals = (word[equals] == text[index])? equals+1 : 0;
		if (equals == wordLen)
		{
			rtas[indexRtas] = index-wordLen+1;
			equals = 0;
			indexRtas++;
			count++;
			rtas[indexRtas] = EOIL;
		}
		index++;
	}

	return indexRtas;
}

// return char len
int strsub(char *text, int init, int end, char *rta)
{
	int index = 0;
	if (init > end) return 0;
	while ((init+index <= end) && (text[init] != EOS))
	{
		rta[index] = text[init+index];
		index++;
	}
	rta[index] = EOS;

	return index;
}

/*!
  \brief Converts a 16bit signed number into a string.
  \param[in,out] dst String buffer to store the number.
  \param[in] dstSize Size of the destination buffer in uint8_ts.
  \param[in] val 16bit signed number to convert.
 */
void Num16sToStr(char *dst, int dstSize, int val)
{
  unsigned char *ptr =  ((unsigned char *)dst);
  unsigned char i=0, j;
  unsigned char tmp;
  unsigned char sign = (unsigned char)(val < 0);

  if (val==(int)(0x8000)) { /* special case 0x8000/-32768: prevent overflow below. */
    strcpy(dst, (unsigned char*)"-32768");
    return;
  }
  dstSize--; /* for zero uint8_t */
  if (sign) {
    val = -val;
  }
  if (val == 0 && dstSize > 0){
    ptr[i++] = '0';
    dstSize--;
  }
  while (val > 0 && dstSize > 0) {
    ptr[i++] = (unsigned char)((val % 10) + '0');
    dstSize--;
    val /= 10;
  }
  if (sign && dstSize > 0){
    ptr[i++] = '-';
    dstSize--;
  }
  for(j=0; j<(i/2); j++) { /* swap buffer */
    tmp = ptr[j];
    ptr[j] = ptr[(i-j)-1];
    ptr[(i-j)-1] = tmp;
  }
  ptr[i] = '\0';
}

