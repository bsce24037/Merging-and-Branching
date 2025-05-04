all: ./a.out

compRun:
	g++ -std=c++17 madina.cpp bank.cpp -o r.out -lnlohmann_json

compTest:
	g++ -std=c++11 test.cpp bank.cpp -o a.out

test: clean compTest; ./a.out

run: clean compRun; ./r.out

clean:
	rm -f *.out
