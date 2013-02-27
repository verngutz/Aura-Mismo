Aura-Mismo
==========

"Ora mismo" is a Filipino slang for "right now" or "at this instant." 
Aura Mismo is a tool for automatically extracting note information from an audio file, 
for use in a game like Guitar Hero or Rock Band, 
to deliver a game from any acquired or custom song, ora mismo.

I. Required Tools

	A. Windows Operating System (for automatically generating the training and testing data) with
		1. Java Compiler and Runtime Environment
	
	B. Linux Operating System (for compiling and running the ICA-SVM system) with
		1. C++ Compiler
		2. Python Interpreter

II. Required Programs (for automatic training and testing data generator, Windows)

	A. Mtx2Midi (included as executable)
	B. Midi2Mtx (included as executable)
	C. Timidity (included as executable)

III. Optional Programs (for automatic training and testing data generator, Windows)

	A. Noteworthy Composer (not included)
	B. Audacity (not included)

IV. Required Libraries (for ICA-SVM system, Linux)

	A. libsndfile (not included)
		http://www.mega-nerd.com/libsndfile/#Download
			or
		sudo apt-get install libsndfile1-dev

	B. FFTW 3 (not included)
		http://www.fftw.org/download.html
			or
		sudo apt-get install libfftw3-3

	C. LIBSVM (included as source, no need to build)
		http://www.csie.ntu.edu.tw/~cjlin/libsvm/

	D. NumPy
		http://sourceforge.net/projects/numpy/files/

	E. Python MDP (not included)
		http://mdp-toolkit.sourceforge.net/install.html
			or
		sudo apt-get install python-mdp

	F. Python scikits.audiolab (not included)
		https://pypi.python.org/pypi/scikits.audiolab/#downloads

V. Instructions for Use

	A. Generating the Training Set (Windows only)
		Note: Build directory already has ready-made Training Set: this part may be skipped.
		1. Go to the "trainer" directory.
		2. Run the "train.bat" script.
		3. Take a coffee break.
		4. Convert all the MIDI files in the "midi" folder to WAV using Timidity (some configuration may be necessary).
		5. Take a coffee break.
		6. Put all the WAV files into a "wav" folder.
		7. Copy the "wav" and "label" folders into the build folder.

	B. Generating the Test File (Windows only)
		Note: Build directory already has ready-made Test File: this part may be skipped.
		Note: Trainer directory already has ready-made MIDI files: steps 2-4 steps may be skipped.
		Note: Trainer directory already has ready-made WAV files: steps 5-6 may be skipped.
		1. Go to the "trainer" directory.
		2. Open the "test.nwc" file.
		3. Mute the bottom staff and save as "test_0.mid".
		4. Unmute the bottom staff, mute the top staff, and save as "test_1.mid".
		5. Convert the two MIDI files to WAV using Timidity (some configuration may be necessary).
		6. Mix down the two WAV files using Audacity and save it as "test.wav". 
			Take only one track from each WAV file, the one that plays earlier.
			Remember to pan each track properly.
		7. Run the "test.bat" script.
		8. Copy "test.wav", "test_0.out", and "test_1.out" into the build folder.

	C. Training and Testing (Linux only)
		1. Go the Aura-Mismo root directory.
		2. Run make.
		3. Go to the "build" directory.
		4. Run the "doit.sh" script.
		5. Take a coffee break.
		6. Open "output0.txt" and "output1.txt" to inspect the output notes.
