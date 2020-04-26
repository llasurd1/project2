all: cache-sim	
proj2: cache-sim.cpp
	g++ -g cache-sim.cpp -o cache-sim

clean:
	rm -rf cache-sim
