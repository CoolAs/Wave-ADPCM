/****************************************************************
 * this example demonstrates streaming synthensized audio
 ****************************************************************/
 
#include <nds.h>
#include <fat.h>
#include <stdio.h>
#include <maxmod9.h>
#include "general.h"
enum {
	// blue backdrop
	bg_colour = 13 << 10,
	// red cpu usage
	cpu_colour = 31
};
FILE* f;
waveInfo w;
mm_word on_stream_request( mm_word length, mm_addr dest, mm_stream_formats format ) {
//----------------------------------------------------------------------------------
  s16* d = (s16*)dest;	
  int req = 0;
  while (req < length)
  {
    if (feof(f))
    {
			mmStreamClose();
			fclose(f);
      printf("Finished song!\n");
      return req; 
    }
    *d++ = (s16)getADCM(f,&w);
    req++;
  }
	return length;
}

/**********************************************************************************
 * main
 *
 * Program Entry Point
 **********************************************************************************/
int main( void ) {
//---------------------------------------------------------------------------------

	//----------------------------------------------------------------
	// print out some stuff
	//----------------------------------------------------------------
	consoleDemoInit();
	fatInitDefault();
	iprintf( "\nMaxmod ADPCM Streaming Example\n\n");
  f = fopen("wave.wav","r");
  if (f == NULL) iprintf("Could not open file 'wave.wav'\n");
  int ret =  parseWave(f,&w);
  iprintf("w->blockAlign = %d\nw->sampleRate = %d\n",w.blockAlign,w.sampleRate);
  if (ret) iprintf("Failed to read .wav file\n Err = %d\n",ret);



	//----------------------------------------------------------------
	// initialize maxmod without any soundbank (unusual setup)
	//----------------------------------------------------------------
	mm_ds_system sys;
	sys.mod_count 			= 0;
	sys.samp_count			= 0;
	sys.mem_bank			= 0;
	sys.fifo_channel		= FIFO_MAXMOD;
	mmInit( &sys );
	
	//----------------------------------------------------------------
	// open stream
	//----------------------------------------------------------------
	mm_stream mystream;
	mystream.sampling_rate	= w.sampleRate;					// sampling rate = loaded from wave file
	mystream.buffer_length	= 1200;						// buffer length = 1200 samples
	mystream.callback		= on_stream_request;		// set callback function
	mystream.format			= MM_STREAM_16BIT_MONO;	// format = mono 16-bit
	mystream.timer			= MM_TIMER0;				// use hardware timer 0
	mystream.manual			= true;						// use manual filling
	mmStreamOpen( &mystream );	
	SetYtrigger( 0 );
	irqEnable( IRQ_VCOUNT );
	
	while( 1 )
	{
		// wait until line 0
		swiIntrWait( 0, IRQ_VCOUNT);
		
		// update stream
		mmStreamUpdate();
		
		// restore backdrop (some lines were drawn with another colour to show cpu usage)
		BG_PALETTE_SUB[0] = bg_colour;
		
		// wait until next frame
		swiWaitForVBlank();
		
		// set backdrop to show cpu usage
		BG_PALETTE_SUB[0] = cpu_colour;
	}
	
	return 0;
}
