#include "stegano.h"

//watermark generator
int watermark(char* imageFileName)
{
	//header information retainers
	FILE* imageFile;
	char type[3];
	int width;
	int height;
	int grayscale;
	//future arrays that will contain the payload
	int **image;
	FILE* outputFile;
//	char fileName[50] = "watermarked";	

	//opening the output file
	imageFile = fopen(imageFileName, "rb");
	outputFile = fopen("watermarked/watermarked.pgm", "wb");

	if (outputFile == NULL || imageFile == NULL)
		return FAIL;

	//reading the header
	headerReader(imageFile, type, &height, &width, &grayscale);

	//allocating the memory for the payload matrix
	image = (int**)malloc(height*sizeof(int*));
	for (int i = 0; i < height; i++)
	{
		*(image + i) = (int*)malloc(width*sizeof(int));
	}

	//reading the image
	for (int i = 0;i < height;i++)
		for (int j = 0;j < width;j++)
			*(*(image + i) + j) = fgetc(imageFile);

	//use the number generator to select a particular pixel in the array 
	//subtract two to flip the 2nd least significant bit (assuming the rules of 2s complement are still valid)
	for (int n = 0, i = 1, j = 3; n < 128;n++)
	{
		i = numberGenerator(i, width);
		j = numberGenerator(j, height);

		image[i][j] ^= 2;
	}

	//write the new watermarked file
	fprintf(outputFile, "%s%c%d%c%d%c%d%c", type, 10, width, 32, height, 10, grayscale, 10);

	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
			fputc(image[i][j], outputFile);

	fclose(imageFile);
	fclose(outputFile);
	return SUCCESSFUL;
}

int newWatermark(char* imageFileName, char* word, int key)
{
	//auxiliary variables
	FILE* imageFile;
	FILE* outputFile;
	unsigned char mask = 128;
	unsigned char bit;
	unsigned char** image;
	int row;
	int col;

	//header info
	char type[3];
	int height;
	int width;
	int grayscale;

	//header reading and file opening	
	imageFile = fopen(imageFileName, "rb");
	outputFile = fopen("watermarked/watermarked.pgm","wb");

	if (outputFile == NULL || imageFile == NULL)
		return FAIL;
	
	headerReader(imageFile, type, &height, &width, &grayscale);

	//allocating the memory for the payload matrix
	image = (unsigned char**)malloc(height * sizeof(unsigned char*));
	for (int i = 0; i < height; i++)
	{
		*(image + i) = (unsigned char*)malloc(width * sizeof(unsigned char));
	}

	//reading the image
	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
			*(*(image + i) + j) = fgetc(imageFile);
	
	//initial value
	row = numberGenerator(key, width);
	col = numberGenerator(key, height);

	//watermarking process
	for (int i = 0; i < strlen(word); i++)
	{
		mask = 128;
		for (int n = 0; n < 8*sizeof(char); n++)//multiply by eight to get the size in bits rather than bytes
		{
			bit = word[i] & mask;
			
			if (bit == 0)
			{
				image[row][col] = image[row][col] & ~mask;
			}
			else if (bit == mask)
			{
				image[row][col] = image[row][col] | mask;
			}

			mask = mask >> 1;
			row = numberGenerator(row, width);
			col = numberGenerator(col, height);
		}
	}


	//write the new watermarked file
	fprintf(outputFile, "%s%c%d%c%d%c%d%c", type, 10, width, 32, height, 10, grayscale, 10);

	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
			fputc(image[i][j], outputFile);

	fclose(imageFile);
	fclose(outputFile);

	return SUCCESSFUL;
}

//potential idea... The key could be the size of the text to be watermarked
//if the text is very long could it create an overflow error? Most likely it will.
//
//original idea of using the very first pixel (i.e. image[0][0]) did not work well
//because that pixel value was altered after the watermarking
//I could have the program check to see if that value is to be altered and then not alter it.
//
//potentially have a token at the end of the string to indicate the end making the watermark reading process 
//simpler
int extractWatermark(char* watermarkedFileName, int key)
{
	//header info
	char type[3];
	int width;
	int height;
	int grayscale;
	
	//auxiliary variables
	FILE* watermarkedFile;
	FILE* watermark;
	unsigned char mask = 128;
	unsigned char letter;
	unsigned char **image;
	int row;
	int col;
	int aux;

	//opening watermark file to write
	watermarkedFile = fopen(watermarkedFileName, "rb");
	watermark = fopen("watermarked/watermark.txt", "w");
	if (watermark == NULL || watermarkedFileName == NULL)
		return FAIL;

	headerReader(watermarkedFile, type, &height, &width, &grayscale);

	//initialazing the image array
	image = (unsigned char**)malloc(height*sizeof(unsigned char*));
	for (int i = 0; i < height; i++)
		*(image + i) = (unsigned char*)malloc(width * sizeof(unsigned char));

	//reading all the pixels from the image
	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
			image[i][j] = fgetc(watermarkedFile);

	//reading the watermark from the image
	row = numberGenerator(key, width);
	col = numberGenerator(key, height);

	for (int m = 0; m < key; m++)
	{
		mask = 128;
		letter = 0;
		for (int n = 0; n < 8*sizeof(char); )
		{
			//modifications
			aux = getAverageAreaValue(image, row, col, width);
			if(errorPercentage(image[row][col],aux) >= 45 || errorPercentage(image[row][col],aux) == 0){
				row = numberGenerator(row, width);
				col = numberGenerator(col, height);
				putchar('1');
			}else{
			
				letter |= image[row][col] & mask;

				row = numberGenerator(row, width);
				col = numberGenerator(col, height);
				mask = mask >> 1;
				putchar('0');
				n++;
			}//end of modifications
		}
		putchar('\n');
		fputc(letter, watermark);
	}

	fclose(watermarkedFile);
	fclose(watermark);
	
	return SUCCESSFUL;
}

int newNewWatermark(char* imageFileName, char* word, int key)
{
	//auxiliary variables
	FILE* imageFile;
	FILE* outputFile;
	unsigned char mask = 128;
	unsigned char bit;
	unsigned char** image;
	int row;
	int col;
	int aux;

	//header info
	char type[3];
	int height;
	int width;
	int grayscale;

	//header reading and file opening	
	imageFile = fopen(imageFileName, "rb");
	outputFile = fopen("watermarked/watermarked.pgm","wb");

	if (outputFile == NULL || imageFile == NULL)
		return FAIL;
	
	headerReader(imageFile, type, &height, &width, &grayscale);

	//allocating the memory for the payload matrix
	image = (unsigned char**)malloc(height * sizeof(unsigned char*));
	for (int i = 0; i < height; i++)
	{
		*(image + i) = (unsigned char*)malloc(width * sizeof(unsigned char));
	}

	//reading the image
	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
			*(*(image + i) + j) = fgetc(imageFile);
	
	//initial value
	row = numberGenerator(key, width);
	col = numberGenerator(key, height);

	//watermarking process
	for (int i = 0; i < strlen(word); i++)
	{
		mask = 128;
		for (int n = 0; n < 8*sizeof(char);)//multiply by eight to get the size in bits rather than bytes
//modify the n put it in the else
		{
			bit = word[i] & mask;
			//modifications
			//verification process
			aux = getAverageAreaValue(image, row, col, width);
			if(errorPercentage(image[row][col],aux) >= 45 || errorPercentage(image[row][col],aux) == 0){
				row = numberGenerator(row, width);
				col = numberGenerator(col, height);
				putchar('1');
			}else{
				if (bit == 0)
				{
					image[row][col] = image[row][col] & ~mask;
				}
				else if (bit == mask)
				{
					image[row][col] = image[row][col] | mask;
				}
			
				mask = mask >> 1;
				row = numberGenerator(row, width);
				col = numberGenerator(col, height);
				putchar('0');
				n++;
			}//end of modifications
			//when it skips the n will still be incremented and the loop will end 
			//before getting through all the bits. I may need to fix that
		}
		putchar('\n');
	}


	//write the new watermarked file
	fprintf(outputFile, "%s%c%d%c%d%c%d%c", type, 10, width, 32, height, 10, grayscale, 10);

	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
			fputc(image[i][j], outputFile);

	fclose(imageFile);
	fclose(outputFile);

	return SUCCESSFUL;
}

int getAverageAreaValue(char** matrix, int x, int y, int limit){
	int sum = 0;
	int m, n = 0;

	for(int i = -1; i <= 1; i++){

		m = x + i;

		if(m < 0)
			m = 0;
		if(m > limit)
			m = limit;

		for(int j = -1; j <= 1; j++){

			n = y + j;

			if(n < 0)
				n = 0;
			if(n > limit)
				n = limit;

			sum += matrix[m][n];
		}
	}

	return sum / 9;
}

float errorPercentage(int theoreticalValue, int measuredValue){
	float error;
	
	error = ( abs(theoreticalValue - measuredValue) / (float)theoreticalValue );
	return error;
}

int numberFileCreator(){
	int one[9][4] = {
		WHITE,WHITE,WHITE,BLACK,
		WHITE,WHITE,WHITE,BLACK,
		WHITE,WHITE,WHITE,BLACK,
		WHITE,WHITE,WHITE,BLACK,
		WHITE,WHITE,WHITE,BLACK,
		WHITE,WHITE,WHITE,BLACK,
		WHITE,WHITE,WHITE,BLACK,
		WHITE,WHITE,WHITE,BLACK,
		WHITE,WHITE,WHITE,BLACK
		};

	FILE* numberFile;

	numberFile = fopen("numberFile.pgm", "wb");
	if(numberFile == NULL)
		return FAIL;

	fprintf(numberFile,"P5\n4 9\n255\n");
	for(int i = 0; i < 9; i++)
		for(int j = 0; j < 4; j++)
			fputc(one[i][j], numberFile);
	
	return SUCCESSFUL;
}
