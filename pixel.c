#include "header.h"
#include "pixel.h"
/*
* 将位图分散为RGB三通道，进行处理
*/
//高斯滤波矩阵
unsigned char GMatrix[25]={2,4,5,4,2,
                             4,9,12,9,4,
                             5,12,15,12,5,
                             4,9,12,9,4,
                             2,4,5,4,2};

void imgAndRGB(bmpStruct* bmpData){
    //重要参数
    unsigned char* ptrData=bmpData->ptrData;
    int height=bmpData->height;
    int width=bmpData->width;
    /* *
     * 用malloc生成二位空间并不是连续的，先生成行指针
     * 然后行指针再去malloc
     * */
    unsigned char** BMatrix =(unsigned char**)malloc(height*sizeof(char*));
    unsigned char** GMatrix =(unsigned char**)malloc(height*sizeof(char*));
    unsigned char** RMatrix =(unsigned char**)malloc(height*sizeof(char*));

    int i,j;
    for(i=0;i<height;i++){
        BMatrix[i]=(unsigned char*)malloc(width);
        RMatrix[i]=(unsigned char*)malloc(width);
        GMatrix[i]=(unsigned char*)malloc(width);
    }
    for(i=0;i<height;i++){
        for(j=0;j<width;j++){
            BMatrix[i][j]=ptrData[3*j+0+i*width*3];
            RMatrix[i][j]=ptrData[3*j+1+i*width*3];
            GMatrix[i][j]=ptrData[3*j+2+i*width*3];
        }
    }
    int markOfSize=0;

//    memset(ptrData,0,height*width*3);
//

 //   memset(newPtrData,0,height*width);

    /* 灰度转换 */
    if(!markOfSize){
        //通道返回
        for(i=0;i<height;i++){
            for(j=0;j<width;j++){
                int temp=0.299 * BMatrix[i][j] + 0.587*GMatrix[i][j] + 0.114 *  RMatrix[i][j];
                if(temp<0) temp=0;
                else if(temp>255) temp=255;
                bmpData->grayPtrData[i*width+j]=(uint8)temp;
            }
        }
    }
    /* 图像平滑,高斯 */
    bmpData->GPtrData=(uint8*)malloc(bmpData->height*bmpData->width);
    unsigned char* grayPtrData= bmpData->grayPtrData;
    for(i=0;i<height;++i){
        if(i<2||i>=height-2)
            bmpData->GPtrData[i*width+j]=grayPtrData[(i)*width+j];
        else{
            for (j = 0; j < width; ++j) {
                if(j<2||j>=width-2)
                    bmpData->GPtrData[i*width+j]=grayPtrData[(i)*width+j];
                else{
                    int16 temp
                            =(2*grayPtrData[(i-2)*width+j-2]+grayPtrData[(i-2)*width+j-1]*4+grayPtrData[(i-2)*width+j]*5 +grayPtrData[(i-2)*width+j+1]*4 +grayPtrData[(i-2)*width+j+2]*2
                              +4*grayPtrData[(i-1)*width+j-2]+grayPtrData[(i-1)*width+j-1]*9+grayPtrData[(i-1)*width+j]*12+grayPtrData[(i-1)*width+j+1]*9+grayPtrData[(i-1)*width+j+2]*4
                              +5*grayPtrData[(i)*width+j-2]  +grayPtrData[(i)*width+j-1]*12 +grayPtrData[(i)*width+j]*15  +grayPtrData[(i)*width+j+1]*12 +grayPtrData[(i)*width+j+2]*5
                              +4*grayPtrData[(i+1)*width+j-2]+grayPtrData[(i+1)*width+j-1]*9+grayPtrData[(i+1)*width+j]*12+grayPtrData[(i+1)*width+j+1]*9+grayPtrData[(i+1)*width+j+2]*4
                              +2*grayPtrData[(i+2)*width+j-2]+grayPtrData[(i+2)*width+j-1]*4+grayPtrData[(i+2)*width+j]*5 +grayPtrData[(i+2)*width+j+1]*4+grayPtrData[(i+2)*width+j+2]*2)/159.0+0.5;
                    if(temp<0) temp=0;
                    else if(temp>255) temp=255;
                    bmpData->GPtrData[i*width+j]=(uint8)temp;
                }
            }
        }
    }

    /* canny */
    //非极大值抑制
    filter_nonMax(bmpData);
    //double-threshold check
    doubleThreshold(bmpData);
    /* end of canny */

    /* Region of Interest ROI */

    /* end of ROI */
    catchROI(bmpData);
    //清空内存                                                                       
        for(i=0;i<height;i++){
            free(GMatrix[i]);
            free(RMatrix[i]);
            free(BMatrix[i]);
        }
        free(GMatrix);
        free(RMatrix);
        free(BMatrix);

}//end of imgAndRGB
/*
 * 非最大值滤波，尽可能使得边缘只有一个像素
 * x方向为高度方向,up
 * y方向为宽度方向,right
 */
double Theta[HIMG][WIMG];
int M[600][800];
void filter_nonMax(bmpStruct* bmpData) {
    //float ()angle=

    const int height = 600;
    const int width = 800;
    int i, j;
    int g1 = 0, g2 = 0, g3 = 0, g4 = 0;                            //用于进行插值，得到亚像素点坐标值
    double dTmp1 = 0.0, dTmp2 = 0.0;                           //保存两个亚像素点插值得到的灰度数据
    double dWeight = 0.0;                                    //插值的权重
    double Gx[HIMG][WIMG];
    double Gy[HIMG][WIMG];
    for (i = 1; i < height; ++i) {
        for (j = 1; j < width; ++j) {
            //x导数
            Gx[i][j] = -bmpData->GPtrData[(i - 1) * width + j-1] + bmpData->GPtrData[(i-1) * width + j+1]
                    -2*bmpData->GPtrData[(i ) * width + j-1] + 2*bmpData->GPtrData[i * width + j+1]
                    -bmpData->GPtrData[(i + 1) * width + j-1] + bmpData->GPtrData[(i+1) * width + j+1];
            //y导数
            Gy[i][j] = bmpData->GPtrData[(i-1) * width + j - 1] + 2*bmpData->GPtrData[(i-1) * width + j]+bmpData->GPtrData[(i-1) * width + j + 1]
                    -bmpData->GPtrData[(i+1) * width + j - 1] - 2*bmpData->GPtrData[(i+1) * width + j]-bmpData->GPtrData[(i+1) * width + j + 1];

            M[i][j]=(uint8) sqrtf(Gx[i][j] * Gx[i][j] + Gy[i][j] * Gy[i][j]);  //梯度膜长

            if(Gx[i][j]==0) Gx[i][j]=0.00001;//防止Gx可能为0
            Theta[i][j]= atan2(Gy[i][j],Gx[i][j])*180/3.1415; //梯度方向，与X轴夹角
        }
    }
     do{
//         break;
         for (i = 1; i < 600 - 1; ++i) {
             for ( j = 1; j < 800 - 1; ++j) {
                if((Theta[i ][j] >= -22.5 & Theta[i ][j] < 22.5) ||
                    (Theta[i ][j] >= 157.5 || Theta[i ][j] < -157.5)){
                    dTmp1=M[i][j+1];
                    dTmp2=M[i][j-1];
                }
                else if((Theta[i ][j] >= 22.5 & Theta[i ][j] < 67.5) ||
                        (Theta[i ][j] >= -157.5 || Theta[i ][j] < -112.5)){
                    dTmp1=M[i+1][j+1];
                    dTmp2=M[i-1][j-1];
                }
                else if((Theta[i ][j] >= 67.5 & Theta[i ][j] < 112.5) ||
                        (Theta[i ][j] >= -112.5 || Theta[i ][j] < -67.5)){
                    dTmp1=M[i+1][j];
                    dTmp2=M[i-1][j];
                }
                else if((Theta[i ][j] >= 112.5 & Theta[i ][j] < 157.5) ||
                        (Theta[i ][j] >= -67.5 || Theta[i ][j] < -22.5)){
                    dTmp1=M[i+1][j-1];
                    dTmp2=M[i-1][j+1];
                }
                else{
                    printf("wrong angle : %f\t",Theta[i ][j]);
                }
                 if ((M[i][j] >= dTmp1) && (M[i][j] >= dTmp2)) {
                     bmpData->edgePtrData[i * width + j] = M[i][j];

                 } else
                     bmpData->edgePtrData[i * width + j] = 0;
              //   bmpData->edgePtrData[i * width + j]=M[i][j];
             }
         }
     }while(0);
}
/* *
 * double threshold check
 * */
#define Th 150
#define Tl 100
void doubleThreshold(bmpStruct* bmpData){
    int i,j;
    double sumPixel=0.0;
    double avrPixel;
    int height=bmpData->height;
    int width=bmpData->width;
    //the average of the pixels
    for(i=0;i<height;++i){
        for (j = 0; j < width; ++j) {
            sumPixel+=bmpData->edgePtrData[i * width + j];
        }
    }
    avrPixel=sumPixel/height/width;

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            if(bmpData->edgePtrData[i * width + j]>Th)
                bmpData->adEdgePtrData[i*width+j]=200;
            else if(bmpData->edgePtrData[i * width + j]<Tl)
                bmpData->adEdgePtrData[i*width+j]=0;
            else{
                // if connected
                if(bmpData->adEdgePtrData[(i+1)*width+(j-1)]>Th || bmpData->adEdgePtrData[(i+1)*width+(j)]>Th || bmpData->adEdgePtrData[(i+1)*width+(j+1)]>Th
                || bmpData->adEdgePtrData[(i)*width+(j-1)]>Th || bmpData->adEdgePtrData[(i)*width+(j+1)]>Th
                || bmpData->adEdgePtrData[(i-1)*width+(j-1)]>Th || bmpData->adEdgePtrData[(i-1)*width+(j)]>Th || bmpData->adEdgePtrData[(i-1)*width+(j+1)]>Th)
                    bmpData->adEdgePtrData[i*width+j]=200;
                else bmpData->adEdgePtrData[i*width+j]=0;
             }
        }
    }
}
//detect the middle line j=width/2
IndexOfPixcel indexOfPixcel[600]
void catchROI(bmpData){
    int i,j;
    int height=bmpData->height;
    int width=bmpData->width;
    int halfWidth=width>>1;
    for(i=0;i<height;++i){
        for (j = halfWidth; j >=0; --j) {

        }
        for(j=halfWidth;j<width;++j){

        }
    }
}