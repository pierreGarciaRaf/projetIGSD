#ifndef FREADER
#define FREADER
/**
 * Has all the information to make a curve skeleton.
 * 
 **/
struct teamHistory
{
    vector<float> ranks;
    vector<float> points;
    vector<int> position;
    float maxPoint;
    float maxRank;
    string name;
};

/**
 * Displays a team's(th) history.
 **/
void printTeamHistory(teamHistory th);

/**
 * fills a team using the path/fileName : fileName.
 **/
vector<teamHistory> readData(string fileName);
#endif FREADER