#include "bptree.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;
int main (int argc,char* argv[])
{

    if(argc != 2){
        cerr << "Call program as:" << endl;
        cerr << "./bplustree file_name" << endl;
        return 1;
    }

    // get input filename
    string input_fn(argv[1]);

    // save commands from input to string of vectors
    ifstream input_stream;
    input_stream.open(input_fn,ios::in);
    vector<string> commands;
    string command;
    // save one command at a time
    while(getline(input_stream,command)){
        commands.push_back(command);
    }

    // close input file
    input_stream.close();

    // create output stream
    ofstream out;
    out.open("output_file.txt",ios::out);
    // get init string as first line
    string init_str = commands.front();
    // init
    size_t start = init_str.find_first_of("(");
    size_t end = init_str.find_first_of(")");
    int m = stoi(init_str.substr(start+1,end-start-1));
    BPTree t(m);

    // go line by line
    for(size_t i = 1; i < commands.size(); i++){     
        string s = commands[i];

        // use rfind to see what command we are dealing with 
        if(s.rfind("Insert",0) == 0){
            // insert
            size_t start = s.find_first_of("(");
            size_t middle = s.find_first_of(",");
            size_t end = s.find_first_of(")");
            int key = stoi(s.substr(start+1,middle-start-1));
            double value = stod(s.substr(middle+1,end-middle-1));
            t.insert(key,value);
        }else if(s.rfind("Delete",0) == 0){
            // delete
            size_t start = s.find_first_of("(");
            size_t end = s.find_first_of(")");
            int key = stoi(s.substr(start+1,end-start-1));
            t.del(key);
        }else if (s.rfind("Search",0) == 0){
            // search single key 
            //       or
            // range search
            size_t start = s.find_first_of("(");
            size_t middle = s.find_first_of(",");
            size_t end = s.find_first_of(")");
            // signle key case
            if(middle == string::npos){
                int key = stoi(s.substr(start+1,middle-start-1));
                double val = t.search(key);
                // check if null otherwise write out value
                if(isnan(val)){
                    out << "Null" << endl;
                }else{
                    out << val << endl;
                }
            }else{
                int key1 = stoi(s.substr(start+1,middle-start-1));
                int key2 = stoi(s.substr(middle+1,end-middle-1));
                vector<double> result = t.search(key1,key2);
                // check if null otherwise write out values
                if(result.empty()){
                    out << "Null" << endl;
                }else{
                    stringstream ss;
                    for(size_t i =0; i < result.size(); ++i){
                        if(i != 0){
                            ss << ",";
                        }
                        ss << result[i];
                    }
                    out << ss.str() << endl;
                }
            }
        }else{
            cerr << "Unrecognized command" << endl;
        }
    }

    // close output file
    out.close();

    return 0;
}