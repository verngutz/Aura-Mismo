all:
	g++ -o build/trainingsetmaker trainingsetmaker.cpp -lsndfile -lfftw3
	g++ -o build/svmtrainer svmtrainer.cpp svm.h svm.cpp
	g++ -o build/wavsplitter wavsplitter.cpp -lsndfile
	g++ -o build/testingwavwriter testingwavwriter.cpp -lsndfile -lfftw3
	g++ -o build/auramismo auramismo.cpp svm.h svm.cpp
	g++ -o build/postprocess postprocess.cpp
	g++ -o build/evaluator evaluator.cpp
	cp ica.py build/ica.py
	cp doit.sh build/doit.sh
	cp normalize.py build/normalize.py
