#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>

using namespace std;

int main() {
	ifstream predict_out_0;
	ifstream predict_out_1;
	
	predict_out_0.open("output0.txt");
	predict_out_1.open("output1.txt");
	
	ifstream true_out_0;
	ifstream true_out_1;
	
	true_out_0.open("test_0.out");
	true_out_1.open("test_1.out");
	
	long double false_positives_0 = 0;
	long double false_positives_1 = 0;
	long double false_negatives_0 = 0;
	long double false_negatives_1 = 0;
	long double num_labels = 0;
	
	int true_label_0;
	int true_label_1;
	int predict_label_0;
	int predict_label_1;
	
	int cols, rows;
	predict_out_0 >> cols >> rows;
	predict_out_1 >> cols >> rows;
	
	int num_pos = 0;
	int num_neg = 0;
	
	while(true_out_0 >> true_label_0 && true_out_1 >> true_label_1 
	&& predict_out_0 >> predict_label_0 && predict_out_1 >> predict_label_1) {
		if(true_label_0 == 0 && predict_label_0 == 1) {
			false_positives_0++;
		}
		if(true_label_0 == 1 && predict_label_0 == 0) {
			false_negatives_0++;
		}
		if(true_label_1 == 0 && predict_label_1 == 1) {
			false_positives_0++;
		}
		if(true_label_1 == 1 && predict_label_1 == 0) {
			false_negatives_0++;
		}
		if(true_label_0 == 0 && predict_label_1 == 1) {
			false_positives_1++;
		}
		if(true_label_0 == 1 && predict_label_1 == 0) {
			false_negatives_1++;
		}
		if(true_label_1 == 0 && predict_label_0 == 1) {
			false_positives_1++;
		}
		if(true_label_1 == 1 && predict_label_0 == 0) {
			false_negatives_1++;
		}
		if(true_label_0 == 0)
		{
			num_neg++;
		}
		else
		{
			num_pos++;
		}
		if(true_label_1 == 0)
		{
			num_neg++;
		}
		else
		{
			num_pos++;
		}
		num_labels+=2;
	}
	
	while(true_out_0 >> true_label_0) {
		if(true_label_0 == 1) {
			false_negatives_0++;
			false_negatives_1++;
			num_pos++;
		}
		else
		{
			num_neg++;
		}
		num_labels++;
	}
	
	while(true_out_1 >> true_label_1) {
		if(true_label_1 == 1) {
			false_negatives_0++;
			false_negatives_1++;
			num_pos++;
		}
		else
		{
			num_neg++;
		}
		num_labels++;
	}
	
	long double false_positives;
	long double false_negatives;
	
	if(false_positives_0 + false_negatives_0 < false_positives_1 + false_negatives_1) {
		cout << "Matched 0 to 0, 1 to 1" << endl;
		false_positives = false_positives_0;
		false_negatives = false_negatives_0;
	}
	else {
		cout << "Matched 0 to 1, 1 to 0" << endl;
		false_positives = false_positives_1;
		false_negatives = false_negatives_1;
	}
	
	cout << "Total labels: " << num_labels << endl;
	cout << "Positives: " << num_pos << endl;
	cout << "Negatives: " << num_neg << endl;
	cout << "False positives: " << false_positives << endl;
	cout << "False negatives: " << false_negatives << endl;
	cout << "False positives percentage: " << setprecision(3) << 100 * false_positives / num_labels << "%" << endl;
	cout << "False negatives percentage: " << setprecision(3) << 100 * false_negatives / num_labels << "%" << endl;
	double error = 100 * (false_positives + false_negatives) / num_labels;
	cout << "Total error: " << setprecision(3) <<  error << "%" << endl;
	cout << "Total raw accuracy: " << setprecision(3) <<  100 - error << "%" << endl;
	cout << "Total positive accuracy: " << setprecision(3) << 100 - 100 * false_negatives / num_pos << "%" << endl;
	cout << "Total negative accuracy: " << setprecision(3) << 100 - 100 * false_positives / num_neg << "%" << endl;	
	return 0;
}
