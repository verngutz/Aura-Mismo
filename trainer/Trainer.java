import java.util.*;
import java.io.*;

public class Trainer
{
	public static void main(String args[]) throws IOException
	{
		for(int i = 0; i < 500; i++)
		{	
			FileWriter blah = new FileWriter(new File(i+".mtx"));
			FileWriter label = new FileWriter(new File(i+".label"));
			int size = 1 + (int)(((Math.random() * 12) + (Math.random() * 6) + (Math.random() * 3)) / 3);
			int[] notes = new int[size];
			
			blah.write("MFile 0 1 384\nMTrk\n0 TimeSig 4/4 24 8\n0 Tempo 500000\n0 PrCh ch=1 p=0\n");
			for(int j = 0; j < size; j++)
			{
				int x = 21 + (int)(Math.random() * 88);
				notes[j] = x;
				blah.write("0 On ch=1 n="+x+" v=96\n");
			}
			for(int j = 0; j < size; j++)
			{
				blah.write("384 On ch=1 n="+notes[j]+" v=0\n");
			}
			blah.write("384 Meta TrkEnd\nTrkEnd\n");
			blah.close();	
			
			int[] yes = new int[size];
			for(int j = 0; j < size; j++)
			{
				yes[j] = notes[j]%12;
			}
			Arrays.sort(yes);
			for(int j = 0; j < 12; j++)
			{
				label.write( Arrays.binarySearch(yes, j) >= 0 ? "1\n" : "-1\n");
			}
			label.close();
		}
	}
}