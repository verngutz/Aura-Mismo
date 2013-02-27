#include <fstream>
#include <vector>
#include <iostream>
using namespace std;

class note
{
public:
	int length;
	int parity;
};
	
void postprocess(int** psd0, int** psd1, int r, int c)
{
	// PSD to notes
	vector<note> notes0[c];
	vector<note> notes1[c];
	for(int i = 0; i < c; i++)
	{
		notes0[i] = vector<note>();
		notes1[i] = vector<note>();
	}
	
	for(int i = 0; i < r; i++)
	{
		for(int j = 0; j < c; j++)
		{
			
			if(notes0[j].size() == 0 || notes0[j][notes0[j].size()-1].parity != psd0[i][j])
			{
				note n;
				n.parity = psd0[i][j];
				n.length = 1;
				notes0[j].push_back(n);
			}
			else
			{
				notes0[j][notes0[j].size()-1].length++;
			}
			
			if(notes1[j].size() == 0 || notes1[j][notes1[j].size()-1].parity != psd1[i][j])
			{
				note n;
				n.parity = psd1[i][j];
				n.length = 1;
				notes1[j].push_back(n);
			}
			else
			{
				notes1[j][notes1[j].size()-1].length++;
			}
		}
	}
	
	// post processing heuristics
	for(int i = 0; i < c; i++)
	{
		for(int j = 0; j < notes0[i].size(); j++)
		{
			if(notes0[i][j].parity == 0 && notes0[i][j].length < 4)
			{
				notes0[i][j].parity = 1;
			}
		}
		
		/**
		for(int j = 0; j < notes0[i].size(); j++)
		{
			if(notes0[i][j].parity == 1 && notes0[i][j].length < 4)
			{
				notes0[i][j].parity = 0;
			}
		}
		*/
		
		for(int j = 0; j < notes1[i].size(); j++)
		{
			if(notes1[i][j].parity == 0 && notes1[i][j].length < 4)
			{
				notes1[i][j].parity = 1;
			}
		}
		
		/**
		for(int j = 0; j < notes1[i].size(); j++)
		{
			if(notes1[i][j].parity == 1 && notes1[i][j].length < 4)
			{
				notes1[i][j].parity = 0;
			}
		}
		*/
	}
	
	
	// notes to PSD
	for(int i = 0; i < r; i++)
	{
		for(int j = 0; j < c; j++)
		{
			int len0 = 0;
			int k;
			for(k = 0; k <= notes0[j].size(); k++)
			{
				if(len0 > i)
					break;
				len0 += notes0[j][k].length;
			}
			psd0[i][j] = notes0[j][k-1].parity;
			
			int len1 = 0;
			for(k = 0; k <= notes1[j].size(); k++)
			{
				if(len1 > i)
					break;
				len1 += notes1[j][k].length;
			}
			psd1[i][j] = notes1[j][k-1].parity;
		}
	}
	
	for(int i = 0; i < c; i++)
	{
		notes0[i] = vector<note>();
		notes1[i] = vector<note>();
	}
	
	for(int i = 0; i < r; i++)
	{
		for(int j = 0; j < c; j++)
		{
			
			if(notes0[j].size() == 0 || notes0[j][notes0[j].size()-1].parity != psd0[i][j])
			{
				note n;
				n.parity = psd0[i][j];
				n.length = 1;
				notes0[j].push_back(n);
			}
			else
			{
				notes0[j][notes0[j].size()-1].length++;
			}
			
			if(notes1[j].size() == 0 || notes1[j][notes1[j].size()-1].parity != psd1[i][j])
			{
				note n;
				n.parity = psd1[i][j];
				n.length = 1;
				notes1[j].push_back(n);
			}
			else
			{
				notes1[j][notes1[j].size()-1].length++;
			}
		}
	}
	
	// post processing heuristics
	for(int i = 0; i < c; i++)
	{
		/**
		for(int j = 0; j < notes0[i].size(); j++)
		{
			if(notes0[i][j].parity == 0 && notes0[i][j].length < 6)
			{
				notes0[i][j].parity = 1;
			}
		}
		*/
		
		for(int j = 0; j < notes0[i].size(); j++)
		{
			if(notes0[i][j].parity == 1 && notes0[i][j].length < 4)
			{
				notes0[i][j].parity = 0;
			}
		}
		
		/**
		for(int j = 0; j < notes1[i].size(); j++)
		{
			if(notes1[i][j].parity == 0 && notes1[i][j].length < 6)
			{
				notes1[i][j].parity = 1;
			}
		}
		*/
		
		for(int j = 0; j < notes1[i].size(); j++)
		{
			if(notes1[i][j].parity == 1 && notes1[i][j].length < 4)
			{
				notes1[i][j].parity = 0;
			}
		}
	}
	
	
	// notes to PSD
	for(int i = 0; i < r; i++)
	{
		for(int j = 0; j < c; j++)
		{
			int len0 = 0;
			int k;
			for(k = 0; k <= notes0[j].size(); k++)
			{
				if(len0 > i)
					break;
				len0 += notes0[j][k].length;
			}
			psd0[i][j] = notes0[j][k-1].parity;
			
			int len1 = 0;
			for(k = 0; k <= notes1[j].size(); k++)
			{
				if(len1 > i)
					break;
				len1 += notes1[j][k].length;
			}
			psd1[i][j] = notes1[j][k-1].parity;
		}
	}
	
	// weighted activity scheduling
	vector<note> input;
	vector<note> output;
}

int main()
{
	ofstream out0;
	out0.open("output0.txt");
	
	ofstream out1;
	out1.open("output1.txt");
	
	ifstream temp0;
	temp0.open("temp0.txt");
	ifstream temp1;
	temp1.open("temp1.txt");
	
	int r, c;
	temp0 >> c >> r;
	temp1 >> c >> r;
	
	int** psd0 = new int*[r];
	for(int i = 0; i < r; i++)
	{
		psd0[i] = new int[c];
	}
	int** psd1 = new int*[r];
	for(int i = 0; i < r; i++)
	{
		psd1[i] = new int[c];
	}
	
	for(int i = 0; i < r; i++) {
		for(int j = 0; j < c; j++) {
			temp0 >> psd0[i][j];
		}
	}
	
	for(int i = 0; i < r; i++) {
		for(int j = 0; j < c; j++) {
			temp1 >> psd1[i][j];
		}
	}
	
	postprocess(psd0, psd1, r, c);
	
	out0 << c << " " << r << endl;
	out1 << c << " " << r << endl;
	
	for(int i = 0; i < r; i++) {
		for(int j = 0; j < c; j++) {
			out0 << psd0[i][j] << " ";
		}
		out0 << endl;
	}
	
	for(int i = 0; i < r; i++) {
		for(int j = 0; j < c; j++) {
			out1 << psd1[i][j] << " ";
		}
		out1 << endl;
	}
}
