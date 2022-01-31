bin/worker: bin/worker.o
	g++ bin/worker.o -o bin/worker

bin/worker.o: worker.cpp
	g++ -c worker.cpp -o bin/worker.o

clean:
	rm bin/*