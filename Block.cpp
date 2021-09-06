#include <vector>
#include <iostream>
#include "Record.cpp"
using namespace std;

class Block {
    private:
        Record ** records;
        int MAX_NUM_RECORDS;

        //Get the next empty spot to add record to
        int findEmptyIndex(){
            for( int i=0; i<this->MAX_NUM_RECORDS; ++i ) {
                if (!records[i]){ //if pointer is null
                    return i;
                }
            }
            return -1;
        }

    public:
        Block(int maxNumRecords){
            this->MAX_NUM_RECORDS = maxNumRecords;
            this->records = new Record*[100]();
        }

        //Get total number of records in a block
        int getSize(){
            int size = 0;
            for( int i=0; i<this->MAX_NUM_RECORDS; ++i ) {
                if (records[i]){ //if pointer is not null
                    size++;
                }
            }
            return size;
        }

        //Add new record and return recordIndex of newly added record
        int addRecord(string tconst, double averageRating, int numVotes){
            if (this->getSize()<this->MAX_NUM_RECORDS){
                int emptyIndex = this->findEmptyIndex();
                if (emptyIndex != -1){
                    records[emptyIndex] = new Record(tconst,averageRating,numVotes);
                    return emptyIndex; //return index so that we can reference it later
                } else {
                    cout << "There are not empty spaces in this block!\n";
                    return -1;
                }
            }
            else {
                cout << "The block you are trying to add to is full!\n";
                return -1;
            }
        }

        //Delete record using the recordIndex
        void deleteRecord(int recordIndex){
            delete this->records[recordIndex];
            this->records[recordIndex] = NULL;
        }

        //Get the record pointer using recordIndex
        Record * getRecord(int recordIndex){
            if (recordIndex < this->MAX_NUM_RECORDS){
                return records[recordIndex];
            }
            else {
                return NULL;
            }
        }

        //To check if we can add anymore records to this block
        bool isFull(){
            return (getSize()>=MAX_NUM_RECORDS);
        }

        ~Block(){
            for( int i=0; i<this->MAX_NUM_RECORDS; ++i ) {
                if (records[i]){ //if pointer is not null
                    delete records[i];
                }
            }
            delete records;
        }

};

//Checks
int main(){
    Block block1(3);

    //Test adding record to a block
    cout << "size: " << block1.getSize()<<"\n";
    block1.addRecord("sdfas", 123.123, 1232314);
    cout << "size: " << block1.getSize()<<"\n";
    cout << block1.getRecord(0)->toString();
    block1.addRecord("sdfas2", 123.123, 1232314);
    cout << "size: " << block1.getSize()<<"\n";
    cout << block1.getRecord(1)->toString();

    //Test deleting a record
    block1.deleteRecord(0);
    cout << "size: " << block1.getSize()<<"\n";
    cout << block1.getRecord(1)->toString();

    //Test that it adds to the first empty spot
    block1.addRecord("sdfas3", 123.123, 1232314);
    cout << "size: " << block1.getSize()<<"\n";
    cout << block1.getRecord(0)->toString();

    block1.addRecord("sdfas4", 123.123, 1232314);
    cout << "size: " << block1.getSize()<<"\n";
    
    //Make sure that it doesn't add more than the limit
    block1.addRecord("sdfa5", 123.123, 1232314);
    cout << "size: " << block1.getSize()<<"\n";
    
    return 0;
}
