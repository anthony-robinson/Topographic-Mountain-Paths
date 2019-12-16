//
//  main.cpp
//  Topograph
//
//  Created by Anthony Robinson on 3/16/19.
//  Copyright © 2019 Anthony Robinson. All rights reserved.
//

#include "Topograph.h"

int main() {
    
    Topograph t = Topograph("map_input.txt");
    Bmp b(t.getHeight(),t.getWidth());
    t.drawMap(b);
    t.drawGreedyPath(b, 0, RED_PIXEL);
    t.drawBestPath(b);
    b.write("new_map.bmp");
    return 0;
}
