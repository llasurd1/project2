all: predictors		

lab1: predictors.cpp
	g++ -g predictors.cpp -o predictors

clean:
	rm -rf predictors
