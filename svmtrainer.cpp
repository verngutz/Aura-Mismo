#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <iostream>
#include <sstream>
#include <cstdlib> 
#include "svm.h"

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define Malloc(type,n) (type *)malloc((n)*sizeof(type))

#include "params.h"

using namespace std;

void read_problem(const char *filename, struct svm_parameter &param, struct svm_problem &prob);

int main(int argc, char* argv[]) {
	cout << "Begin SVM training" << endl;
	
	// declare SVM parameters
	struct svm_parameter param;
	param.svm_type = C_SVC;
	param.kernel_type = RBF;
	param.degree = 12;
	param.gamma = 0;
	param.coef0 = 0;
	param.nu = 0.5;
	param.cache_size = 100;
	param.C = 1;
	param.eps = 1e-3;
	param.p = 0.1;
	param.shrinking = 0;
	param.probability = 0;
	param.nr_weight = 0;
	param.weight_label = NULL;
	param.weight = NULL;
	
	int start = 0;
	int end = NUM_KEYS; //* 2;
	
	if(argc > 1)
	{
		start = atoi(argv[1]);
	}
	if(argc > 2)
	{
		end = atoi(argv[2]);
	}
		
	// create SVM for each training file
	for( int i = start; i < end; i++ ) {
		if(i < NUM_KEYS) {
			cout << "Training Piano Key: " << i << endl;
		}
		else {
			cout << "Training Clarinet: " << i - NUM_KEYS << endl;
		}
		
		// get training file name
		stringstream training_file_name;
		if(i < NUM_KEYS)
		{
			training_file_name << PIANO_TRAINING_FILE << i << ".txt";
		}
		else
		{
			training_file_name << CLARINET_TRAINING_FILE << i - NUM_KEYS << ".txt";
		}
		
		// create SVM problem from training file
		struct svm_problem prob;
		read_problem( training_file_name.str().c_str(), param, prob );
		const char * error_msg = svm_check_parameter( &prob, &param );
		if( error_msg ) {
			fprintf( stdout, "ERROR: %s\n", error_msg );
			return 1;
		}
	
		// get model file name
		stringstream model_file_name;
		model_file_name << training_file_name.str() << ".model";
	
		cout << "Training: " << model_file_name.str() << endl;
	
		// train SVM and save the model to file
		struct svm_model *model = svm_train( &prob, &param );
		if( svm_save_model( model_file_name.str().c_str(), model ) ) {
			fprintf( stdout, "can't save model to file %s\n", model_file_name.str().c_str() );
			return 1;
		}
	
		// clean-up memory used by SVM training
		svm_free_and_destroy_model( &model );
		free( prob.y );
		free( prob.x );
   }
   
	svm_destroy_param( &param );
   
   cout << "SVM training done" << endl;
	return 0;
}

static char *line = NULL;
static int max_line_len;

static char* readline( FILE *input ) {
	int len;
	
	if( fgets( line, max_line_len,input ) == NULL)
		return NULL;

	while( strrchr( line, '\n' ) == NULL ) {
		max_line_len *= 2;
		line = (char *) realloc( line, max_line_len );
		len = (int) strlen( line );
		if( fgets( line + len, max_line_len - len, input ) == NULL)
			break;
	}
	return line;
}

// read in a problem (in svmlight format)
void read_problem( const char *filename, struct svm_parameter &param, struct svm_problem &prob ) {
		
	int elements, max_index, inst_max_index, i, j;
	FILE *fp = fopen( filename, "r" );
	char *endptr;
	char *idx, *val, *label;

	if( fp == NULL )	{
		fprintf( stdout,"can't open input file %s\n", filename );
		exit(1);
	}

	prob.l = 0;
	elements = 0;

	max_line_len = 1024;
	line = Malloc( char, max_line_len );
	while( readline( fp ) != NULL ) {
		char *p = strtok( line, " \t" ); // label
		// features
		while(1) {
			p = strtok( NULL, " \t" );
			if( p == NULL || *p == '\n' ) // check '\n' as ' ' may be after the last feature
				break;
			++elements;
		}
		++elements;
		++prob.l;
	}
	rewind( fp );

	prob.y = Malloc( double,prob.l );
	prob.x = Malloc( struct svm_node *, prob.l );
	struct svm_node *x_space = Malloc( struct svm_node, elements );

	max_index = 0;
	j = 0;
	for( i = 0; i < prob.l; i++ ) {
		inst_max_index = -1; // strtol gives 0 if wrong format, and precomputed kernel has <index> start from 0
		readline( fp );
		prob.x[i] = &x_space[j];
		label = strtok( line, " \t\n" );
		if( label == NULL ) // empty line
			exit(1);

		prob.y[i] = strtod(label,&endptr);
		if( endptr == label || *endptr != '\0' )
			exit(1);

		while(1) {
			idx = strtok( NULL, ":" );
			val = strtok( NULL, " \t" );

			if( val == NULL) {
				break;
			}

			errno = 0;
			x_space[j].index = (int) strtol( idx, &endptr, 10 );
			if( endptr == idx || errno != 0 || *endptr != '\0' || x_space[j].index <= inst_max_index ) {
				exit(1);
			}
			else {
				inst_max_index = x_space[j].index;
			}

			errno = 0;
			x_space[j].value = strtod( val, &endptr );
			if( endptr == val || errno != 0 || ( *endptr != '\0' && !isspace( *endptr ) ) )
				exit(1);
			++j;
		}

		if( inst_max_index > max_index )
			max_index = inst_max_index;
		x_space[j++].index = -1;
	}

	if( param.gamma == 0 && max_index > 0 )
		param.gamma = 1.0/max_index;

	if( param.kernel_type == PRECOMPUTED ) {
		for( i = 0; i < prob.l; i++ ) {
			if ( prob.x[i][0].index != 0 ) {
				fprintf( stdout, "Wrong input format: first column must be 0:sample_serial_number\n" );
				exit(1);
			}
			if ( (int)prob.x[i][0].value <= 0 || (int)prob.x[i][0].value > max_index )
			{
				fprintf( stdout, "Wrong input format: sample_serial_number out of range\n" );
				exit(1);
			}
		}
	}
	//free( x_space );
	fclose( fp );
}

