all:
	g++ -o build/trainingsetmaker trainingsetmaker.cpp -lsndfile -lfftw3
	g++ -o build/svmtrainer svmtrainer.cpp svm.h svm.cpp
	g++ -o build/testingwavwriter testingwavwriter.cpp -lsndfile -lfftw3
	g++ -o build/auramismo auramismo.cpp svm.h svm.cpp
	g++ -o build/evaluator evaluator.cpp
