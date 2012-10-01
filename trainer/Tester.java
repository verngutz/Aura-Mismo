import java.util.*;
import java.io.*;

public class Tester
{
	public static final double SAMPLES_PER_SECOND = 44100;
	public static final double SAMPLES_PER_CHUNK = 1024;
	public static final double CHUNKS_PER_SECOND = SAMPLES_PER_SECOND / SAMPLES_PER_CHUNK;
	public static final double TICKS_PER_SECOND = 384;
	public static final double TICKS_PER_CHUNK = TICKS_PER_SECOND / CHUNKS_PER_SECOND;
	public static final double AUDIBLE_VOLUME = 55;
	
	public static final int PIANO = 1;
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
		// open the input mtx
		Scanner in = new Scanner(new File("test.mtx"));
		
		// create separate output files for each channel for the input file
		FileWriter[] out = new FileWriter[channels.length];
		for(int channelNum = 0; channelNum < channels.length; channelNum++)
		{
			out[channelNum] = new FileWriter(new File("test_" + channelNum + ".out"));
		}
		
		// each channel will have a map from a time to a (change in) note event info
		// TreeMap is used so that the time step can vary easily (just use floorEntry)
		TreeMap<Integer, boolean[]> noteEvents[] = new TreeMap[channels.length];
		for(int channelNum = 0; channelNum < channels.length; channelNum++)
		{
			noteEvents[channelNum] = new TreeMap<Integer, boolean[]>();
		}
		
		// whether or not to interpret a line as a midi event
		// initially false - only begin parsing midi events at the start of tracks (don't parse metadata)
		boolean parseTrack = false;
		while(in.hasNextLine())
		{
			String line = in.nextLine();
			
			// stop parsing at end of track
			if(line.equals("TrkEnd"))
			{
				parseTrack = false;
			}
			
			if(parseTrack)
			{
				Scanner lineScanner = new Scanner(line);
				int time = lineScanner.nextInt();
				String midiEvent = lineScanner.next();
				
				// only note on and off events are relevant for labelling
				if(midiEvent.equals("On"))
				{
					// parse channel
					String[] c = lineScanner.next().split("=");
					int channel = Integer.parseInt(c[1]);
					int channelIndex = Arrays.binarySearch(channels, channel);
					
					// make a new time point if there are no note events on this time yet
					if(!noteEvents[channelIndex].containsKey(time))
					{
						noteEvents[channelIndex].put(time, new boolean[channel == PIANO ? PIANO_NUMKEYCLASSES : drumAll.length]);
						Integer lower = noteEvents[channelIndex].lowerKey(time);	
						
						// copy previous note event info, if any
						if(lower != null)
						{
							for(int noteNum = 0; noteNum < (channel == PIANO ? PIANO_NUMKEYCLASSES : drumAll.length); noteNum++)
							{
								noteEvents[channelIndex].get(time)[noteNum] = noteEvents[channelIndex].get(lower)[noteNum];
							}
						}
					}
					
					// parse note
					String[] n = lineScanner.next().split("=");
					int note = Integer.parseInt(n[1]);
					
					// parse volume
					String[] v = lineScanner.next().split("=");
					int volume = Integer.parseInt(v[1]);
					
					if(channel == PIANO)
					{
						noteEvents[channelIndex].get(time)[note % PIANO_NUMKEYCLASSES] = volume > AUDIBLE_VOLUME;
					}
					else if(channel == DRUMS)
					{
						for(int drumGroup = 0; drumGroup < drumAll.length; drumGroup++)
						{
							if(Arrays.binarySearch(drumAll[drumGroup], note) >= 0)
							{
								noteEvents[channelIndex].get(time)[drumGroup] = volume > AUDIBLE_VOLUME;
							}
						}
					}
				}
			}
			
			// begin parsing at beginning of track
			if(line.equals("MTrk"))
			{
				parseTrack = true;
			}
		}
		
		/**
		// print debug info
		for(int channelNum = 0; channelNum < channels.length; channelNum++)
		{
			System.out.println("channel: " + channelNum);
			for(Map.Entry<Integer, boolean[]> a : noteEvents[channelNum].entrySet())
			{
				System.out.print(String.format("%1$5s", a.getKey()) + "\t\t");
				for(int i = 0; i < a.getValue().length; i++)
				{
					System.out.print(" " + (a.getValue()[i] ? "1" : "0"));
				}
				System.out.println();
			}
			System.out.println();
		}
		*/
		
		for(int channelNum = 0; channelNum < channels.length; channelNum++)
		{
			// create a label row per chunk
			for(int time = 0; ; time += TICKS_PER_CHUNK)
			{
				// get note event info at time nearest but lower than current chunk's time
				Map.Entry<Integer, boolean[]> noteEntry = noteEvents[channelNum].floorEntry(time);
				boolean[] turnedOn = (noteEntry == null ? new boolean[channels[channelNum] == PIANO ? PIANO_NUMKEYCLASSES : drumAll.length] : noteEntry.getValue());
				
				out[channelNum].write(turnedOn[0] ? "1" : "0");
				for(int noteNum = 1; noteNum < turnedOn.length; noteNum++)
				{
					out[channelNum].write(" " + (turnedOn[noteNum] ? "1" : "0"));
				}
				out[channelNum].write("\n");
				out[channelNum].flush();
				
				// stop writing output if all the notes are off and there are no more future note events
				boolean notesOn = false;
				for(int noteNum = 0; noteNum < turnedOn.length; noteNum++)
				{
					notesOn |= turnedOn[noteNum];
				}
				if(!notesOn && noteEvents[channelNum].higherKey(time) == null)
				{
					break;
				}
			}
			out[channelNum].close();
		}
	}
}