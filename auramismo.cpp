#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include "svm.h"

#include "params.h"

using namespace std;

int max_nr_attr = 64;

static char *line = NULL;
static int max_line_len;

void predict(FILE *input, FILE *output, struct svm_model* model[], int size);
void predict_piano(FILE *input, FILE *output, struct svm_model* model[], int size);
void proc(const char* TESTING_FILE, const char* PIANO_OUTPUT, const char* DRUM_OUTPUT, struct svm_model* piano_model[], struct svm_model* drum_model[]);
void proc(const char* TESTING_FILE, const char* OUTPUT, struct svm_model* model[]);

int main() {
	cout << "Begin SVM prediction" << endl;
	
	struct svm_model* piano_model[NUM_KEYS];
	for(int i = 0; i < NUM_KEYS; i++)
	{
		stringstream ss;
		ss << PIANO_TRAINING_FILE << i << MODEL_FILE_EXTENSION;
		if((piano_model[i] = svm_load_model(ss.str().c_str())) == 0)
		{
			fprintf(stderr, "can't open model file %s\n", ss.str().c_str());
			return 1;
		}
	}
	
	/**
	
	struct svm_model* clarinet_model[NUM_KEYS];
	for(int i = 0; i < NUM_KEYS; i++)
	{
		stringstream ss;
		ss << CLARINET_TRAINING_FILE << i << MODEL_FILE_EXTENSION;
		if((clarinet_model[i] = svm_load_model(ss.str().c_str())) == 0)
		{
			fprintf(stderr, "can't open model file %s\n", ss.str().c_str());
			return 1;
		}
	}
	
	*/
	
	//proc("testing.txt", "piano.txt", "clarinet.txt", piano_model, clarinet_model);
	
	proc(TESTING_FILE_0, "temp0.txt", piano_model);
	proc(TESTING_FILE_1, "temp1.txt", piano_model);
	
	//proc(TESTING_FILE_0, "temp_piano_0", "temp_clarinet_0", piano_model, clarinet_model);
	//proc(TESTING_FILE_1, "temp_piano_1", "temp_clarinet_1", piano_model, clarinet_model);
	
	for(int i = 0; i < NUM_KEYS; i++) {
		svm_free_and_destroy_model(&piano_model[i]);
	}
	
	/**
	for(int i = 0; i < NUM_KEYS; i++) {
		svm_free_and_destroy_model(&clarinet_model[i]);
	}
	*/
	
	cout << "SVM prediction finished" << endl;
	return 0;
}

void proc(const char* TESTING_FILE, const char* OUTPUT, struct svm_model* model[])
{
	FILE *input, *output;
	int i;
	
	input = fopen(TESTING_FILE, "r");
	if(input == NULL)
	{
		fprintf(stderr, "can't open input file %s\n", TESTING_FILE);
		exit(1);
	}

	output = fopen(OUTPUT, "w");
	if(output == NULL)
	{
		fprintf(stderr, "can't open output file %s\n", OUTPUT);
		exit(1);
	}

	predict(input, output, model, NUM_KEYS);
	fclose(input);
	fclose(output);
}

void proc(const char* TESTING_FILE, const char* PIANO_OUTPUT, const char* CLARINET_OUTPUT, struct svm_model* piano_model[], struct svm_model* clarinet_model[]) {

	FILE *input, *piano_output, *clarinet_output;
	int i;
	
	input = fopen(TESTING_FILE, "r");
	if(input == NULL)
	{
		fprintf(stderr, "can't open input file %s\n", TESTING_FILE);
		exit(1);
	}

	piano_output = fopen(PIANO_OUTPUT, "w");
	if(piano_output == NULL)
	{
		fprintf(stderr, "can't open output file %s\n", PIANO_OUTPUT);
		exit(1);
	}
	
	clarinet_output = fopen(CLARINET_OUTPUT, "w");
	if(clarinet_output == NULL)
	{
		fprintf(stderr, "can't open output file %s\n", CLARINET_OUTPUT);
		exit(1);
	}

	predict(input, piano_output, piano_model, NUM_KEYS);
	fclose(input);
	input = fopen(TESTING_FILE, "r");
	if(input == NULL)
	{
		fprintf(stderr, "can't open input file %s\n", TESTING_FILE);
		exit(1);
	}
	fclose(piano_output);
	
	predict(input, clarinet_output, clarinet_model, NUM_KEYS);
	fclose(input);
	fclose(clarinet_output);	
}


static char* readline(FILE *input)
{
	int len;
	
	if(fgets(line,max_line_len,input) == NULL)
		return NULL;

	while(strrchr(line,'\n') == NULL)
	{
		max_line_len *= 2;
		line = (char *) realloc(line,max_line_len);
		len = (int) strlen(line);
		if(fgets(line+len,max_line_len-len,input) == NULL)
			break;
	}
	return line;
}

void predict_piano(FILE *input, FILE *output, struct svm_model* model[], int size) 
{
	int chunk_num = 0;
	
	FILE *temp;
	temp = fopen("temp.out", "w");
	if(temp == NULL)
	{
		fprintf(stderr, "can't open temp file %s\n", "temp.out");
		exit(1);
	}

	
	max_line_len = 1024;
	line = (char *)malloc(max_line_len*sizeof(char));
	while(readline(input) != NULL)
	{
		cout << "Processing chunk number: " << chunk_num++ << endl;
		struct svm_node *x = (struct svm_node *) malloc(max_nr_attr*sizeof(struct svm_node));
		
		int i = 0;
		int predict_label[12];
		char *idx, *val, *label, *endptr;
		int inst_max_index = -1; // strtol gives 0 if wrong format, and precomputed kernel has <index> start from 0

		label = strtok(line," \t\n");
	
		while(1)
		{
			if(i>=max_nr_attr-1)	// need one more for index = -1
			{
				max_nr_attr *= 2;
				x = (struct svm_node *) realloc(x,max_nr_attr*sizeof(struct svm_node));
			}

			idx = strtok(NULL,":");
			val = strtok(NULL," \t");

			if(val == NULL)
				break;
			errno = 0;
			x[i].index = (int) strtol(idx,&endptr,10);
			inst_max_index = x[i].index;

			errno = 0;
			x[i].value = strtod(val,&endptr);
			
			++i;
		}
		x[i].index = -1;

		for(int k = 0; k < 12; k++)
			predict_label[k] = 0;

		for(int k = 0; k < size; k++)
		{
			predict_label[k%12] |= (int)svm_predict(model[k],x);
		}
		
		for(int k = 0; k < 12; k++)
			fprintf(temp, "%d ", predict_label[k]);	
		
		free(x);
		fprintf(temp, "\n");
	}
	
	fclose(temp);
	temp = fopen("temp.out", "r");
	if(temp == NULL) {
		fprintf(stderr, "can't open temp file %s\n", "temp.out");
		exit(1);
	}
	
	fprintf(output, "%d %d\n", 12, chunk_num);
	while(readline(temp) != NULL) {
		fprintf(output, "%s", line);
	}
}

void predict(FILE *input, FILE *output, struct svm_model* model[], int size) 
{
	int chunk_num = 0;
	
	FILE *temp;
	temp = fopen("temp.out", "w");
	if(temp == NULL)
	{
		fprintf(stderr, "can't open temp file %s\n", "temp.out");
		exit(1);
	}

	
	max_line_len = 1024;
	line = (char *)malloc(max_line_len*sizeof(char));
	while(readline(input) != NULL)
	{
		cout << "Processing chunk number: " << chunk_num++ << endl;
		struct svm_node *x = (struct svm_node *) malloc(max_nr_attr*sizeof(struct svm_node));
		
		int i = 0;
		double predict_label;
		char *idx, *val, *label, *endptr;
		int inst_max_index = -1; // strtol gives 0 if wrong format, and precomputed kernel has <index> start from 0

		label = strtok(line," \t\n");
	
		while(1)
		{
			if(i>=max_nr_attr-1)	// need one more for index = -1
			{
				max_nr_attr *= 2;
				x = (struct svm_node *) realloc(x,max_nr_attr*sizeof(struct svm_node));
			}

			idx = strtok(NULL,":");
			val = strtok(NULL," \t");

			if(val == NULL)
				break;
			errno = 0;
			x[i].index = (int) strtol(idx,&endptr,10);
			inst_max_index = x[i].index;

			errno = 0;
			x[i].value = strtod(val,&endptr);
			
			++i;
		}
		x[i].index = -1;

		for(int k = 0; k < size; k++)
		{
			predict_label = svm_predict(model[k],x);
			fprintf(temp, "%d ", (int)predict_label);	
		}
		
		free(x);
		fprintf(temp, "\n");
	}
	
	fclose(temp);
	temp = fopen("temp.out", "r");
	if(temp == NULL) {
		fprintf(stderr, "can't open temp file %s\n", "temp.out");
		exit(1);
	}
	
	fprintf(output, "%d %d\n", size, chunk_num);
	while(readline(temp) != NULL) {
		fprintf(output, "%s", line);
	}
}


