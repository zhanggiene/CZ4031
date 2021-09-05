
#include <iostream>
#include <string>
#include <sstream>

using namespace std;
class Record {       
    private:             
        string tconst;        
        double averageRating;
        int numVotes;
    public:
        static const int RECORD_SIZE = 22; //10+8+4

        Record(string tconst, double averageRating, int numVotes){
            this->tconst = tconst;
            this->averageRating = averageRating;
            this->numVotes = numVotes;
        }

        void setTconst(string tconst){
            this->tconst = tconst;
        }

        void setAverageRating(double averageRating){
            this->averageRating = averageRating;
        }

        void setNumVotes(int numVotes){
            this->numVotes = numVotes;
        }

        string getTconst(){
            return this->tconst;
        }

        double getAverageRating(){
            return this->averageRating;
        }

        int getNumVotes(){
            return this->numVotes;
        }

        string toString(){
            ostringstream out;
            out << this->tconst << "\t" << this->averageRating << "\t" << this->numVotes<<"\n";
            return out.str();
        }
};
