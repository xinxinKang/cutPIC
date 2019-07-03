#ifndef IMG_H
#define IMG_H//start of IMG_H

#define uint8 unsigned char
#define uint16 unsigned short
#define int16 short
#define uint32 unsigned int
#define int32 int

#pragma pack(2)
/* 文件头：文件类型 */
typedef struct tagBITMAPFILEHEADER
{
uint16 bfType;        //2Bytes，必须为"BM"，即0x424D 才是Windows位图文件
uint32 bfSize;        //4Bytes，整个BMP文件的大小
uint16 bfReserved1;   //2Bytes，保留，为0
uint16 bfReserved2;   //2Bytes，保留，为0
uint32 bfOffBits;     //4Bytes，文件起始位置到图像像素数据的字节偏移量
} BITMAPFILEHEADER;
/* 信息头：尺寸信息 */
typedef struct _tagBMP_INFOHEADER
{
uint32   biSize;            //4Bytes，本结构所占用字节数
int32    biWidth;           //4Bytes，图像宽度（以像素为单位）
int32    biHeight;          //4Bytes，图像高度，+：图像存储顺序为Bottom2Top，-：Top2Bottom
uint16   biPlanes;          //2Bytes，图像数据平面，BMP存储RGB数据，因此总为1
uint16   biBitCount;        //2Bytes，每个像素的位数：1-双色；4-16色；8-256色；24-真彩色
uint32   biCompression;     //4Bytes，0：不压缩，1：RLE8压缩类型，2：RLE4压缩类型
uint32   biSizeImage;       //4Bytes，4字节对齐的图像数据大小
int32    biXPelsPerMeter;   //4Bytes，用象素/米表示的水平分辨率
int32    biYPelsPerMeter;   //4Bytes，用象素/米表示的垂直分辨率
uint32   biClrUsed;         //4Bytes，位图实际使用的颜色表中的颜色数
uint32   biClrImportant;    //4Bytes，重要的调色板索引数，0：所有的调色板索引都重要
}BMP_INFOHEADER;

/**
 * 颜色表
 * biBitCount=1、4、8时，分别有2，16，256个表项
 * biBitCount=24时，没有颜色表项
**/
typedef struct _tagRGBQUAD
{
    uint8 rgbBlue;          //蓝色亮度（0～255）
    uint8 rgbGreen;
    uint8 rgbRed;
    uint8 rgbReserved;      //保留，必须为0
}RGBQUAD;
#pragma pack()
typedef struct BMPStruct{
    int height;
    int width;
    BITMAPFILEHEADER* bmpHeader;
    BMP_INFOHEADER* bmpInfo;
    unsigned char* ptrData;
    unsigned char* grayPtrData;
    unsigned char* GPtrData;
    unsigned char* edgePtrData;
    unsigned char* adEdgePtrData; //after double-threshold check
}bmpStruct;
bmpStruct* readBMP(char* file);

typedef enum
{
    so_gray     =0,
    so_Gfilter  =1,
    so_edge     =2,
    so_adEdge

}saveBMP_option;

void saveBMP(bmpStruct* bmpData,char* file,saveBMP_option option);
void deleteMem(bmpStruct* bmpData);
#endif//end of IMG_H
