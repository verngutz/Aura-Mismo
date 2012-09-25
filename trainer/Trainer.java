import java.util.*;
import java.io.*;

public class Trainer
{
	public static final int TRAINING_SIZE = 1500;
	
	public static final int PIANO = 1;
	public static final int PIANO_LOWEST = 21;
	public static final int PIANO_NUMKEYS = 88;
	public static final int PIANO_NUMKEYCLASSES = 12;
	
	public static final int DRUMS = 10;
	public static final int[] drumBass = {35, 36};
	public static final int[] drumSnare = {38, 39, 40};
	public static final int[] drumCrash = {49, 55, 57};
	public static final int[] drumHat = {42, 44, 46};
	public static final int[] drumTom = {41, 43, 45, 47, 48, 50};
	public static final int[][] drumAll = { drumBass, drumSnare, drumCrash, drumHat, drumTom };
	
	public static final int[] channels = {PIANO, DRUMS};
	
	public static void main(String args[]) throws IOException
	{	
		// create data set
		for(int dataNum = 0; dataNum < TRAINING_SIZE; dataNum++)
		{	
			// each item in the data set is two files: an mtx and a label
			// mtx is a text format that can be converted to midi, contains the music (see Mtx2Midi and Midi2Mtx)
			// label contains the correct label for the produced mtx
			FileWriter mtx = new FileWriter(new File(dataNum + ".mtx"));
			FileWriter[] label = new FileWriter[channels.length];
			for(int channelNum = 0; channelNum < channels.length; channelNum++)
			{
				label[channelNum] = new FileWriter(new File(dataNum + "_" + channelNum + ".label"));
			}
			
			// determine how many notes should play for each instrument/channel
			int[] sizes = new int[channels.length];
			for(int channelNum = 0; channelNum < channels.length; channelNum++)
			{
				// first notes in sample are solo piano and drum notes
				if(dataNum < 88)
				{
					if(channels[channelNum] == PIANO)
					{
						sizes[channelNum] = 1;
					}
					else if(channels[channelNum] == DRUMS)
					{
						sizes[channelNum] = 0;
					}
				}
				else if(dataNum < 105)
				{
					if(channels[channelNum] == PIANO)
					{
						sizes[channelNum] = 0;
					}
					else if(channels[channelNum] == DRUMS)
					{
						sizes[channelNum] = 1;
					}
				}
				else
				{
					// generate a biased random number of notes
					sizes[channelNum] = (int)(((Math.random() * 12) + (Math.random() * 6) + (Math.random() * 3)) / 3);
				}
			}
			
			// build the array of notes per instrument/channel
			int[][] notes = new int[channels.length][];
			for(int channelNum = 0; channelNum < channels.length; channelNum++)
			{
				notes[channelNum] = new int[sizes[channelNum]];
			}
			
			// begin writing mtx
			mtx.write("MFile 1 " + channels.length + " 384\nMTrk\n0 TimeSig 4/4 24 8\n0 Tempo 500000\n0 Meta 0x21 00\n0 PrCh ch=1 p=0\n");
			
			for(int channelNum = 0; channelNum < channels.length; channelNum++)
			{
				// begin track
				if(channelNum != 0)
				{
					mtx.write("MTrk\n");
					mtx.write("0 Meta 0x21 00\n");
				}
				
				mtx.write("0 Par ch=" + channels[channelNum] + " c=7 v=127\n");
				mtx.write("0 Par ch=" + channels[channelNum] + " c=10 v=64\n");
				
				
				// write a sizes[channelNum] number of random notes for the current channel
				for(int noteNum = 0; noteNum < sizes[channelNum]; noteNum++)
				{
					int midiNum = 0;
					
					// choose any note from the 88 keys of the piano
					if(channels[channelNum] == PIANO)
					{
						// first notes in sample are solo piano notes
						if(dataNum < 88)
						{
							midiNum = PIANO_LOWEST + dataNum;
						}
						else if(dataNum >= 105)
						{
							midiNum = PIANO_LOWEST + (int)(Math.random() * PIANO_NUMKEYS);
						}
					}
					
					// choose a note from the supported drum notes
					else if(channels[channelNum] == DRUMS)
					{
						if(dataNum < 88)
						{
							midiNum = 0;
						}
						else if(dataNum < 105)
						{
							switch(dataNum)
							{
								case 88:
									midiNum = drumAll[0][0];
									break;
								case 89:
									midiNum = drumAll[0][1];
									break;
								case 90:
									midiNum = drumAll[1][0];
									break;
								case 91:
									midiNum = drumAll[1][1];
									break;
								case 92:
									midiNum = drumAll[1][2];
									break;
								case 93:
									midiNum = drumAll[2][0];
									break;
								case 94:
									midiNum = drumAll[2][1];
									break;
								case 95:
									midiNum = drumAll[2][2];
									break;
								case 96:
									midiNum = drumAll[3][0];
									break;
								case 97:
									midiNum = drumAll[3][1];
									break;
								case 98:
									midiNum = drumAll[3][2];
									break;
								case 99:
									midiNum = drumAll[4][0];
									break;
								case 100:
									midiNum = drumAll[4][1];
									break;
								case 101:
									midiNum = drumAll[4][2];
									break;
								case 102:
									midiNum = drumAll[4][3];
									break;
								case 103:
									midiNum = drumAll[4][4];
									break;
								case 104:
									midiNum = drumAll[4][5];
									break;
							}
						}
						else
						{
							do
							{
								int drumGroup = (int)(Math.random() * drumAll.length);
								int drumMember = (int)(Math.random() * drumAll[drumGroup].length);
								midiNum = drumAll[drumGroup][drumMember];
							}
							while(Arrays.asList(notes[channelNum]).contains(midiNum));
						}
					}
					notes[channelNum][noteNum] = midiNum;
					
					// turn the note on
					mtx.write("0 On ch=" + channels[channelNum] + " n=" + midiNum + " v=127\n");
				}
				
				// turn the notes off
				for(int noteNum = 0; noteNum < sizes[channelNum]; noteNum++)
				{
					mtx.write("384 On ch=" + channels[channelNum] + " n=" + notes[channelNum][noteNum] + " v=0\n");
				}
				
				// end track
				mtx.write("384 Meta TrkEnd\nTrkEnd\n");
				mtx.flush();
			}
			
			mtx.close();	
			
			// raw note data post-processing before labelling
			for(int channelNum = 0; channelNum < channels.length; channelNum++)
			{
				// classify piano notes into key classes
				if(channels[channelNum] == PIANO)
				{
					for(int noteNum = 0; noteNum < sizes[channelNum]; noteNum++)
					{
						notes[channelNum][noteNum] %= PIANO_NUMKEYCLASSES;
					}
				}
			}
			
			// begin automatic labelling
			for(int channelNum = 0; channelNum < channels.length; channelNum++)
			{
				// sort to be able to use binary search function
				Arrays.sort(notes[channelNum]);
				
				// for each key class, "YES" label if a note belonging to that class is present in the current midi
				if(channels[channelNum] == PIANO)
				{
					for(int keyClass = 0; keyClass < PIANO_NUMKEYCLASSES; keyClass++)
					{
						label[channelNum].write(Arrays.binarySearch(notes[channelNum], keyClass) >= 0 ? "1 " : "0 ");
					}
				}
				
				// for each drum group, "YES" label if a note belonging to that group is present in the current midi
				else if(channels[channelNum] == DRUMS)
				{
					for(int drumGroup = 0; drumGroup < drumAll.length; drumGroup++)
					{
						boolean found = false;
						for(int drumMember = 0; drumMember < drumAll[drumGroup].length; drumMember++)
						{
							if(Arrays.binarySearch(notes[channelNum], drumAll[drumGroup][drumMember]) >= 0)
							{
								found = true;
								break;
							}
						}
						label[channelNum].write(found ? "1 " : "0 ");
					}
				}
				label[channelNum].write("\n");
				label[channelNum].flush();
				label[channelNum].close();
			}
		}
	}
}