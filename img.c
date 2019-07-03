#include "header.h"
#include "img.h"

bmpStruct* readBMP(char* file){
    //位图尺寸，数据流,保存的文件头
    bmpStruct* bmpData=(bmpStruct*)malloc(sizeof(bmpStruct));
    
    FILE *fp=fopen(file,"rb");
    if(!fp) {
        printf("wrong file\n");
        exit(-1);
    }
    //为文件头分配内存空间
    bmpData->bmpHeader=(BITMAPFILEHEADER*)malloc(sizeof(BITMAPFILEHEADER));
    bmpData->bmpInfo=(BMP_INFOHEADER*)malloc(sizeof(BMP_INFOHEADER));
    //读取文件头、信息头
    fread(bmpData->bmpHeader,sizeof(BITMAPFILEHEADER),1,fp);
    fread(bmpData->bmpInfo,sizeof(BMP_INFOHEADER),1,fp);

    bmpData->height = bmpData->bmpInfo->biHeight;
    bmpData->width = bmpData->bmpInfo->biWidth;
    //存放位图数据流
    int capacity=bmpData->height*bmpData->width*3;//数据容量
    //为数据流分配内存空间
    bmpData->ptrData=(unsigned char*)malloc(sizeof(char)*capacity);
    bmpData->grayPtrData=(uint8*)malloc(bmpData->height*bmpData->width);
    bmpData->edgePtrData=(uint8*)malloc(bmpData->height*bmpData->width);
    bmpData->adEdgePtrData=(uint8*)malloc(bmpData->height*bmpData->width);
    int bitCount = bmpData->bmpInfo->biBitCount;
    if(bitCount!=24){
        printf("the bit count of the bmp is bitCount=%d\n",bitCount);
        printf("please input a true color bmp");
        exit(1);
    }

    //设置文件指针到数据流
    //fseek(fp,bmpData->bmpHeader->bfOffBits,SEEK_SET);
    fread(bmpData->ptrData,sizeof(char)*capacity,1,fp);
    fclose(fp);

//    //测试：输出图片尺寸
//    printf("capacity=%d\n",capacity);
//    printf("biSizeImage=%d\n",bmpData->bmpInfo->biSizeImage);
//    printf("biSize=%d\n",bmpData->bmpInfo->biSize);
//    printf("bfOffBits=%d\n",bmpData->bmpHeader->bfOffBits);
//    printf("bfSize=%d\n",bmpData->bmpHeader->bfSize);
   printf("%d\t%d\n", bmpData->bmpInfo->biHeight,bmpData->bmpInfo->biWidth);

    return bmpData;
}//end of readBMP
/*
 * 保存图像信息
 * 这里将处理过后的图像数据和
*/
void saveBMP(bmpStruct* bmpData,char* file,saveBMP_option option){

    uint8 glayFlag=1;
    FILE *fp=fopen(file,"wb");
    uint16 i;
    RGBQUAD rgbquad[256];

    //初始化灰度级，图像数据区
    if(glayFlag==1){
        for(i=0;i<256;i++){
            rgbquad[i].rgbBlue=(uint8)i;
            rgbquad[i].rgbGreen=(uint8)i;
            rgbquad[i].rgbRed=(uint8)i;
            rgbquad[i].rgbReserved=0;
        }
    }
    //写文件头:8bit文件头
    bmpData->bmpInfo->biBitCount=8;
    bmpData->bmpInfo->biSizeImage=bmpData->height*bmpData->width;
    bmpData->bmpHeader->bfOffBits=sizeof(BITMAPFILEHEADER)+sizeof(BMP_INFOHEADER)+sizeof(RGBQUAD)*256;
    bmpData->bmpHeader->bfSize= bmpData->bmpHeader->bfOffBits+bmpData->bmpInfo->biSizeImage;

    fwrite(bmpData->bmpHeader,sizeof(BITMAPFILEHEADER),1,fp);
    fwrite(bmpData->bmpInfo,sizeof(BMP_INFOHEADER),1,fp);
    fwrite(rgbquad,sizeof(RGBQUAD),256,fp);
    //写图像数据
    // fseek(fp,bmpData->bmpHeader->bfOffBits,SEEK_SET);

    if(option==so_gray)
        fwrite(bmpData->grayPtrData,bmpData->bmpInfo->biSizeImage,1,fp);
    else if(option==so_Gfilter)
        fwrite(bmpData->GPtrData,bmpData->bmpInfo->biSizeImage,1,fp);
    else if(option==so_edge)
        fwrite(bmpData->edgePtrData,bmpData->bmpInfo->biSizeImage,1,fp);
    else if(option==so_adEdge)
        fwrite(bmpData->adEdgePtrData,bmpData->bmpInfo->biSizeImage,1,fp);



}//end of saveBMP
void deleteMem(bmpStruct* bmpData){
    //释放动态内存
    free(bmpData->bmpHeader);
    free(bmpData->bmpInfo);
    free(bmpData->ptrData);
    free(bmpData->grayPtrData);
    free(bmpData->GPtrData);
    free(bmpData->adEdgePtrData);
}