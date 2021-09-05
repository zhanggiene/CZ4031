#include <vector>
#include "Record.cpp"
using namespace std;

class Block {
    private:
        vector<Record*> records;

    public:
        int MAX_NUM_RECORDS;

        Block(int maxNumRecords){
            MAX_NUM_RECORDS = maxNumRecords;
        }

        int addRecord(string tconst, double averageRating, int numVotes){
            if (records.size()<MAX_NUM_RECORDS){
                records.push_back(new Record(tconst, averageRating, numVotes));
                return records.size()-1; //so that we can reference it later
            }
            else {
                cout << "The block you are trying to add to is full!\n";
            }
        }

        void deleteRecord(int recordIndex){
            this->records.erase(records.begin()+recordIndex);
        }

        Record* getRecord(int recordIndex){
            if (recordIndex<records.size()){
                return this->records[recordIndex];
            } else {
                cout << "Index is out of range\n";
                return NULL;
            }
        }

        //to check if we can add anymore
        bool isFull(){
            return (records.size()>=MAX_NUM_RECORDS);
        }

        ~Block(){
            for( int i=0; i<records.size(); ++i ) {
                delete records[i];
            }
        }

};