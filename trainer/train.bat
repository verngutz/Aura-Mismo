ECHO Deleting existing files...
rm -r label
rm -rf mtx
rm -rf midi
ECHO Compiling Training Set Mtext Generator...
javac Trainer.java
ECHO Running Training Set Mtext Generator...
java Trainer
ECHO Converting Training Set Mtext to MIDI...
FOR /L %%A IN (0,1,499) DO echo.|Mtx2Midi %%A.mtx
mkdir midi
mv *.mid midi
mkdir mtx
mv *.mtx mtx
mkdir label
mv *.label label
