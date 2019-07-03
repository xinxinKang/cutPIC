#include "header.h"

int main()
{
    bmpStruct* bmpData=readBMP("in.bmp");
    imgAndRGB(bmpData);
    saveBMP( bmpData,"gray.bmp",so_gray);
    saveBMP( bmpData,"Gfilter.bmp",so_Gfilter);
    saveBMP( bmpData,"edge.bmp",so_edge);
    saveBMP( bmpData,"after_thresholds.bmp",so_adEdge);
    deleteMem(bmpData);

    return 0;
}

