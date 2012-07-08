/*
 * fft.c: compute FFT and IFFT from an array
 */
#include <stdio.h>
#include <fftw3.h>
 
#define SIZE 4
 
int main( int argc, char** argv )
{
	double          input[SIZE] = { 1.0, 1.0, 1.0, 1.0 };
	fftw_complex    *data, *fft_result, *ifft_result;
	fftw_plan       plan_forward, plan_backward;
	int             i;
   
	data        = ( fftw_complex* ) fftw_malloc( sizeof( fftw_complex ) * SIZE );
	fft_result  = ( fftw_complex* ) fftw_malloc( sizeof( fftw_complex ) * SIZE );
	ifft_result = ( fftw_complex* ) fftw_malloc( sizeof( fftw_complex ) * SIZE );
   
	plan_forward  = fftw_plan_dft_1d( SIZE, data, fft_result, FFTW_FORWARD, FFTW_ESTIMATE );
	plan_backward = fftw_plan_dft_1d( SIZE, fft_result, ifft_result, FFTW_BACKWARD, FFTW_ESTIMATE );
   
	/* populate input data */
	for( i = 0 ; i < SIZE ; i++ ) {
		data[i][0] = input[i];
		data[i][1] = 0.0;
	}
 
	/* print initial data */
	for( i = 0 ; i < SIZE ; i++ ) {
		fprintf( stdout, "data[%d] = { %2.2f, %2.2f }\n",
					i, data[i][0], data[i][1] );
	}
   
	fftw_execute( plan_forward );
   
	/* print fft result */
	for( i = 0 ; i < SIZE ; i++ ) {
		fprintf( stdout, "fft_result[%d] = { %2.2f, %2.2f }\n",
					i, fft_result[i][0], fft_result[i][1] );
	}
 
	fftw_execute( plan_backward );
 
	/* print ifft result */
	for( i = 0 ; i < SIZE ; i++ ) {
		fprintf( stdout, "ifft_result[%d] = { %2.2f, %2.2f }\n",
					i, ifft_result[i][0] / SIZE, ifft_result[i][1] / SIZE );
	}
 
	/* free memory */
	fftw_destroy_plan( plan_forward );
	fftw_destroy_plan( plan_backward );
 
	fftw_free( data );
	fftw_free( fft_result );
	fftw_free( ifft_result );
   
	return 0;
}
