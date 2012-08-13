#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <cmath>
#include <limits>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <fftw3.h>
#include <sndfile.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define PI 3.14159265
#define BUFFER_LEN 1024
#define TRAINING_FILE "training"
#define LABEL_FILE "label/"
#define WAVE_FILE "wav/"
#define NUM_KEYS 12
#define NUM_WAVES 500
#define NUM_PEAKS 20

using namespace std;

long long positives[NUM_KEYS];
long long negatives[NUM_KEYS];
   
void hamming( int window_length, float *buffer );
void stft( vector<float> &signal, long signal_length, int window_size, int hop_size, int labels[], ofstream training_files[] );

int main() {

	cout << "Begin writing training data" << endl;
	
	// open all training files for writing
   ofstream training_files[NUM_KEYS];
   for( int i = 0; i < NUM_KEYS; i++ ) {
   	stringstream training_filename;
   	training_filename << TRAINING_FILE << i << ".txt";
   	training_files[i].open( training_filename.str().c_str() );
   }
   
   memset(positives, 0, sizeof(long long) * NUM_KEYS);
   memset(negatives, 0, sizeof(long long) * NUM_KEYS);
   
   // populate training files with stft data from each training wav file
   for( int i = 0; i < NUM_WAVES; i++ ) {

		// get wav file name
		stringstream wav_filename;
		wav_filename << WAVE_FILE << i << ".wav";
	
		cout << "Processing WAV file: " << wav_filename.str();
	
		// open wav file
		SNDFILE *infile;
		SF_INFO sfinfo;
		if ( !( infile = sf_open( wav_filename.str().c_str(), SFM_READ, &sfinfo ) ) ) {
			printf( "Not able to open input file %s.\n", wav_filename.str().c_str() );
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
	
		// open, read, and close label files
		ifstream label_file;
		stringstream ss;
		ss << LABEL_FILE << i << ".label";
		label_file.open( ss.str().c_str() );
		int labels[NUM_KEYS];
		for( int j = 0; j < NUM_KEYS; j++ ) {
			label_file >> labels[j];
		}
		label_file.close();
	
	  	// get STFT of signal vector, match them up with labels, and write results to training files
		stft( signal, signal_length, 1024, 1024, labels, training_files );
		cout << "DONE" << endl;
	}
	
	// close training files
	for( int i = 0; i < NUM_KEYS; i++ ) {
		training_files[i].close();
	}
	
	cout << "Training data writing done" << endl;
	for(int i = 0; i < NUM_KEYS; i++) {
		cout << "Key " << i << " has " << positives[i] << " positives and " << negatives[i] << " negatives " << endl;
	}
	
	return 0;
}

void hamming( int window_length, float *buffer ) {
	for( int i = 0; i < window_length; i++ ) {
		buffer[i] = 0.54 - ( 0.46 * cos( 2 * PI * (i / (window_length - 1.0) ) ) );
	}
}

void stft( vector<float> &signal, long signal_length, int window_size, int hop_size, int labels[], ofstream training_files[] ) {
   
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
	bool first_time = true;
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
		bool has_signal = false;
		for( int i = 0; i < window_size; i++ ) {
			if(fft_result[i][0] > 0.0001) {
				has_signal = true;
			}
		}
		if(has_signal || first_time) {
			first_time = false;
			for( int i = 0; i < NUM_KEYS; i++ ) {
				vector<double> peaks;				
				for( int j = 0; j < window_size; j++ ) {
					double mag = fft_result[j][0] * fft_result[j][0];
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
				if(has_signal && labels[i] == 1) {
					positives[i]++;
				}
				else {
					negatives[i]++;
				}
				training_files[i] << ( has_signal ? labels[i] : -1 );
				for(int j = 0; j < window_size; j++) {
					double mag = fft_result[j][0] * fft_result[j][0];
					if(find(peaks.begin(), peaks.end(), mag) != peaks.end()) {
						training_files[i] << " " << (j+1) << ":" << (int)(mag);
					}
				}
				training_files[i] << endl;
			}
		}
		chunk_pos += hop_size;
		num_chunks++;
	}
	
	// destroy plan
	fftw_destroy_plan( plan_forward );
	
	// free input data and output fft_result buckets
	fftw_free( data );
	fftw_free( fft_result );
}
