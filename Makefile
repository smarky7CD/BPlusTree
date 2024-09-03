all:
	c++ main.cpp bptree.cpp -o bplustree -w --std=c++17

clean:
	rm bplustree
