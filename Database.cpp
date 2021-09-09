//Written by LR
//Stores both btree index and the disk
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "Tree.cpp"
#include "Disk.cpp"

class Database {
    private:
        string filename; //tsv data we want to import
        Disk disk; //for now 3 for testing
        bTree btree;

        vector<string> split(string stringToSplit){
            char delimiter = '\t';
            vector<string> result;
            int indexOfLastDelimiter = -1;
            for (int i=0;i<stringToSplit.size();i++){
                if (stringToSplit[i] == delimiter){
                    string substring = stringToSplit.substr(indexOfLastDelimiter+1, i-indexOfLastDelimiter);
                    result.push_back(substring);
                    indexOfLastDelimiter = i;
                } else if (i==(stringToSplit.size()-1)){
                    string substring = stringToSplit.substr(indexOfLastDelimiter+1, i-indexOfLastDelimiter);
                    result.push_back(substring);
                }
            }
            return result;
        }

    public:
        Database(string filename, int n) : filename(filename), btree(n){}

        //For experiment 1:
        //Experiment 1: Store the data and report the following statistics:
        //- the number of blocks
        //- the size of database (in terms of MB)
        void addAllRecordsWithNoIndex(){
            //read file
            ifstream file(this->filename);
            //read each line from the tsv file
            string line;
            int i = 0;
            while (getline (file, line)) {
                if (i==0){ //ignore header
                    i++;
                    continue;
                }
                this->disk.insert(line); //add tuple to disk
            }
            file.close();
            //number of blocks output
            int numBlocks = disk.getTotalBlocks();
            cout << "Total Number of Blocks: " << numBlocks <<"\n";
            //the size of database (in terms of MB)
            int blocksize = 100;
            float mb = (float(numBlocks * blocksize) / float(1000000));
            cout <<  "Size of database (in terms of MB): " << mb << "\n";
        }

        void addToDiskAndBplus(){
            //read file
            ifstream file(this->filename);
            //read each line from the tsv file
            string line;
            int i = 0;
            while (getline (file, line)) {
                if (i==0){ //ignore header
                    i++;
                    continue;
                }
                void * pointer = this->disk.insert(line);
                int key = stoi(split(line)[2]);
                this->btree.insertToBTree(key,&i);
            }
            file.close();
        }


//         Experiment 2: build a B+ tree on the attribute "numVotes" by inserting the
//          records sequentially and report the following statistics:
//          - the parameter n of the B+ tree
//          - the number of nodes of the B+ tree
//          - the height of the B+ tree, i.e., the number of levels of the B+ tree
//          - the content of the root node and its 1st child node
        void experiment2(){
            cout << "Parameter, n: " << btree.getN() << "\n";
            cout << "Number Of Nodes in B+ Tree: " << btree.getNumberOfNodes() <<"\n";
            cout << "Height of B+ tree: " << btree.getHeight() <<"\n";
            cout << "Content of root node: ";
            btree.getRoot()->printAllKeys();
            cout << "\n";
            cout << "Content of 1st child node: ";
            Node * firstChild = (Node *)btree.getRoot()->children[0];
            firstChild->printAllKeys();
            cout << "\n";
        }

        void printBlocks(){
            this->disk.printAllRecord();
        }

        void printTree(){
            this->btree.printNodeTree();
        }
    
};
