///Copyright Ryan Hermle.  Feb. 20, 2018.  All rights reserved.

#include "Bmp.h"


Bmp::Bmp(int _height, int _width):height(_height), width(_width)
{
    pixels.resize(height);
    for(int i = 0; i < height; i++)
    {
        pixels[i].resize(width);
    }
}

void Bmp::readHeader(istream& is)
{
    //cout << "Reading header." << endl;
    is.read(as_bytes(header.headerType), sizeof(short));
    is.read(as_bytes(header.fileSize), sizeof(int));
    is.read(as_bytes(header.reserved1), sizeof(short));
    is.read(as_bytes(header.reserved2), sizeof(short));
    is.read(as_bytes(header.offset), sizeof(int));

    is.read(as_bytes(header.headerSize), sizeof(int));
    is.read(as_bytes(header.width), sizeof(int));
    is.read(as_bytes(header.height), sizeof(int));
    is.read(as_bytes(header.colorPlanes), sizeof(short));
    is.read(as_bytes(header.bitsPerPixel), sizeof(short));
    is.read(as_bytes(header.compressionMethod), sizeof(int));
    is.read(as_bytes(header.imageSize), sizeof(int));
    is.read(as_bytes(header.horizRes), sizeof(int));
    is.read(as_bytes(header.vertRes), sizeof(int));
    is.read(as_bytes(header.numColors), sizeof(int));
    is.read(as_bytes(header.numImportantColors), sizeof(int));
    cout << "Finished reading header." << endl;
    //printHeader();

    ///come to my office hour if you want to talk about the next line (difficult)
    rowPadding = ((header.width * 3 + 3) & (~3)) - (header.width * 3);

    if(header.bitsPerPixel != 24) error("Bits per pixel must be 24");
}

void Bmp::writeHeader(ostream& os)
{
    if(header.headerType == 0)  ///header uninitialized
    {
        error("Attempted to write an uninitialized header.");
    }

    //cout << "Writing header." << endl;

    os.write(as_bytes(header.headerType), sizeof(short));
    os.write(as_bytes(header.fileSize), sizeof(int));
    os.write(as_bytes(header.reserved1), sizeof(short));
    os.write(as_bytes(header.reserved2), sizeof(short));
    os.write(as_bytes(header.offset), sizeof(int));

    os.write(as_bytes(header.headerSize), sizeof(int));
    os.write(as_bytes(header.width), sizeof(int));
    os.write(as_bytes(header.height), sizeof(int));
    os.write(as_bytes(header.colorPlanes), sizeof(short));
    os.write(as_bytes(header.bitsPerPixel), sizeof(short));
    os.write(as_bytes(header.compressionMethod), sizeof(int));
    os.write(as_bytes(header.imageSize), sizeof(int));
    os.write(as_bytes(header.horizRes), sizeof(int));
    os.write(as_bytes(header.vertRes), sizeof(int));
    os.write(as_bytes(header.numColors), sizeof(int));
    os.write(as_bytes(header.numImportantColors), sizeof(int));

    //cout << "Finished writing header." << endl;
    //printHeader();

    ///if offset is larger than expected, fill the space with 0's (just in case of some weird header format)
    for(int i = 54; i < header.offset; i++)
    {
        os.put(0);
    }

}

void Bmp::readPixels(istream& is)
{
    if(header.headerType == 0)  ///header uninitialized
    {
        error("Attempted to read pixels without a header loaded.");
    }

    ///clear any existing pixel data
    pixels.clear();
    ///update member variables and reallocate pixel vector
    height = header.height;
    width = header.width;
    pixels.resize(height);
    for(int i = 0; i < height; i++)
    {
        pixels[i].resize(width);
    }

    //cout << "Reading pixel data." << endl;
    is.seekg(header.offset);  ///seek to beginning of pixel data

    unsigned char r,g,b;

    for (int i = 0; i < height; i++)
    {
        int j;
        for (j = 0; j < width; j++)
        {
            b = (unsigned char)is.get();
            g = (unsigned char)is.get();
            r = (unsigned char)is.get();
            pixels[i][j] = rgb{r,g,b};
        }

        for(int k = 0; k < rowPadding; k++) is.get();
    }


    ///invert the rows because row 0 is actually the bottom of the image
    vector<rgb> temp;  ///temp row for swapping
    for (int i = 0; i < height / 2; i++)
    {
        temp = pixels[i];
        pixels[i] = pixels[pixels.size() - 1 - i];
        pixels[pixels.size() - 1 - i] = temp;
    }

    //cout << "Finished reading pixel data" << endl;
}

void Bmp::writePixels(ostream& os)
{
    if(header.headerType == 0) error("Attempted to write pixels without a header loaded.");
    else if(pixels.size() == 0) error("Attempted to write empty pixel array");
    else if(pixels[0].size() == 0) error("Attempted to write empty pixel array");
    else if(pixels.size() != (size_t)header.height || pixels[0].size() != (size_t)header.width)
        error("Header and pixel vector dimensions do not match.");

    //cout << "Writing pixel data." << endl;

    os.seekp(header.offset);  ///seek to beginning of pixel data

    for (int i = 0; i < height; i++)
    {
        int j;
        for (j = 0; j < width; j++)
        {
            ///invert the rows because row 0 is actually the bottom of the image
            os.put((char)pixels[pixels.size() - 1 - i][j].b);
            os.put((char)pixels[pixels.size() - 1 - i][j].g);
            os.put((char)pixels[pixels.size() - 1 - i][j].r);
        }

        for(int k = 0; k < rowPadding; k++) os.put(0);
    }

    //cout << "Finished writing pixel data." << endl;
}

void Bmp::createHeader()
{
    if(pixels.size() == 0)
    {
        error("Cannot create a header with an empty pixel array.");
    }

    header.headerType = 19778;
    header.height = pixels.size();
    header.width = pixels[0].size();

    ///come to my office hour if you want to talk about the next line
    rowPadding = ((header.width * 3 + 3) & (~3)) - (header.width * 3);

    header.imageSize = (header.width * 3 + rowPadding) * header.height;

    header.fileSize = header.imageSize + 54;
    header.reserved1 = 0;
    header.reserved2 = 0;
    header.offset = 54;
    header.headerSize = 40;
    header.colorPlanes = 1;
    header.bitsPerPixel = 24;
    header.compressionMethod = 0;
    header.horizRes = 4724;
    header.vertRes =4724;
    header.numColors = 0;
    header.numImportantColors = 0;
}

void Bmp::write(string fileName)
{
    //cout << "Generating header." << endl;
    createHeader();
    cout << "Writing to " << fileName << endl;
    ofstream os(fileName, ios::binary);
    writeHeader(os);
    writePixels(os);
    cout << "Write complete." << endl;
}
void Bmp::read(string fileName)
{
    ifstream is(fileName, ios::binary);
    if(!is) error("Failed to open " + fileName);
    cout << "Reading from " << fileName << endl;
    readHeader(is);
    readPixels(is);
    cout << "Read complete." << endl;
}

void Bmp::printHeader()
{
    ///cout << hex << showbase;
    cout << "Contents of header:" << endl;
    cout << "headerType: " << header.headerType << endl;
    cout << "fileSize: " << header.fileSize << endl;
    cout << "reserved1: " << header.reserved1 << endl;
    cout << "reserved2: " << header.reserved2 << endl;
    cout << "offset: " << header.offset << endl;

    cout << "headerSize: " <<header.headerSize << endl;
    cout << "width: " << header.width << endl;
    cout << "height: " <<header.height << endl;
    cout << "colorPlanes: " << header.colorPlanes << endl;
    cout << "bitsPerPixel: " << header.bitsPerPixel << endl;
    cout << "compressionMethod: " << header.compressionMethod << endl;
    cout << "imageSize: " <<header.imageSize << endl;
    cout << "horizRes: " << header.horizRes << endl;
    cout << "vertRes: " << header.vertRes << endl;
    cout << "numColors: " << header.numColors << endl;
    cout << "numImportantColors: " << header.numImportantColors << endl;
}
