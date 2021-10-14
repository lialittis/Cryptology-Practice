/**************************************************************/
/* Geffe.c                                                    */
/* Author : Alain Couvreur                                    */
/* alain.couvreur@lix.polytechnique.fr                        */
/* Last modification September 24, 2018                       */
/**************************************************************/

#include <stdio.h>
#include <math.h>
#include "buffer.h"
#include "bits.h"
#include "LFSR.h"
#include "Geffe.h"
#define DEBUG 1


void Geffe(buffer_t *output, buffer_t *s1, buffer_t *s2, buffer_t *s3){
    buffer_reset(output);
	/*Initialization*/
    buffer_append(output,s1);
	/*Compute the stream*/
	uchar *state = output->tab;
	for(int i = 0; i < output->length; i++,state++){
	    *state = (*state & (s2->tab[i]))^(s2->tab[i]&s3->tab[i])^(s3->tab[i]);
	}
}


double correlation(buffer_t *s1, buffer_t *s2){
	double count = 0.0;
	for(int i = 0; i < s1->length; i++){
	    uchar current_char = s1->tab[i];
		uchar current_char_2 = s2->tab[i];
		for(int j = 0; j < BYTE_SIZE; j++){
		    uchar x = getBit(current_char,j);
			uchar y = getBit(current_char_2,j);
			if(x==y){
			    count +=1.0;
			}
		}
	}
    double correlation = count/(s1->length*BYTE_SIZE);

	return correlation;
}


void searchIV(buffer_t *IV_candidate, buffer_t *stream, buffer_t *trans, double threshold){
	buffer_t stream_candidate;
	buffer_init(&stream_candidate, trans->length);
	buffer_reset(IV_candidate);
	for(int i = 0; i < trans->length; i++)
		buffer_append_uchar(IV_candidate, 0);
    LFSR(&stream_candidate,trans,IV_candidate,stream->length);
	while(correlation(&stream_candidate,stream)<threshold){
		increment_buffer(IV_candidate);
		LFSR(&stream_candidate,trans,IV_candidate,stream->length);
	}
        	
	buffer_clear(&stream_candidate);
}


void positions(buffer_t *output, buffer_t *s1, buffer_t *s3){
	if(s1->length != s3->length){
		perror("Input streams should have the same length.\n");
		return;
	}
	buffer_reset(output);
	uchar *state = output->tab;
	for(int i = 0; i < s1->length; i++,state++){
		uchar char1 = s1->tab[i];
		uchar char3 = s3->tab[i];
	    for(int j = 0; j < BYTE_SIZE ; j++){
			if(getBit(char1,j)==1 && getBit(char3,j)==0){
				setBit(*state,j,1);
			}else{
			    setBit(*state,j,0);
			}
		}
	}
}

int match_at(buffer_t *s, buffer_t *s1, buffer_t *pos){
	if(s->length != s1->length || s->length != pos->length){
		perror("Input buffers should have the same lengths\n");
		return 0;
	}
    for(int i = 0; i< pos->length; i++){
	    uchar indicator = pos->tab[i];
        uchar char0 = s->tab[i];
		uchar char1 = s->tab[i];
		for(int j = 0; j< BYTE_SIZE; j++){
		    if(getBit(indicator,j)==1 && getBit(char0,j)!=getBit(char1,j)){
				return 0;
			}
		}
	}
	return 1;
}


void search_with_match(buffer_t *IV_candidate, buffer_t *stream,
					   buffer_t *trans, buffer_t *pos){
	buffer_t stream_candidate;
	buffer_init(&stream_candidate, pos->length);
	buffer_reset(IV_candidate);
	for(int i = 0; i < trans->length; i++)
		buffer_append_uchar(IV_candidate, 0);
	LFSR(&stream_candidate,trans,IV_candidate,stream->length);
	while(match_at(&stream_candidate,stream,pos)==0){
		increment_buffer(IV_candidate);
		LFSR(&stream_candidate,trans,IV_candidate,stream->length);
	}
	
	buffer_clear(&stream_candidate);
}


void attack(buffer_t *IV_candidate1, buffer_t *IV_candidate2,
			   buffer_t *IV_candidate3, buffer_t *stream,
			   buffer_t *trans1, buffer_t *trans2, buffer_t *trans3,
			   double threshold){

    searchIV(IV_candidate1,stream,trans1,threshold);
	searchIV(IV_candidate2,stream,trans2,threshold);
	searchIV(IV_candidate3,stream,trans3,threshold);
}
/*
1. The complexity of a brute force attack on Geffe's generator:  O(256^(l1 + l2 + l3 - 3))

2. The complexity of the above attack: It depends on the length of transition vector, 
   normally, the complexity is O(256^n), where n is the length of transition vector.

3. The complexity of the attack when using linear algebra to guess the second LFSR: O(n^2) 

4. When the length of LFSR's is about 30 bits, there is almost no way to attack the stream by a brute force attack. The interest for me is the length and content of the transition vector.

*/



