#include <stdio.h>
#include <fftw3.h>
#include <cmath>
#include <vector>
#include <iostream>
#include <sndfile.h>

#define PI 3.14159265
#define BUFFER_LEN 1024

using namespace std;

void hamming( int window_length, float *buffer ) {
	for( int i = 0; i < window_length; i++ ) {
		buffer[i] = 0.54 - ( 0.46 * cos( 2 * PI * (i / (window_length - 1.0) ) ) );
	}
}

void stft( vector<float> &signal, long signal_length, int window_size, int hop_size ) {
	fftw_complex *data, *fft_result, *ifft_result;
	fftw_plan plan_forward, plan_backward;
	
	data = (fftw_complex*) fftw_malloc( sizeof( fftw_complex ) * window_size );
	fft_result = (fftw_complex*) fftw_malloc( sizeof( fftw_complex ) * window_size );
	ifft_result = (fftw_complex*) fftw_malloc( sizeof( fftw_complex ) * window_size );
	
	plan_forward = fftw_plan_dft_1d( window_size, data, fft_result, FFTW_FORWARD, FFTW_ESTIMATE );
	
	float window[window_size];
	hamming( window_size, window );
	
	long chunk_pos = 0;
	long read_index;
	
	int stop = 0;
	
	long num_chunks = 0;
	
	while( chunk_pos < signal_length && !stop ) {
		for( int i = 0; i < window_size; i++ ) {
			read_index = chunk_pos + i;
			if( read_index < signal_length ) {
				data[i][0] = signal[read_index] * window[i];
				data[i][1] = 0;
			} 
			else {
				data[i][0] = 0;
				data[i][1] = 0;
				stop = 1;	
			}
		}
	
		fftw_execute( plan_forward );
	
		cout << "Column: " << chunk_pos << endl;
		for( int i = 0; i < window_size; i++ ) {
			printf("fft_result[%d] = { %.2f, %.2f }\n", i, fft_result[i][0], fft_result[i][1]);
		}
		
		chunk_pos += hop_size;
		num_chunks++;
	}
	
	fftw_destroy_plan( plan_forward );
	
	fftw_free( data );
	fftw_free( fft_result );
	fftw_free( ifft_result );
}

int main() {
	vector<float> signal;
	long signal_length = 0;
	
   static double data[BUFFER_LEN];
   SNDFILE *infile;
   SF_INFO sfinfo;
   int readcount;
   const char *infilename = "helloworld.wav";
   
   if ( !( infile = sf_open( infilename, SFM_READ, &sfinfo ) ) ) {
   	printf ( "Not able to open input file %s.\n", infilename );
   	puts ( sf_strerror (NULL) );
   	return 1;
	}

	while ( readcount = sf_read_double( infile, data, BUFFER_LEN ) ) {
		signal_length += readcount;
		for( int i = 0; i < readcount; i+= sfinfo.channels ) {
			signal.push_back( (float)data[i] );
		}
	}
	
   sf_close( infile );
	stft( signal, signal_length, 1024, 512 );
   return 0 ;
}
