# B+ Tree

An implementation of a internal-memory m-way B+ tree in C++. The data structure assumes no duplicates, but to handle errors simply ignores duplicate insertions.

## Documentation

The API and function prototypes are detailed in `function_prototypes.txt`.

## Tests

We have tests in the `tests` directory.

After compiling, they can be run as:

```
make 
./bplustree tests/test.txt
./bplustree tests/test2.txt
./bplustree tests/test3.txt
```

The results will be output to a file called `output_file.txt` in the top level directory.

The results should match:

```
121.56
3.55,-3.95
Null
-3.95
3.55,0.02,-3.95,-3.91,3.55,-109.23,3.26
3.55,0.02,-3.91
Null
```
