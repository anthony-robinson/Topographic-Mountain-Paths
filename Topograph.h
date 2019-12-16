#ifndef TOPOGRAPH_H_INCLUDED
#define TOPOGRAPH_H_INCLUDED

#include "Bmp.h"

const rgb RED_PIXEL = {255,0,0};
const rgb BLUE_PIXEL = {0,0,255};

class Topograph
{
public:
    ///**************Part 1********************************
    ///Constructor.  Open a ArcGIS ASCII Grid file
    ///read the header to assign height and width
    ///resize v and read elevation data
    Topograph(string fileName);

    int getHeight(){return height;}
    int getWidth(){return width;}

    ///find min and max elevation values
    ///call mapRange to convert each elevation to grayscale
    ///set each bmp(i,j) to its grayscale value
    void drawMap(Bmp& bmp);

    ///**************Part 2********************************
    ///Draw one greedy path
    ///call moveForward to advance (i,j)
    ///set each bmp(i,j) to color
    ///returns the total sum of the elevation change
    int drawGreedyPath(Bmp& bmp, int startingRow, rgb color);

    ///call drawGreedyPath for each startingRow, color red
    ///store the index of the path with lowest elevation change
    ///call drawGreedyPath for the lowest index, color blue
    void drawBestPath(Bmp& bmp);
    ///****************************************************
    
   
   
private:
    ///**************Part 1********************************
   
    ///scale n from [fromLow:fromHigh] to [toLow:toHigh]
    void findMin();
    void findMax();
    int mapRange(int n, int fromLow, int fromHigh, int toLow, int toHigh);
    

    vector<vector<int>> v;
    int height;
    int width;
    int min;
    int max;

    ///**************Part 2********************************
    
    ///Advance (i,j) along its greedy path
    ///Choose the right side adjacent index with the lowest elevation change
    ///For a tie, mid has highest priority, then down, then up
    ///i + 1 is down, i - 1 is up
    ///j + 1 is forward
    ///Be careful if i is on the upper or lower edge
    void moveForward(int& i, int& j);
};

#endif // TOPOGRAPH_H_INCLUDED
