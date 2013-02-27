#include <sndfile.h>
#include <iostream>
#define BUFFER_LEN 1024

using namespace std;

int main()
{
	SF_INFO sfinfo;
	int readcount;
	
	// open files
	
	SNDFILE *infile = sf_open("testout.wav", SFM_READ, &sfinfo);
	SNDFILE *outfile_0 = sf_open("test0.wav", SFM_WRITE, &sfinfo);
	SNDFILE *outfile_1 = sf_open("test1.wav", SFM_WRITE, &sfinfo);
	
	if (!infile) {
		cerr << "Not able to open input file: testout.wav" << endl;
		return 1;
	}
	if(!outfile_0) {
		cerr << "Not able to open input file: test0.wav" << endl;
		return 1;
	}
	if(!outfile_1) {
		cerr << "Not able to open input file: test1.wav" << endl;
		return 1;
	}

	// read wav data into buffer and write to separate files
	long signal_length = 0;
	static float data[BUFFER_LEN];
	static float leftdata[BUFFER_LEN];
	static float rightdata[BUFFER_LEN];

	while (readcount = sf_read_float(infile, data, BUFFER_LEN)) {
		signal_length += readcount;
		for(int j = 0; j < readcount; j += sfinfo.channels) {
			leftdata[j] = data[j];
			leftdata[j+1] = data[j];
		}
		for(int j = 1; j < readcount; j += sfinfo.channels) {
			rightdata[j-1] = data[j];
			rightdata[j] = data[j];
		}
		sf_write_float(outfile_0, leftdata, readcount);
		sf_write_float(outfile_1, rightdata, readcount);
	}
	signal_length /= sfinfo.channels;

	// close files
	sf_close(infile);
	sf_close(outfile_0);
	sf_close(outfile_1);
	
	return 0;
}
