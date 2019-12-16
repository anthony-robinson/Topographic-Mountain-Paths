//
//  Topograph.cpp
//  Topograph
//
//  Created by Anthony Robinson on 3/16/19.
//  Copyright © 2019 Anthony Robinson. All rights reserved.
//

#include <stdio.h>
#include "Topograph.h"


///**************Part 1********************************
///Constructor.  Open an ArcGIS ASCII Grid file
///read the header to assign height and width
///resize v and read elevation data
Topograph::Topograph(string fileName)
{
    string ncols, nrows, xllcorner, yllcorner, cellsize;
    int cols, rows;
    ifstream is;
    is.open(fileName);
    is >> ncols;
    is >> cols;
    is >> nrows;
    is >> rows;
    is.ignore();
    getline(is, xllcorner);
    getline(is, yllcorner);
    getline(is, cellsize);
    height = rows;
    width = cols;
    v.resize(height,vector<int>(cols));
    for(int i = 0; i < rows; i++)
    {
        for(int j = 0; j < cols; j++)
        {
            is >> v[i][j];
        }
    }
    is.close();
    findMin();
    findMax();

}

void Topograph::findMin()
{
    int pmin = v[0][0]; //assume min is first element in 2D array
    for(int i = 0; i < v.size(); i++)
    {
        for(int j = 0; j < v[0].size(); j++)
        {
            //loop through vector to check for min elements
            if(v[i][j] < pmin) pmin = v[i][j];
        }
    }
    min = pmin;
    cout << min << endl;
}
void Topograph::findMax()
{
    int pmax = v[0][0];
    for(int i = 0; i < v.size(); i++)
    {
        for(int j = 0; j < v[0].size(); j++)
        {
            if(v[i][j] > pmax) pmax = v[i][j];
        }
    }
    max = pmax;
    cout << "max is: " << max << endl;
}

///scale n from [fromLow:fromHigh] to [toLow:toHigh]
int Topograph::mapRange(int n, int fromLow, int fromHigh, int toLow, int toHigh)
{
    int result;
    double intermed;
    intermed = (double((n - fromLow))/(fromHigh - fromLow));
    result = intermed * (toHigh - toLow);
    return result;
}

///find min and max elevation values
///call mapRange to convert each elevation to grayscale
///set each bmp(i,j) to its grayscale value
void Topograph::drawMap(Bmp& bmp)
{
    int p;

    for(int i = 0; i < v.size(); i++)
    {
        for(int j = 0; j < v[0].size(); j++)
        {
            p = mapRange(v[i][j],min,max,0,255);
            bmp(i,j).b = p;
            bmp(i,j).r = p;
            bmp(i,j).g = p;
        }
    }

    bmp.write("map.bmp");
}

///**************Part 2********************************
///Advance (i,j) along its greedy path
///Choose the right side adjacent index with the lowest elevation change
///For a tie, mid has highest priority, then down, then up
///i + 1 is down, i - 1 is up
///j + 1 is forward

///
void Topograph::moveForward(int& i, int& j)
{
    int current = v[i][j];
    int fwd = v[i][j + 1];
    int down = fwd;
    int up = fwd;
    int fwdD = fabs(current - fwd);
    int downD = fabs(current - down);
    int upD = fabs(current - up);

    if(i != 0)
    {
        up = v[i - 1][j + 1];
    }
    if(i != v.size() - 1)
    {
        down = v[i + 1][j + 1];
    }
    if(up > -1) upD = fabs(current - up);
    if(down > -1)downD = fabs(current - down);


    if(upD < fwdD && upD == downD)
    {
        if(i != 0 && j != v[0].size() - 1)
            i++;
        else if(i == v.size() - 1) {i--;}
    }

   if(upD < downD && upD < fwdD)
   {
       i--; // go up
   }
    if(downD < upD && downD < fwdD)
    {
        i++; //go down
    }
    if(fwdD < downD && fwdD < upD)
    {
        //go forward
    }
    if(upD == fwdD && downD > fwdD)
    {
        // go forward
    }


}



//Draw one greedy path
///call moveForward to advance (i,j)
///set each bmp(i,j) to color
///returns the total sum of the elevation change
int Topograph::drawGreedyPath(Bmp& bmp, int startingRow, rgb Color)
{
    int total = 0;
    int currentRow = startingRow;

    for(int j = 0; j < v[0].size() - 1; j++)
    {
        bmp(startingRow,j) = Color;
        moveForward(startingRow, j);
        if(startingRow == 0 || startingRow == v.size() - 1) {
                total += fabs(v[startingRow][j] + 1);
        }
        if(startingRow > currentRow) {total += fabs(v[currentRow][j] - v[currentRow + 1][j + 1]);}
        else if (startingRow < currentRow){total += fabs(v[currentRow][j] - v[currentRow - 1][j + 1]);}
        else {total += fabs(v[currentRow][j] - v[currentRow][j + 1]);}
        currentRow = startingRow;
    }
    return total;
}





///call drawGreedyPath for each startingRow, color red
///store the index of the path with lowest elevation change
///call drawGreedyPath for the lowest index, color blue
void Topograph::drawBestPath(Bmp& bmp)
{
    int lowest = drawGreedyPath(bmp,0,RED_PIXEL);
    int indexofLowest = 0;
    for(int i = 0; i < v.size(); i++)
    {
        drawGreedyPath(bmp, i, RED_PIXEL);
        if(drawGreedyPath(bmp,i,RED_PIXEL) < lowest)
        {
            lowest = drawGreedyPath(bmp,i,RED_PIXEL);
            indexofLowest = i;
        }
    }
    drawGreedyPath(bmp,indexofLowest,BLUE_PIXEL);
    cout << "Path of lowest elevation change: " << lowest << endl;
}




