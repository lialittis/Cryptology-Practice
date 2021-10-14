/**************************************************************/
/* LFSR.c                                                     */
/* Author : Alain Couvreur                                    */
/* alain.couvreur@lix.polytechnique.fr                        */
/* Last modification September 30, 2020 (FJM)                 */
/* Last modification September 24, 2018                       */
/**************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "buffer.h"
#include "bits.h"
#include "LFSR.h"
#define DEBUG 1


uchar weightMod2(uchar c){
    uchar result = 0;
    for(int j = 0; j < BYTE_SIZE; j++)
	result ^= 1 & (c >> j);
    return result;
}



void LFSR(buffer_t *stream, buffer_t *trans, buffer_t *IV, int stream_length){
    if(IV->length != trans->length){
	perror(
	       "ERROR : IV and transition vector should have the same length\n");
	return;
    }

    buffer_reset(stream);
    uchar current_bit;
	
    /* Initialization */
    buffer_append(stream, IV);
		
	
    /* Computation of the stream */
    uchar *state = stream->tab;
    uchar *cursor_trans;
    uchar *cursor_state;
    for(int i = trans->length; i < stream_length; i++, state++){
	/* Initialize the next octet to fill in */
	buffer_append_uchar(stream, 0);
	for(int j = 0; j < BYTE_SIZE; j++){
	    /* Construct bit-wise the next octet*/
	    cursor_trans = trans->tab;
	    cursor_state = state;
	    current_bit = 0; /* This is the "bit" we will compute in this iteration */

	    for(int k = 0; k < trans->length; k++, cursor_trans++,
		    cursor_state++){
		current_bit ^= weightMod2( (*cursor_state << j)  & *cursor_trans );
		current_bit ^= weightMod2( *(cursor_state + 1) &
					   ( *cursor_trans << (BYTE_SIZE - j) ));
	    }
	    *(state + trans->length) ^= current_bit << (BYTE_SIZE - 1 - j);
	}
    }
}



void increment_buffer(buffer_t *buf){
	for(int i= buf->length-1; i>=0; i--){
	    if(buf->tab[i]<255){
	        buf->tab[i]++;
			return;
	    }else{
		    buf->tab[i]=0;
		}
	}
}




void bourrinate_IV(buffer_t *searched_IV, buffer_t *trans, buffer_t *stream){
    buffer_t stream_candidate;
    buffer_init(&stream_candidate, stream->length);
    buffer_reset(searched_IV);
    for(int i = 0; i < trans->length; i++)
	buffer_append_uchar(searched_IV, 0);

    // Complete the function...
	LFSR(&stream_candidate,trans,searched_IV,stream->length);
    while(!(buffer_equality(&stream_candidate,stream))){
		increment_buffer(searched_IV);
		LFSR(&stream_candidate,trans,searched_IV,stream_candidate.length);
	}
    
	buffer_clear(&stream_candidate);
}
