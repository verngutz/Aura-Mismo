#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;

int main() {
	ifstream piano_predict_out;
	ifstream drum_predict_out;
	
	piano_predict_out.open("output0.txt");
	drum_predict_out.open("output1.txt");
	
	ifstream piano_true_out;
	ifstream drum_true_out;
	
	piano_true_out.open("test_0.out");
	drum_true_out.open("test_1.out");
	
	long double false_positives = 0;
	long double false_negatives = 0;
	long double num_labels = 0;
	
	int true_label;
	int predict_label;
	
	int cols, rows;
	piano_predict_out >> cols >> rows;
	drum_predict_out >> cols >> rows;
	
	while(piano_true_out >> true_label && piano_predict_out >> predict_label) {
		if(true_label == 0 && predict_label == 1) {
			false_positives++;
		}
		if(true_label == 1 && predict_label == 0) {
			false_negatives++;
		}
		num_labels++;
	}
	
	while(piano_true_out >> true_label) {
		if(true_label == 1) {
			false_negatives++;
		}
		num_labels++;
	}
	
	while(piano_predict_out >> predict_label)	{
		if(predict_label == 1) {
			false_positives++;
		}
		num_labels++;
	}
	
	while(drum_true_out >> true_label && drum_predict_out >> predict_label) {
		if(true_label == 0 && predict_label == 1) {
			false_positives++;
		}
		if(true_label == 1 && predict_label == 0) {
			false_negatives++;
		}
		num_labels++;
	}
	
	while(drum_true_out >> true_label) {
		if(true_label == 1) {
			false_negatives++;
		}
		num_labels++;
	}
	
	while(drum_predict_out >> predict_label)	{
		if(predict_label == 1) {
			false_positives++;
		}
		num_labels++;
	}
	
	cout << "Total labels: " << num_labels << endl;
	cout << "False positives: " << false_positives << endl;
	cout << "False negatives: " << false_negatives << endl;
	cout << "False positives percentage: " << setprecision(3) << 100 * false_positives / num_labels << endl;
	cout << "False negatives percentage: " << setprecision(3) << 100 * false_negatives / num_labels << endl;
	cout << "Total error: " << setprecision(3) << 100 * (false_positives + false_negatives) / num_labels << endl;
	
	return 0;
}
