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
	public static final int NUM_KEYS = 12;
	public static void main(String args[]) throws IOException
	{
		Scanner in = new Scanner(new File("test.mtx"));
		TreeMap<Integer, boolean[]> noteEvents = new TreeMap<Integer, boolean[]>();
		boolean parseTrack = false;
		while(in.hasNextLine())
		{
			String line = in.nextLine();
			if(line.equals("TrkEnd"))
			{
				parseTrack = false;
			}
			if(parseTrack)
			{
				Scanner liner = new Scanner(line);
				int time = liner.nextInt();
				String sig = liner.next();
				if(sig.equals("On"))
				{
					Integer lower = noteEvents.lowerKey(time);
					Integer higher = noteEvents.higherKey(time);
					if(!noteEvents.containsKey(time))
					{
						noteEvents.put(time, new boolean[NUM_KEYS]);
					}
					String channel = liner.next();
					String[] n = liner.next().split("=");
					int note = Integer.parseInt(n[1]);
					String[] v = liner.next().split("=");
					int volume = Integer.parseInt(v[1]);
					noteEvents.get(time)[note % NUM_KEYS] = volume > AUDIBLE_VOLUME;
				}
			}
			if(line.equals("MTrk"))
			{
				parseTrack = true;
			}
		}
		for(int i = 0; ; i += TICKS_PER_CHUNK)
		{
			boolean[] output = noteEvents.floorEntry(i).getValue();
			System.out.print(output[0] ? 1 : 0);
			for(int j = 1; j < NUM_KEYS; j++)
			{
				System.out.print(" " + (output[j] ? 1 : 0));
			}
			System.out.println();
			boolean notesOn = false;
			for(int j = 0; j < NUM_KEYS; j++)
			{
				notesOn |= output[j];
			}
			if(!notesOn && noteEvents.higherKey(i) == null)
			{
				break;
			}
		}
	}
}