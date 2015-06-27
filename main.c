#include <stdio.h>
int ima_step_table[89] = { 
		7,	  8,	9,	 10,   11,	 12,   13,	 14,
	   16,	 17,   19,	 21,   23,	 25,   28,	 31,
	   34,	 37,   41,	 45,   50,	 55,   60,	 66,
	   73,	 80,   88,	 97,  107,	118,  130,	143,
	  157,	173,  190,	209,  230,	253,  279,	307,
	  337,	371,  408,	449,  494,	544,  598,	658,
	  724,	796,  876,	963, 1060, 1166, 1282, 1411,
	 1552, 1707, 1878, 2066, 2272, 2499, 2749, 3024,
	 3327, 3660, 4026, 4428, 4871, 5358, 5894, 6484,
	 7132, 7845, 8630, 9493,10442,11487,12635,13899,
	15289,16818,18500,20350,22385,24623,27086,29794,
	32767
};
int ima_index_table[16] = {
  -1, -1, -1, -1, 2, 4, 6, 8,
  -1, -1, -1, -1, 2, 4, 6, 8
}; 
int signed_nibble_conversion[16] = {
  0, 1, 2, 3, 4, 5, 6, 7,
  -8, -6, -5 ,-4 ,-3, -2, -1
};
bool get;
char n;
int getNibble()
{
  if (get)
  {
    n = getchar();
    get = false;
    return n & 0x0f;

  }
  else
  {
    get = true;
    return (n & 0xF0) >> 4;
  }
}
bool require(char c)
{
  if (getchar()!=c) return true;
  return false;
}
bool findfmt()
{
  while (getchar()!='f'){
    if (feof(stdin)) return true;
  }
  if (getchar()!='m') findfmt();
  if (getchar()!='t') findfmt();
  if (getchar()!=' ') findfmt();
}
bool finddata()
{
  while (getchar()!='d'){
    if (feof(stdin)) return true;
  }
  if (getchar()!='a') finddata();
  if (getchar()!='t') finddata();
  if (getchar()!='a') finddata();
}
int main()
{
  get = true;
  //Decompresses mono ADPCM wav files, and does not check to see that the wav is of this type.
  int predictor = 0;
  int step_index = 0;
  int step = 0;
  //Parse the .wav header
  if (require('R')) return -1;
  if (require('I')) return -1;
  if (require('F')) return -1;
  if (require('F')) return -1;
  //Ignore the size
  for (int i = 0; i < 4; ++i)
    getchar();
  if (require('W')) return -1;
  if (require('A')) return -1;
  if (require('V')) return -1;
  if (require('E')) return -1;
  if (findfmt()) return -1;
  //Ignore the size
  for (int i = 0; i < 4; ++i)
    getchar();
  if (require(0x11)) return -2;
  if (require(0x00)) return -2;  
  if (require(0x01)) return -3;
  if (require(0x00)) return -3;
  //Ignore the size
  for (int i = 0; i < 8; ++i)
    getchar();
  int blockSize = getchar();
  blockSize |= getchar()<<8;
  if (finddata()) return -4;
  //Ignore the size
  for (int i = 0; i < 4; ++i)
    getchar();
  fprintf(stderr,"Starting conversion\nblockSize = %d\n",blockSize);

  //Now to actual conversion
  int reset = 0;
  while (!feof(stdin))
  {
    int nibble = 0;
    if (reset == 0 )
    {
      reset = blockSize*2;
      getchar();
      getchar();
      predictor = 0;
      step_index = getchar();
      step = 0;     
      getchar();
      putchar(predictor&0xff);
      putchar((predictor&0xFF00)>>8);
      reset-=8;
    }
    else 
    {
      nibble = getNibble();
      step = ima_step_table[step_index];
      
      int diff = step >> 3;    
    	if(nibble&4)
    		diff += step;
    	if(nibble&2)
    		diff += step>>1;
    	if(nibble&1)
    		diff += step>>2;
      if (nibble&8) predictor -= diff;
      else predictor += diff ;
      if (predictor > 32767) predictor = 32767;
      else if (predictor < -32768) predictor = -32768;
      putchar(predictor&0xFF);
        putchar((predictor&0xFF00)>>8);
      step_index += ima_index_table[nibble&7];
      if (step_index > 88) step_index = 88;
      if (step_index < 0) step_index = 0;
      reset-=1;
    }
  }
}
