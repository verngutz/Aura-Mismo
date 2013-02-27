from mdp import fastica
from scikits.audiolab import wavread, wavwrite
from numpy import abs, max
 
# Load in the stereo file

#for i in range(0, 1500):
#	recording, fs, enc = wavread('piano/wav/' + str(i) + '.wav')
#	recording /= (5 * max(abs(recording), axis = 0))
#	wavwrite(recording, 'piano/wav/' + str(i) + '.wav', fs, enc)
	
#for i in range(0, 1500):
#	recording, fs, enc = wavread('clarinet/wav/' + str(i) + '.wav')
#	recording /= (5 * max(abs(recording), axis = 0))
#	wavwrite(recording, 'clarinet/wav/' + str(i) + '.wav', fs, enc)

for i in range(0, 6000):
	recording, fs, enc = wavread('wav/' + str(i) + '.wav')
	recording /= (5 * max(abs(recording), axis = 0))
	wavwrite(recording, 'wav/' + str(i) + '.wav', fs, enc)
