#include <vector>
#include <iostream>
#include <cstdlib>
#include <iomanip>

// for reading WAV files
#include <fstream>
#include <sndfile.h>

// for STFT
#include <fftw3.h>
#include <cmath>

// for peak picking
#include <queue>
#include "magnitude_point.cpp"
#include <algorithm>

#include "params.h"

using namespace std;

void hamming(int window_length, float buffer[]);
void stft(vector<float> &signal, long signal_length, int window_size, int hop_size, ofstream &testing_file);
void proc(const char* TESTING_FILE, const char* WAV_FILE);

int main() {
	cout << "Begin writing testing data" << endl;
	//proc("testing.txt", "test.wav");
	proc(TESTING_FILE_0, WAV_FILE_0);
	proc(TESTING_FILE_1, WAV_FILE_1);
	cout << "Testing data writing done" << endl;
	return 0;
}

void proc(const char* TESTING_FILE, const char* WAV_FILE) {

	// open testing file for writing
	ofstream testing_file;
	testing_file.open(TESTING_FILE);
   
   // populate testing file with STFT data from testing WAV file
	cout << "Processing WAV file: " << WAV_FILE;
		
	// open WAV file
	SF_INFO sfinfo;
	SNDFILE *infile = sf_open(WAV_FILE, SFM_READ, &sfinfo);
	if (!infile) {
		cerr << "Not able to open input file: " << WAV_FILE << endl;
		exit(1);
	}
	
	// read WAV data into buffer and push buffered data into signal vector
	vector<float> signal;
	long signal_length = 0;
	static float data[BUFFER_LEN];
	int readcount;
	while (readcount = sf_read_float(infile, data, BUFFER_LEN)) {
		signal_length += readcount;
		for(int i = 0; i < readcount; i += sfinfo.channels) {
			signal.push_back(data[i]);
		}
	}
	signal_length /= sfinfo.channels;
	
	// close wav file
	sf_close(infile);
	
  	// get STFT of signal vector and write results to testing file
	stft(signal, signal_length, 1024, 1024, testing_file);
		
	// close testing file
	testing_file.close();
}

void hamming(int window_length, float buffer[]) {
	for(int i = 0; i < window_length; i++) {
		buffer[i] = 0.54 - 0.46 * cos(2 * PI * i / (window_length - 1.0));
	}
}

void stft(vector<float> &signal, long signal_length, int window_size, int hop_size, ofstream &testing_file) {
	
	// declare input data bucket, output fft_result bucket, and plan
	fftw_complex *data, *fft_result;
	fftw_plan plan_forward;
	
	// malloc input and output buckets
	data = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * window_size);
	fft_result = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * window_size);
	
	// create FFT plan
	plan_forward = fftw_plan_dft_1d(window_size, data, fft_result, FFTW_FORWARD, FFTW_ESTIMATE);
	
	// create Hamming window
	float window[window_size];
	hamming(window_size, window);
	
	// read chunks from signal vector, write to input data bucket, perform FFT, write results to training file
	long chunk_pos = 0;
	long read_index = 0;
	bool stop = false;
	long num_chunks = 0;
	while(chunk_pos < signal_length && !stop) {
	
		// progress report
		if(num_chunks % 10 == 0) {
			cout << ".";
			cout.flush();
		}
		
		// read chunk from input signal, write to input data bucket
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
		
		// calculate magnitude and sort results with priority queue
		priority_queue<magnitude_point> mags;
		for(int i = 0; i < window_size; i++) {
			// TO-DO: find out if fft_result[j][1] (complex part) should be discarded or not, right now it is discarded
			double mag = fft_result[i][0] * fft_result[i][0];
			mags.push(magnitude_point(i, mag));
		}
		
		// peak picking
		vector<magnitude_point> peaks;
		for(int i = 0; i < NUM_PEAKS; i++) {
			peaks.push_back(mags.top());
			mags.pop();
		}
		sort(peaks.begin(), peaks.end(), index_sort);
		
		// write peaks to testing file
		bool firstentry = true;
		for(int i = 0; i < NUM_PEAKS; i++) {
			testing_file << (firstentry ? "" : " ") << setw(4) << right << (peaks[i].index + 1) << ":" << setw(4) << left << (int)(peaks[i].magnitude);
			firstentry = false;
		}
		
		testing_file << endl;
		
		chunk_pos += hop_size;
		num_chunks++;
	}
	
	// destroy plan
	fftw_destroy_plan(plan_forward);
	
	// free input data and output fft_result buckets
	fftw_free(data);
	fftw_free(fft_result);
	cout << endl;
}
