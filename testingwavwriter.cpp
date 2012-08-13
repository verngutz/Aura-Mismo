#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <cmath>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>
#include <fftw3.h>
#include <sndfile.h>

#define PI 3.14159265
#define BUFFER_LEN 1024
#define TESTING_FILE "testing.txt"
#define WAVE_FILE "CMajorScale.wav"
#define NUM_PEAKS 20

using namespace std;

void hamming( int window_length, float *buffer );
void stft( vector<float> &signal, long signal_length, int window_size, int hop_size, ofstream &testing_file );

int main() {

	cout << "Begin writing testing data" << endl;
	
	// open testing file for writing
	ofstream testing_file;
	testing_file.open(TESTING_FILE);
   
   // populate testing file with stft data from testing wav file
   
	cout << "Processing WAV file: " << WAVE_FILE;
		
	// open wav file
	SNDFILE *infile;
	SF_INFO sfinfo;
	if ( !( infile = sf_open( WAVE_FILE, SFM_READ, &sfinfo ) ) ) {
		printf( "Not able to open input file %s.\n", WAVE_FILE );
		puts( sf_strerror( NULL ) );
		return 1;
	}
	
	// read wav data into buffer and push buffered data into signal vector
	vector<float> signal;
	long signal_length = 0;
	static double data[BUFFER_LEN];
	int readcount;
	while ( readcount = sf_read_double( infile, data, BUFFER_LEN ) ) {
		signal_length += readcount;
		for( int i = 0; i < readcount; i+= sfinfo.channels ) {
			signal.push_back( (float)data[i] );
		}
	}
	signal_length /= sfinfo.channels;
	
	// close wav file
	sf_close( infile );
	
  	// get STFT of signal vector and write results to testing file
	stft( signal, signal_length, 1024, 1024, testing_file );
		
	// close testing file
	testing_file.close();
	
	cout << "Testing data writing done" << endl;
	
	return 0;
}

void hamming( int window_length, float *buffer ) {
	for( int i = 0; i < window_length; i++ ) {
		buffer[i] = 0.54 - ( 0.46 * cos( 2 * PI * (i / (window_length - 1.0) ) ) );
	}
}

void stft( vector<float> &signal, long signal_length, int window_size, int hop_size, ofstream &testing_file ) {
	
	// declare input data bucket, output fft_result bucket, and plan
	fftw_complex *data, *fft_result;
	fftw_plan plan_forward;
	
	// malloc input and output buckets
	data = (fftw_complex*) fftw_malloc( sizeof( fftw_complex ) * window_size );
	fft_result = (fftw_complex*) fftw_malloc( sizeof( fftw_complex ) * window_size );
	
	// create FFT plan
	plan_forward = fftw_plan_dft_1d( window_size, data, fft_result, FFTW_FORWARD, FFTW_ESTIMATE );
	
	// create Hamming window
	float window[window_size];
	hamming( window_size, window );
	
	// read chunks from signal vector, write to input data bucket, perform FFT, write results to training file
	long chunk_pos = 0;
	long read_index;
	int stop = 0;
	long num_chunks = 0;
	int x = 0;
	while( chunk_pos < signal_length && !stop ) {
		if(x++ % 10 == 0) {
			cout << ".";
			cout.flush();
		}
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
		
		vector<double> peaks;
		
		for( int i = 0; i < window_size; i++ ) {
			double mag = fft_result[i][0] * fft_result[i][0];
			// TO-DO: find out if fft_result[j][1] (complex part) should be discarded or not, right now it is discarded
			if(peaks.size() < NUM_PEAKS) {
				peaks.push_back(mag);
			}
			else {
				int min_index = 0;
				double min = numeric_limits<double>::max();
				for(int k = 0; k < NUM_PEAKS; k++) {
					if(peaks[k] < min) {
						min = peaks[k];
						min_index = k;
					}
				}
				if(mag > min) {
					peaks[min_index] = mag;
				}
			}
		}
		
		bool firstentry = true;
		for(int i = 0; i < window_size; i++) {
			double mag = fft_result[i][0] * fft_result[i][0];
			if(find(peaks.begin(), peaks.end(), mag) != peaks.end()) {
				testing_file << (firstentry ? "" : " ") << (i+1) << ":" << (int)(mag);
				firstentry = false;
			}
		}
		
		testing_file << endl;
		
		chunk_pos += hop_size;
		num_chunks++;
	}
	
	// destroy plan
	fftw_destroy_plan( plan_forward );
	
	// free input data and output fft_result buckets
	fftw_free( data );
	fftw_free( fft_result );
	cout << endl;
}
