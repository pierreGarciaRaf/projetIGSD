#include <vector>
using namespace std;

#include "fusionVectorSort.hpp"

struct tabRange
{
    int min;
    int max;
};

int tRSize(tabRange tr)
{
    return tr.max - tr.min;
}

tabRange moitie1(tabRange tR)
{
    tR.max = tR.min + (tRSize(tR)) / 2;
    return tR;
}

tabRange moitie2(tabRange tR)
{
    tR.min = tR.min + (tRSize(tR)) / 2;
    return tR;
}

tabRange fusionne(tabRange tR1, tabRange tR2, vector<int> &values, vector<int> &basicIndexes)
{

    tabRange final = {tR1.min, tR2.max};
    int cursorTR1 = tR1.max;

    while (tRSize(tR2) > 0)
    {
        int temp = values[tR2.min];
        int tempIndex = basicIndexes[tR2.min];

        for (cursorTR1 = tR2.min; cursorTR1 > tR1.min; cursorTR1--)
        {
            values[cursorTR1] = values[cursorTR1 - 1];
            basicIndexes[cursorTR1] = basicIndexes[cursorTR1 - 1];

            if (values[cursorTR1]  < temp)
            {
                break;
            }
        }
        values[cursorTR1] = temp;
        basicIndexes[cursorTR1] = tempIndex;

        tR1.max++;
        tR2.min++;
    }
    return tR1;
}


tabRange triFusion(tabRange tR, vector<int> &values, vector<int> &basicIndexes)
{
    if (tRSize(tR) == 1)
    {
        return tR;
    }
    else
    {
        return fusionne(triFusion(moitie1(tR),values,basicIndexes), triFusion(moitie2(tR),values,basicIndexes),values,basicIndexes);
    }
}

void triFusion(vector<int> &values, vector<int> &basicIndexes)//fonction d'appel initiale.
{
    tabRange tR;
    tR.min = 0;
    tR.max = values.size();
    fusionne(triFusion(moitie1(tR),values,basicIndexes), triFusion(moitie2(tR),values,basicIndexes),values,basicIndexes);
}

