#include <string.h>
#include <string>
#include <vector>
#include <iostream>

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

// for parallelism
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "params.h"

using namespace std;

long long positives[NUM_KEYS + NUM_DRUM_GROUPS];
long long negatives[NUM_KEYS + NUM_DRUM_GROUPS];
   
void hamming(int window_length, float buffer[]);
void stft(vector<float> &signal, long signal_length, int window_size, int hop_size, 
			int piano_labels[], int drum_labels[], ofstream piano_training_files[], ofstream drum_training_files[]);

int main() {

	cout << "Begin writing training data" << endl;
	
	// open all training files for writing
   ofstream piano_training_files[NUM_KEYS];
   ofstream drum_training_files[NUM_DRUM_GROUPS];
   
   for(int i = 0; i < NUM_KEYS; i++) {
   	stringstream training_filename;
   	training_filename << PIANO_TRAINING_FILE << i << ".txt";
   	piano_training_files[i].open(training_filename.str().c_str());
   }
   
   for(int i = 0; i < NUM_DRUM_GROUPS; i++) {
   	stringstream training_filename;
   	training_filename << DRUM_TRAINING_FILE << i << ".txt";
   	drum_training_files[i].open(training_filename.str().c_str());
   }
   
   // reset label count
   memset(positives, 0, sizeof(long long) * (NUM_KEYS + NUM_DRUM_GROUPS));
   memset(negatives, 0, sizeof(long long) * (NUM_KEYS + NUM_DRUM_GROUPS));
   
   // populate training files with STFT data from each training WAV file
   for(int i = 0; i < NUM_WAVS; i++) {

		// get wav file name
		stringstream wav_filename;
		wav_filename << WAVE_FILE << i << ".wav";
	
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
		ifstream piano_label_file;
		ifstream drum_label_file;
		
		stringstream piano_label_filename;
		stringstream drum_label_filename;
		
		piano_label_filename << LABEL_DIR << i << PIANO_LABEL_EXT;
		drum_label_filename << LABEL_DIR << i << DRUM_LABEL_EXT;
		
		piano_label_file.open(piano_label_filename.str().c_str());
		drum_label_file.open(drum_label_filename.str().c_str());
		
		int piano_labels[NUM_KEYS];
		int drum_labels[NUM_DRUM_GROUPS];
		
		for(int j = 0; j < NUM_KEYS; j++) {
			piano_label_file >> piano_labels[j];
		}
		piano_label_file.close();
		
		for(int j = 0; j < NUM_DRUM_GROUPS; j++) {
			drum_label_file >> drum_labels[j];
		}
		drum_label_file.close();
	
	  	// get STFT of signal vector, match them up with labels, and write results to training files
		stft(signal, signal_length, WINDOW_SIZE, HOP_SIZE, piano_labels, drum_labels, piano_training_files, drum_training_files);
		cout << "DONE" << endl;
	}
	
	// close training files
	for(int i = 0; i < NUM_KEYS; i++) {
		piano_training_files[i].close();
	}
	
	for(int i = 0; i < NUM_DRUM_GROUPS; i++) {
		drum_training_files[i].close();
	}
	
	cout << "Training data writing done" << endl;
	for(int i = 0; i < NUM_KEYS; i++) {
		cout << "Key " << i << " has " << positives[i] << " positives and " << negatives[i] << " negatives " << endl;
	}
	for(int i = NUM_KEYS; i < NUM_KEYS + NUM_DRUM_GROUPS; i++) {
		cout << "Drum Group " << i << " has " << positives[i] << " positives and " << negatives[i] << " negatives " << endl;
	}
	
	return 0;
}

void hamming(int window_length, float buffer[]) {
	for(int i = 0; i < window_length; i++) {
		buffer[i] = 0.54 - 0.46 * cos(2 * PI * i / (window_length - 1.0));
	}
}

void stft(vector<float> &signal, long signal_length, int window_size, int hop_size, 
			int piano_labels[], int drum_labels[], ofstream piano_training_files[], ofstream drum_training_files[]) {
   
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
	while(chunk_pos < signal_length && !stop) {
	
		// progress report
		if(num_chunks % 3 == 0) {
			cout << ".";
			cout.flush();
		
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
				if(fft_result[i][0] > 0.0001) {
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
			
				// increase label count
				for(int i = 0; i < NUM_KEYS; i++) {
					if(has_signal && piano_labels[i] == 1) {
						positives[i]++;
					}
					else {
						negatives[i]++;
					}
				}
			
				for(int i = 0; i < NUM_DRUM_GROUPS; i++) {
					if(has_signal && drum_labels[i] == 1) {
						positives[NUM_KEYS + i]++;
					}
					else {
						negatives[NUM_KEYS + i]++;
					}
				}
			
				// write label
				for(int i = 0; i < NUM_KEYS; i++) {
					piano_training_files[i] << ( has_signal ? piano_labels[i] : 0 );
				}
			
				for(int i = 0; i < NUM_DRUM_GROUPS; i++) {
					drum_training_files[i] << ( has_signal ? drum_labels[i] : 0 );
				}
			
				// write peaks to training files
				for(int i = 0; i < NUM_KEYS; i++) {
					for(int j = 0; j < NUM_PEAKS; j++) {
						piano_training_files[i] << " " << (peaks[j].index + 1) << ":" << (int)(peaks[j].magnitude);
					}
					piano_training_files[i] << endl;
				}
			
				for(int i = 0; i < NUM_DRUM_GROUPS; i++) {
					for(int j = 0; j < NUM_PEAKS; j++) {
						drum_training_files[i] << " " << (peaks[j].index + 1) << ":" << (int)(peaks[j].magnitude);
					}
					drum_training_files[i] << endl;
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
