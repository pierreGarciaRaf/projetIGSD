#include <vector>
#include <string>
#include <iostream>
#include <fstream>
using namespace std;

#include "fileReader.hpp"
#include "fusionVectorSort.hpp"

void printTeamHistory(teamHistory th)
{
    cout << th.name + " : ";
    for (int matchIndex = 0; matchIndex < th.ranks.size(); matchIndex += 1)
    {
        cout << '{' << th.ranks[matchIndex] << ',' << th.points[matchIndex] << "}";
    }
    cout << endl;
}
/**
 * classe les équipes selon la formule pour la hauteur à un temps donné.
 */
void sortAtTime(vector<teamHistory> &th, int time,int maxRank, int maxPoint){
    
    vector<int> scores = vector<int>(th.size());
    vector <int> basicIndexes = vector<int>(th.size());
    //ici on garde l'information de hauteur en entiers dans le vecteur scores. 
    // en fait on multiplie l'integralité de la formule de base par les deux maximums.
    for (int teamIdx = 0; teamIdx < th.size(); teamIdx += 1){
            scores[teamIdx] =   19 - th[teamIdx].ranks[time] *maxPoint +
                                th[teamIdx].points[time] * maxRank;
            basicIndexes[teamIdx] = teamIdx;
            
    }
    //maintenant on trie les scores
    triFusion(scores,basicIndexes);

    //on sauvegarde aussi les changements de positions dans basicIndexes.

    //on utilise l'indice des indexes de départ
    for (int idx = 0; idx < basicIndexes.size(); idx += 1){
        th[basicIndexes[idx]].position[time] = idx;
    }
}

void fillPositions(vector<teamHistory> &th){
    int maxRank = 0;
    int maxPoint = 0;
    for (int teamIdx = 0; teamIdx < th.size(); teamIdx += 1)//on fixe les maximums.
    {
        if (maxRank < th[teamIdx].maxRank)
        {
            maxRank = th[teamIdx].maxRank;
        }
        if (maxPoint < th[teamIdx].maxPoint)
        {
            maxPoint = th[teamIdx].maxPoint;
        }
        th[teamIdx].position = vector<int>(th[teamIdx].ranks.size());
    }
    for (int time = 0; time < th[0].ranks.size(); time+= 1){
        sortAtTime(th,time,maxRank,maxPoint);
    }
}

/**
 * @param fileName : file toRead
 * 
 * @return: all team history.
 *          conataining all the skeleton needs to create itself.
 **/
vector<teamHistory> readData(string fileName)
{
    //38 + 3 match
    ifstream dataFile;
    dataFile.open(fileName);
    string extractedString;

    vector<teamHistory> toReturn(0);
    getline(dataFile, extractedString, ',');
    extractedString.erase(remove(extractedString.begin(), extractedString.end(), '\n'), extractedString.end());
    int rankOrPoint;

    do
    {
        teamHistory now;

        now.name = extractedString;
        now.maxPoint = 0;
        now.maxRank = 0;
        for (int i = 0; i < 41; i += 1)
        {
            getline(dataFile, extractedString, ',');
            extractedString.erase(remove(extractedString.begin(), extractedString.end(), ' '), extractedString.end());
            rankOrPoint = stoi(extractedString);
            now.ranks.push_back(rankOrPoint);
            if (rankOrPoint > now.maxRank)
            {
                now.maxRank = rankOrPoint;
            }
            getline(dataFile, extractedString, ',');
            extractedString.erase(remove(extractedString.begin(), extractedString.end(), ' '), extractedString.end());
            rankOrPoint = stoi(extractedString);
            if (rankOrPoint > now.maxPoint)
            {
                now.maxPoint = rankOrPoint;
            }
            now.points.push_back(rankOrPoint);
            getline(dataFile, extractedString, ',');
            getline(dataFile, extractedString, ',');
            getline(dataFile, extractedString, ',');
            getline(dataFile, extractedString, ',');
        }
        //getline(dataFile, extractedString, ',');
        toReturn.push_back(now);
        getline(dataFile, extractedString, ',');
        extractedString.erase(remove(extractedString.begin(), extractedString.end(), '\n'), extractedString.end());
    } while (extractedString.length() > 0);

    fillPositions(toReturn);

    return toReturn;
}

void swapInts(vector<int> &list, int idx1, int idx2){
    int temp = list[idx2];
    list[idx2] = list[idx1];
    list[idx1] = temp;
}