#include <stdio.h>
#include <stdlib.h>

//size_t fread ( void * ptr, size_t size, size_t count, FILE * stream );
//其中，ptr：指向保存结果的指针；size：每个数据类型的大小；count：数据的个数；stream：文件指针
//函数返回读取数据的个数。

//size_t fwrite ( const void * ptr, size_t size, size_t count, FILE * stream );
//其中，ptr：指向保存数据的指针；size：每个数据类型的大小；count：数据的个数；stream：文件指针
//函数返回写入数据的个数。

int main() {
    ////写
    //FILE *pFile;
    //int buffer[] = {1, 2, 3, 4};
    //if((pFile = fopen("myfile.txt", "wb")) == NULL) {
    //    printf("cant open the file");
    //    exit(0);
    //}
    ////可以写多个连续的数据(这里一次写4个)
    //fwrite(buffer , sizeof(int), 4, pFile);
    //fclose(pFile);


    //读
    FILE * fp;
    int buffer[4];
    if((fp=fopen("myfile.txt","rb"))==NULL) {
        printf("cant open the file");
        exit(0);
    }
    if(fread(buffer, sizeof(int), 4, fp) != 4) {  //可以一次读取
        printf("file read error\n");
        exit(0);
    }
    for(int i=0;i<4;i++) printf("%d\n",buffer[i]);
    return 0;
}
