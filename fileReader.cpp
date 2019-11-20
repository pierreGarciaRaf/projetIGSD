#include <fileReader.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
using namespace std;

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
    return toReturn;
}
