piping: piping.o
	g++ -o piping piping.cpp

piping.o: piping.cpp
	g++ -c piping.cpp

run:
	./piping

clean:
	rm *.o piping
