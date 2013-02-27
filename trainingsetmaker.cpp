#include <string.h>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>

// for reading WAV files
#include <sndfile.h>
#include <fstream>
#include <sstream>

// for STFT
#include <fftw3.h>
#include <cmath>

// for peak picking
#include <queue>
#include "magnitude_point.cpp"
#include <algorithm>

#include "params.h"

using namespace std;

long long piano_positives[NUM_KEYS];
long long piano_negatives[NUM_KEYS];

long long clarinet_positives[NUM_KEYS];
long long clarinet_negatives[NUM_KEYS];
   
void hamming(int window_length, float buffer[]);
void stft(vector<float> &signal, long signal_length, int window_size, int hop_size, 
			int labels[], ofstream training_files[], long long positives[], long long negatives[]);

int wav_num = 0;
int main() {

	cout << "Begin writing training data" << endl;
	
	// open all training files for writing
   ofstream piano_training_files[NUM_KEYS];
   //ofstream clarinet_training_files[NUM_KEYS];
   
   for(int i = 0; i < NUM_KEYS; i++) {
   	stringstream training_filename;
   	training_filename << PIANO_TRAINING_FILE << i << ".txt";
   	piano_training_files[i].open(training_filename.str().c_str());
   }
   
   /**
   for(int i = 0; i < NUM_KEYS; i++) {
   	stringstream training_filename;
   	training_filename << CLARINET_TRAINING_FILE << i << ".txt";
   	clarinet_training_files[i].open(training_filename.str().c_str());
   }
   */
   
   // reset label count
   memset(piano_positives, 0, sizeof(long long) * NUM_KEYS);
   memset(piano_negatives, 0, sizeof(long long) * NUM_KEYS);
   
   //memset(clarinet_positives, 0, sizeof(long long) * NUM_KEYS);
   //memset(clarinet_negatives, 0, sizeof(long long) * NUM_KEYS);
   
   // populate training files with STFT data from each training WAV file
   for(int i = 0; i < NUM_WAVS /* * 2 */; i++) {

		wav_num = i;
		
		// get wav file name
		stringstream wav_filename;
		
		wav_filename << "wav/" << i << ".wav";
		
		/**
		if(i < NUM_WAVS)
		{
			wav_filename << PIANO_WAV_DIR << i << ".wav";
		}
		else
		{
			wav_filename << CLARINET_WAV_DIR << (i-NUM_WAVS) << ".wav";
		}
		*/
	
		cout << "Processing WAV file: " << wav_filename.str();
	
		// open wav file
		SF_INFO sfinfo;
		SNDFILE *infile = sf_open(wav_filename.str().c_str(), SFM_READ, &sfinfo);
		if (!infile) {
			cerr << "Not able to open input file: " << wav_filename.str() << endl;
			return 1;
		}
	
		// read wav data into buffer and push buffered data into signal vector
		vector<float> signal;
		long signal_length = 0;
		static float data[BUFFER_LEN];
		int readcount;
		while (readcount = sf_read_float(infile, data, BUFFER_LEN)) {
			signal_length += readcount;
			for(int j = 0; j < readcount; j += sfinfo.channels) {
				signal.push_back(data[j]);
			}
		}
		signal_length /= sfinfo.channels;
	
		// close wav file
		sf_close(infile);
	
		// open, read, and close label files
		ifstream label_file;
		
		stringstream label_filename;
		
		label_filename << "label/" << i << ".label";
		
		/**
		if(i < NUM_WAVS)
		{
			label_filename << PIANO_LABEL_DIR << i << LABEL_EXT;
		}
		else
		{
			label_filename << CLARINET_LABEL_DIR << (i-NUM_WAVS) << LABEL_EXT;
		}
		*/
		
		label_file.open(label_filename.str().c_str());
		
		int labels[NUM_KEYS];
		
		for(int j = 0; j < NUM_KEYS; j++) {
			label_file >> labels[j];
		}
		label_file.close();
	
	  	// get STFT of signal vector, match them up with labels, and write results to training files
		//if(i < NUM_WAVS)
		//{
			stft(signal, signal_length, WINDOW_SIZE, HOP_SIZE, labels, piano_training_files, piano_positives, piano_negatives);
		//}
		//else
		//{
		//	stft(signal, signal_length, WINDOW_SIZE, HOP_SIZE, labels, clarinet_training_files, clarinet_positives, clarinet_negatives);
		//}
		cout << "DONE" << endl;
	}
	
	// close training files
	for(int i = 0; i < NUM_KEYS; i++) {
		piano_training_files[i].close();
	}
	
	/**
	for(int i = 0; i < NUM_KEYS; i++) {
		clarinet_training_files[i].close();
	}
	*/
	
	cout << "Training data writing done" << endl;
	for(int i = 0; i < NUM_KEYS; i++) {
		cout << "Piano Key " << i << " has " << piano_positives[i] << " positives and " << piano_negatives[i] << " negatives " << endl;
	}
	/**
	for(int i = 0; i < NUM_KEYS; i++) {
		cout << "Clarinet Key " << i << " has " << clarinet_positives[i] << " positives and " << clarinet_negatives[i] << " negatives " << endl;
	}
	*/
	
	return 0;
}

void hamming(int window_length, float buffer[]) {
	for(int i = 0; i < window_length; i++) {
		buffer[i] = 0.54 - 0.46 * cos(2 * PI * i / (window_length - 1.0));
	}
}

void stft(vector<float> &signal, long signal_length, int window_size, int hop_size, 
			int labels[], ofstream training_files[], long long positives[], long long negatives[]) {
   
	// declare input data bucket, output fft_result bucket, and plan
	fftw_complex *data, *fft_result;
	fftw_plan plan_forward;
	
	// malloc input and output buckets
	data = (fftw_complex*) fftw_malloc(sizeof(fftw_complex ) * window_size);
	fft_result = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * window_size);
	
	// create FFT plan
	plan_forward = fftw_plan_dft_1d(window_size, data, fft_result, FFTW_FORWARD, FFTW_ESTIMATE);
	
	// create Hamming window
	float window[window_size];
	hamming(window_size, window);
	
	long chunk_pos = 0;
	long read_index = 0;
	bool stop = false;
	long num_chunks = 0;
	bool first_time = true;
	int negalimiter[NUM_KEYS];
	for(int i = 0; i < NUM_KEYS; i++)
		negalimiter[i] = 0;
		
	while(chunk_pos < signal_length && !stop) {
	
		// progress report
		if(num_chunks % 5 == 0)
		{
			cout << ".";
			cout.flush();
		}
					
		if(num_chunks % TRAINING_SHRINKER == 0) {
		
			// read chunk from signal vector, write to input data bucket
			for(int i = 0; i < window_size; i++) {
				read_index = chunk_pos + i;
				if(read_index < signal_length) {
					data[i][0] = signal[read_index] * window[i];
					data[i][1] = 0;
				} 
				else {
					data[i][0] = 0;
					data[i][1] = 0;
					stop = true;	
				}
			}
		
			// perform FFT
			fftw_execute(plan_forward);
		
			// determine if chunk has a sound or is silent
			bool has_signal = false;
			for(int i = 0; i < window_size; i++) {
				if((int)(fft_result[i][0] * fft_result[i][0]) > 0) {
					has_signal = true;
				}
			}
		
			if(has_signal || first_time) {
				first_time = false;
						
				// collect magnitudes
				priority_queue<magnitude_point> mags;
				for(int j = 0; j < window_size; j++) {
					// TO-DO: find out if fft_result[j][1] (complex part) should be discarded or not, right now it is discarded
					double mag = fft_result[j][0] * fft_result[j][0];
					mags.push(magnitude_point(j, mag));
				}
			
				// peak picking
				vector<magnitude_point> peaks;
				for(int j = 0; j < NUM_PEAKS; j++) {
					peaks.push_back(mags.top());
					mags.pop();
				}
				sort(peaks.begin(), peaks.end(), index_sort);
				
				// write label and peaks
				for(int i = 0; i < NUM_KEYS; i++) {
					int res = ( has_signal ? labels[i] : 0 );
					if(res == 1 || negalimiter[i]++ % NEGA_LIMITER == 0)
					{
						training_files[i] << res;
						for(int j = 0; j < NUM_PEAKS; j++) {
							if((int)(peaks[j].magnitude) != 0)
								training_files[i] << " " << setw(4) << right << (peaks[j].index + 1) << ":" << setw(4) << left << (int)(peaks[j].magnitude);
						}
						training_files[i] << endl;
						if(res == 1) positives[i]++;
						else negatives[i] ++;
					}
				}
			}
		}
		chunk_pos += hop_size;
		num_chunks++;
	}
	
	// destroy plan
	fftw_destroy_plan(plan_forward);
	
	// free input data and output fft_result buckets
	fftw_free(data);
	fftw_free(fft_result);
}
