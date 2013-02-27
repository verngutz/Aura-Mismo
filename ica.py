from mdp import fastica
from scikits.audiolab import wavread, wavwrite
from numpy import abs, max
 
# Load in the stereo file
recording, fs, enc = wavread('test.wav')
 
# Perform FastICA algorithm on the two channels
sources = fastica(recording)
 
# The output levels of this algorithm are arbitrary, so normalize them to 1.0.
sources /= (5 * max(abs(sources), axis = 0))
 
# Write back to a file
wavwrite(sources, 'testout.wav', fs, enc)
