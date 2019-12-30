#include <stdlib.h>
#include <vector>
#include <iostream>
using namespace std;

#include "fusionVectorSort.hpp"

void printValuesWithIndexes(vector<int> values, vector<int>basicIndexes){
    for (int i =0; i < values.size(); i+= 1){
        cout<<values[i]<< " : "<<basicIndexes[i]<<",  ";
        if(i%10 == 9){
            cout<<endl;
        }
    }
}
bool vectorTrie(vector<int> vec){
    bool right = true;
    for(int i = 0; i < vec.size()-1; i += 2){
        if(!(vec[i] < vec[i+1])){
            right = false;
        }
    }
    return right;
}

void testTriFusion(){
    const int size =100;
    vector<int> values = vector<int>(size);
    vector<int> basicIndexes = vector<int>(size);
    for (int i = 0; i < size; i+=1){
        basicIndexes[i] = i;
        values[i]= rand();
    }
    printValuesWithIndexes(values,basicIndexes);
    cout<<"before triFusion"<<endl;
    triFusion(values,basicIndexes);
    cout<<"after triFusion"<<endl;
    printValuesWithIndexes(values,basicIndexes);
    if (vectorTrie(values)){
        std::cout<<"good"<<std::endl;
    }
    else{
        std::cout<<"bad"<<std::endl;
    }
}

int main(){
    
    testTriFusion();
}