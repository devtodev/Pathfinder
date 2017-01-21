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

