import java.util.*;
import java.io.*;

public class Mtext
{
	public static void main(String args[]) throws IOException
	{
		for(int i = 0; i < 50; i++)
		{
			FileWriter[] labels = new FileWriter[]
			{
				new FileWriter(new File(i+"_"+"C.label")),
				new FileWriter(new File(i+"_"+"CS.label")),
				new FileWriter(new File(i+"_"+"D.label")),
				new FileWriter(new File(i+"_"+"DS.label")),
				new FileWriter(new File(i+"_"+"E.label")),
				new FileWriter(new File(i+"_"+"F.label")),
				new FileWriter(new File(i+"_"+"FS.label")),
				new FileWriter(new File(i+"_"+"G.label")),
				new FileWriter(new File(i+"_"+"GS.label")),
				new FileWriter(new File(i+"_"+"A.label")),
				new FileWriter(new File(i+"_"+"AS.label")),
				new FileWriter(new File(i+"_"+"B.label"))
			};
			
			FileWriter blah = new FileWriter(new File(i+".mtx"));
			int size = 1;
			int[] notes = new int[size];
			
			blah.write("MFile 0 1 384\nMTrk\n0 TimeSig 4/4 24 8\n0 Tempo 500000\n0 PrCh ch=1 p=0\n");
			for(int j = 0; j < size; j++)
			{
				int x = 24 + (int)(Math.random() * 60);
				notes[j] = x;
				blah.write("0 On ch=1 n="+x+" v=96\n");
			}
			for(int j = 0; j < size; j++)
			{
				blah.write("192 On ch=1 n="+notes[j]+" v=0\n");
			}
			blah.write("192 Meta TrkEnd\nTrkEnd\n");
			blah.close();	
			
			int[] yes = new int[size];
			for(int j = 0; j < size; j++)
			{
				yes[j] = notes[j]%12;
			}
			Arrays.sort(yes);
			for(int j = 0; j < 12; j++)
			{
				labels[j].write( Arrays.binarySearch(yes, j) >= 0 ? "1\n" : "-1\n");	
				labels[j].close();
			}
		}
	}
}