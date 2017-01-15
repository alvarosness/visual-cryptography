#ifndef STEGANO_H
#define STEGANO_H

#include "imgproc.h"
#include "consts.h"

int watermark(char* imageFileName);
int newWatermark(char* imageFileName, char* word, int key);
int newNewWatermark(char* imageFileName, char* word, int key);
int extractWatermark(char* watermarkedFileName, int key);
int getAverageAreaValue(char** matrix, int x, int y, int limit);
float errorPercentage(int theoreticalValue, int measuredValue);
int numberFileCreator();

#endif
