#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <pigpio.h>
#include "74hc595_functions.h"

#define BOARD_ROW 8

void dot(int row, int col)
{
    uint8_t row8, col8;
    uint16_t tmp;
    row8 = ~(1 << (8-row));
    col8 = 1 << (8-col);
    tmp = (row8<<8) | col8;
    set16(tmp);
}

int cnt;
void callback(void)
{
	printf("500 milliseconds have elapsed\n");
	cnt++;	
}
 
int main(){
    FILE *fp;
   
    uint8_t mat[BOARD_ROW][1000];
    uint8_t *newMat[BOARD_ROW];
    int total_col;
    int eof; 
    int ret;

    fp = fopen("data", "r");
    if(fp == NULL){
        printf("file not open!!!\n");
        return 1;
    }
    
    for(int i=0; i<BOARD_ROW; i++){
          total_col=0;
          while((eof = fgetc(fp))!='\n'){
             if(eof == EOF) break;
             
             if(total_col%2){
                 total_col++;
             }
             else if(eof == 'o'){
                 mat[i][total_col/2] = 1;
                 total_col++;  
             } else {
                 mat[i][total_col/2] = 0;
                 total_col++;
             }
         }
         newMat[i] = (uint8_t*)malloc((total_col/2)*sizeof(uint8_t));        
        
         for(int k=0; k<(total_col/2); k++)
             newMat[i][k] = mat[i][k];
    }

    total_col /= 2;
    printf("len: %d\n", total_col);

    for(int i=0; i<BOARD_ROW; i++){
        for(int j=0; j<total_col; j++){
            if(newMat[i][j] == 1)
                printf("%c",'o');
            else
                printf("%c",' ');
        }
        printf("\n");
    }
    
    ret = init();
    if(ret == 0)
        return 0;

    gpioSetTimerFunc(0, 500, callback);

    while(1)
        for(int i = 0 ; i < 8 ; i++)
            for(int j = 0 ; j < 8 ; j++)
                if(newMat[i][(j+cnt)%total_col] == 1)
                    dot(i+1,j+1);
    
    release();
    fclose(fp);
    return 0;
}

