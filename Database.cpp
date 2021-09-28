//Written by LR
//Stores both btree index and the disk
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <functional>
#include <algorithm>
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
            int blocksize = disk.getBlockSizeinByte();
            float mb = (float(numBlocks * blocksize) / float(1024*1024));
            cout <<  "Size of database (in terms of MB): " << mb << "\n";
        }

        void addToDiskAndBplus(){
            //read file
            ifstream file(this->filename);
            int i = 0;
            //read each line from the tsv file
            string line;
            while (getline (file, line)) {
                if (i==0){ //ignore header
                    i++;
                    continue;
                }
                void * pointer = this->disk.insert(line);
                int key = stoi(split(line)[2]);
                this->btree.insertToBTree(key,pointer);
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

        void experiment3()
        {
            vector<pair<int,int> > result=btree.search(1000);
            cout<<endl;
            float SUM=0;

            vector<float> allRatings;
            vector<float> allnumVotes;
            cout<<"Size of result: "<<result.size()<<"\n";
            for(auto x: result)
            {
                cout<< "<"<<x.first<<","<<x.second<<">"<<",";
                float temp=disk.getRecord(x.first,x.second).rating;
                int temp2=disk.getRecord(x.first,x.second).numVotes;
                allRatings.push_back(temp);
                allnumVotes.push_back(temp2);
            }
            cout << endl;
            for(auto x: allRatings){
                SUM+=x;
                cout <<x<<",";
            } 
            cout << endl;
            for(auto x: allnumVotes){
                cout <<x<<",";
            }
            cout <<"sum:" << SUM<<endl;
            cout<<"Number of records: "<<allRatings.size()<<"\n";
            cout<<"Average of \"averageRating\"s of the records: "<<SUM/allRatings.size()<<"\n";
        }

         void experiment4()
        {


            vector<pair<int,int> > result=btree.searchRange(30000,40000);
            cout<<endl;
            float SUM=0;

            vector<float> allRatings;
            vector<float> allnumVotes;
            cout<<"size of result is"<<result.size();
            for(auto x: result)
            {
                float temp=disk.getRecord(x.first,x.second).rating;
                allRatings.push_back(temp);
                int temp2=disk.getRecord(x.first,x.second).numVotes;
                allnumVotes.push_back(temp2);
            }
            for(auto x: allRatings){
                SUM+=x;
                cout <<x<<",";
            } 
            cout << endl;
            for(auto x: allnumVotes){
                cout <<x<<",";
            }
            cout <<"sum:" << SUM<<endl;
            cout<<"number of records is"<<allRatings.size();
            cout<<"the average of “averageRating’s” of the records: is "<<SUM/allRatings.size();
        }


         void experiment5()
        {


             vector<pair<int,int> > result=btree.search(1000);
            cout<<endl;
            cout<<"Size of result: "<<result.size()<<"\n";

            for (int i=0;i<result.size();i++)
            {
                btree.deleteOneKey(1000);
                auto temp=btree.search(1000);
                cout<<i<<endl;
                cout<<"Size of result: "<<temp.size()<<"\n";


            }
        }


        void printBlocks(){
            this->disk.printAllRecord();
        }

        void printTree(){
            this->btree.printNodeTree();
        }

        void printLastRowOfPointers(){
            this->btree.printLastRowPointers();
            cout << endl;
        }

        void printAllRecordsAccordingToIndex(){
            cout << "printAllRecordsAccordingToIndex"<<endl;
            vector<void *> children =  this->btree.returnLastRowPointers();
            for (auto child:children){
                pair<int,int>* castedChild = (pair<int,int>*) child;
                cout<< "<"<<castedChild->first<<","<<castedChild->second<<">"<<": ";
                cout<<disk.getRecord(castedChild->first,castedChild->second).toString();
            }
        }

         void printAllRecords(){
            cout << "printAllRecords"<<endl;
            vector<void *> children =  this->btree.returnLastRowPointers();
            cout <<"|";
            for (int i=0;i<children.size();i++){
                pair<int,int>* castedChild = (pair<int,int>*) children[i];
                cout<<disk.getRecord(castedChild->first,castedChild->second).getNumVotes()<<"|";
            }
            cout<<endl;
        }

    
};
