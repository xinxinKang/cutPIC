#ifndef PIXCEL_H
#define PIXCEL_H//start of PIXCEL_H

void imgAndRGB(bmpStruct* bmpData);

void filter_nonMax(bmpStruct* bmpData);
void doubleThreshold(bmpStruct* bmpData);
void catchROI(bmpStruct* bmpData);

typedef struct _indexOfPixcel
{
    uint8 x;
    uint8 y;
}IndexOfPixcel;

#endif//end of PIXCEL_H