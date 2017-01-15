#include "crypto.h"

//encrypting based on the special rule
int visualSpecEncrypt(char* imageFileNameOne, char* imageFileNameTwo) 
{
	int noiseBlock[6][2][2] = { 
			{ WHITE,BLACK,BLACK,WHITE },
			{ BLACK,BLACK,WHITE,WHITE },
			{ WHITE,BLACK,WHITE,BLACK },
			{ BLACK,WHITE,BLACK,WHITE },
			{ WHITE,WHITE,BLACK,BLACK },
			{ BLACK,WHITE,WHITE,BLACK }
				 };

	FILE* imageFileOne;
	FILE* imageFileTwo;
	FILE* baseNoiseFile;
	FILE* noiseImageFileOne;
	FILE* noiseImageFileTwo;
	int* imageBufferOne;
	int* imageBufferTwo;
	int** baseBlock;
	int** imageBlockOne;
	int** imageBlockTwo;
	int r, r1, r2;
	int pixel = 0;
	char type[3];
	int width;
	int height;
	int grayscale;
	int rowNumberOne = -1;//this will store the value of the row where the secret message starts
	int rowNumberTwo = -1;	
	int rowCountOne = 0;//this will count the number of rows that were read, this is crucial to determining the rowNumber
	int rowCountTwo = 0;
	int textHeightOne = 0;//this will determine the heigtj of the slit to be ecnrypted
	int textHeightTwo = 0;
	int row = 0;//auxiliary variable to be used in the encryption process
	int whiteCount = 0;

	//opening the two image files
	imageFileOne = fopen(imageFileNameOne, "rb");
	imageFileTwo = fopen(imageFileNameTwo, "rb");

	if(imageFileOne == NULL || imageFileTwo == NULL)
		return FAIL;

	//reading the header for the respective image files
	headerReader(imageFileOne, type, &height, &width, &grayscale);
	headerReader(imageFileTwo, type, &height, &width, &grayscale);

	//allocating memory for the image file buffers
	//the buffers are the size of the width of the image because
	//the program reads one line at a time
	imageBufferOne = (int*)malloc(width*sizeof(int));
	imageBufferTwo = (int*)malloc(width*sizeof(int));
	
	//allocating memory for the "buffers" that are twice the size of the image buffers
	baseBlock = (int**)malloc(2 * sizeof(int*));
	*(baseBlock) = (int*)malloc(2*width*sizeof(int));
	*(baseBlock + 1) = (int*)malloc(2 * width*sizeof(int));

	imageBlockOne = (int**)malloc(2 * sizeof(int*));
	*(imageBlockOne) = (int*)malloc(2*width*sizeof(int));
	*(imageBlockOne + 1) = (int*)malloc(2 * width*sizeof(int));

	imageBlockTwo = (int**)malloc(2 * sizeof(int*));
	*(imageBlockTwo) = (int*)malloc(2 * width*sizeof(int));
	*(imageBlockTwo + 1) = (int*)malloc(2 * width*sizeof(int));

	//opening the destination files
	baseNoiseFile = fopen("encrypted/baseSpecNoise.pgm", "wb");
	noiseImageFileOne = fopen("encrypted/clientSpecOneShare.pgm", "wb");
	noiseImageFileTwo = fopen("encrypted/clientSpecTwoShare.pgm", "wb");

	if (baseNoiseFile == NULL || noiseImageFileOne == NULL || noiseImageFileTwo == NULL)
		return FAIL;
	
	//writing the headers of the files
	fprintf(baseNoiseFile, "%s\n%d %d\n%d\n", type, 2*width, 2*height, grayscale);	
	fprintf(noiseImageFileOne, "%s\n%d %d\n%d\n", type, 2*width, 2*height, grayscale);
	fprintf(noiseImageFileTwo, "%s\n%d %d\n%d\n", type, 2*width, 2*height, grayscale);
	
	//locating the secret slot in the first image
	while(!feof(imageFileOne)){
		for(int i = 0; i < width; i++){
			imageBufferOne[i] = fgetc(imageFileOne);
			
			if(imageBufferOne[i] == WHITE)
				whiteCount++;
		}
		
		if(whiteCount != width){
			if(rowNumberOne == -1)
				rowNumberOne = rowCountOne;
			
			textHeightOne++;
		}
		rowCountOne++;
		whiteCount = 0;
	}


	//locating the secret slot in the second image
	while(!feof(imageFileTwo)){
		for(int i = 0; i < width; i++){
			imageBufferTwo[i] = fgetc(imageFileTwo);
			
			if(imageBufferTwo[i] == WHITE)
				whiteCount++;
		}
		
		if(whiteCount != width){
			if(rowNumberTwo == -1)
				rowNumberTwo = rowCountTwo;
			
			textHeightTwo++;
		}
		rowCountTwo++;
		whiteCount = 0;
	}

	//rewinding the image file pointers
	rewind(imageFileOne);
	rewind(imageFileTwo);
	headerReader(imageFileOne, type, &height, &width, &grayscale);
	headerReader(imageFileTwo, type, &height, &width, &grayscale);

	//encrypting the files
	while (!feof(imageFileOne))
	{
		for (int i = 0; i < width; i++){
			imageBufferOne[i] = fgetc(imageFileOne);
			imageBufferTwo[i] = fgetc(imageFileTwo);
		}

		for (int n = 0; n < width; n++)
		{
			r = rand() % 6;
			r1 = rand() % 6;
			r2 = rand() % 6;
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++)
				{
					baseBlock[i][(j + 2*n)] = noiseBlock[r][i][j];

					if(row >= rowNumberOne && row < textHeightOne + rowNumberOne){
						if (imageBufferOne[n] == BLACK)
						{
							imageBlockOne[i][(j + 2 * n)] = noiseBlock[5-r][i][j];
						}
						else
						{
							imageBlockOne[i][(j + 2 * n)] = noiseBlock[r][i][j];
						}//end if else
					}
					else{
						imageBlockOne[i][(j + 2 * n)] = noiseBlock[r1][i][j];
					}
					
					if(row >= rowNumberTwo && row < textHeightTwo + rowNumberTwo){
						if (imageBufferTwo[n] == BLACK)
						{
							imageBlockTwo[i][(j + 2 * n)] = noiseBlock[5-r][i][j];
						}
						else
						{
							imageBlockTwo[i][(j + 2 * n)] = noiseBlock[r][i][j];
						}//end if else
					}
					else{
						imageBlockTwo[i][(j + 2 * n)] = noiseBlock[r2][i][j];
					}
				}//end for j
			}//end for i

		}//end for n


		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 2*width; j++)
			{
				fputc(baseBlock[i][j], baseNoiseFile);
				fputc(imageBlockOne[i][j], noiseImageFileOne);
				fputc(imageBlockTwo[i][j], noiseImageFileTwo);
			}//end for
		}//end for
		row++;
	}//end while

	return SUCCESSFUL;
}//end visualEncrypt with special rule


//encrypting two client shares at the same time
int visualMultiEncrypt(char* imageFileNameOne, char* imageFileNameTwo) 
{
	int noiseBlock[6][2][2] = { 
			{ WHITE,BLACK,BLACK,WHITE },
			{ BLACK,BLACK,WHITE,WHITE },
			{ WHITE,BLACK,WHITE,BLACK },
			{ BLACK,WHITE,BLACK,WHITE },
			{ WHITE,WHITE,BLACK,BLACK },
			{ BLACK,WHITE,WHITE,BLACK }
				 };

	FILE* imageFileOne;
	FILE* imageFileTwo;
	FILE* baseNoiseFile;
	FILE* noiseImageFileOne;
	FILE* noiseImageFileTwo;
	int* imageBufferOne;
	int* imageBufferTwo;
	int** baseBlock;
	int** imageBlockOne;
	int** imageBlockTwo;
	int r;
	int pixel = 0;
	char type[3];
	int width;
	int height;
	int grayscale;

	//opening the two image files
	imageFileOne = fopen(imageFileNameOne, "rb");
	imageFileTwo = fopen(imageFileNameTwo, "rb");

	if(imageFileOne == NULL || imageFileTwo == NULL)
		return FAIL;

	//reading the header for the respective image files
	headerReader(imageFileOne, type, &height, &width, &grayscale);
	headerReader(imageFileTwo, type, &height, &width, &grayscale);

	//allocating memory for the image file buffers
	//the buffers are the size of the width of the image because
	//the program reads one line at a time
	imageBufferOne = (int*)malloc(width*sizeof(int));
	imageBufferTwo = (int*)malloc(width*sizeof(int));
	
	//allocating memory for the "buffers" that are twice the size of the image buffers
	baseBlock = (int**)malloc(2 * sizeof(int*));
	*(baseBlock) = (int*)malloc(2*width*sizeof(int));
	*(baseBlock + 1) = (int*)malloc(2 * width*sizeof(int));

	imageBlockOne = (int**)malloc(2 * sizeof(int*));
	*(imageBlockOne) = (int*)malloc(2*width*sizeof(int));
	*(imageBlockOne + 1) = (int*)malloc(2 * width*sizeof(int));

	imageBlockTwo = (int**)malloc(2 * sizeof(int*));
	*(imageBlockTwo) = (int*)malloc(2 * width*sizeof(int));
	*(imageBlockTwo + 1) = (int*)malloc(2 * width*sizeof(int));

	//opening the destination files
	baseNoiseFile = fopen("encrypted/baseNoise.pgm", "wb");
	noiseImageFileOne = fopen("encrypted/clientOneShare.pgm", "wb");
	noiseImageFileTwo = fopen("encrypted/clientTwoShare.pgm", "wb");

	if (baseNoiseFile == NULL || noiseImageFileOne == NULL || noiseImageFileTwo == NULL)
		return FAIL;
	
	//writing the headers of the files
	fprintf(baseNoiseFile, "%s\n%d %d\n%d\n", type, 2*width, 2*height, grayscale);	
	fprintf(noiseImageFileOne, "%s\n%d %d\n%d\n", type, 2*width, 2*height, grayscale);
	fprintf(noiseImageFileTwo, "%s\n%d %d\n%d\n", type, 2*width, 2*height, grayscale);
	
	while (!feof(imageFileOne))
	{
		for (int i = 0; i < width; i++){
			imageBufferOne[i] = fgetc(imageFileOne);
			imageBufferTwo[i] = fgetc(imageFileTwo);
		}

		for (int n = 0; n < width; n++)
		{
			r = rand() % 6;//I may need to change that at some point
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++)
				{
					baseBlock[i][(j + 2*n)] = noiseBlock[r][i][j];

					if (imageBufferOne[n] == BLACK)
					{
						imageBlockOne[i][(j + 2 * n)] = noiseBlock[5-r][i][j];
					}
					else
					{
						imageBlockOne[i][(j + 2 * n)] = noiseBlock[r][i][j];
					}//end if else
					
					if (imageBufferTwo[n] == BLACK)
					{
						imageBlockTwo[i][(j + 2 * n)] = noiseBlock[5-r][i][j];
					}
					else
					{
						imageBlockTwo[i][(j + 2 * n)] = noiseBlock[r][i][j];
					}//end if else

				}//end for
			}//end for
		}//end for


		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 2*width; j++)
			{
				fputc(baseBlock[i][j], baseNoiseFile);
				fputc(imageBlockOne[i][j], noiseImageFileOne);
				fputc(imageBlockTwo[i][j], noiseImageFileTwo);
			}//end for
		}//end for

	}//end while

	return SUCCESSFUL;
}//end visualEncrypt


//Name for the encryption pattern
int visualEncrypt(char* imageFileName) 
{
	int noiseBlock[6][2][2] = { 
			{ WHITE,BLACK,BLACK,WHITE },
			{ BLACK,BLACK,WHITE,WHITE },
			{ WHITE,BLACK,WHITE,BLACK },
			{ BLACK,WHITE,BLACK,WHITE },
			{ WHITE,WHITE,BLACK,BLACK },
			{ BLACK,WHITE,WHITE,BLACK }
				 };

	FILE* imageFile;
	FILE* noiseImageFileOne;
	FILE* noiseImageFileTwo;
	int* imageBuffer;
	int** imageBlockOne;
	int** imageBlockTwo;
	int r;
	int pixel = 0;
	char type[3];
	int width;
	int height;
	int grayscale;

	imageFile = fopen(imageFileName, "rb");
	if(imageFile == NULL)
		return FAIL;

	headerReader(imageFile,type,&height,&width,&grayscale);
	
	imageBuffer = (int*)malloc(width*sizeof(int));

	imageBlockOne = (int**)malloc(2 * sizeof(int*));
	*(imageBlockOne) = (int*)malloc(2*width*sizeof(int));
	*(imageBlockOne + 1) = (int*)malloc(2 * width*sizeof(int));

	imageBlockTwo = (int**)malloc(2 * sizeof(int*));
	*(imageBlockTwo) = (int*)malloc(2 * width*sizeof(int));
	*(imageBlockTwo + 1) = (int*)malloc(2 * width*sizeof(int));

	noiseImageFileOne = fopen("encrypted/visualShareOne.pgm", "wb");
	if (noiseImageFileOne == NULL)
		return FAIL;

	noiseImageFileTwo = fopen("encrypted/visualShareTwo.pgm", "wb");
	if (noiseImageFileTwo == NULL)
		return FAIL;
	
	fprintf(noiseImageFileOne, "%s\n%d %d\n%d\n", type, 2*width, 2*height, grayscale);
	fprintf(noiseImageFileTwo, "%s\n%d %d\n%d\n", type, 2*width, 2*height, grayscale);
	
	while (!feof(imageFile))
	{
		for (int i = 0; i < width; i++)
			imageBuffer[i] = fgetc(imageFile);

		for (int n = 0; n < width; n++)
		{
			r = rand() % 6;//I may need to change that at some point
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++)
				{
					imageBlockOne[i][(j + 2*n)] = noiseBlock[r][i][j];

					if (imageBuffer[n] == BLACK)
					{
						imageBlockTwo[i][(j + 2 * n)] = noiseBlock[5-r][i][j];
					}
					else
					{
						imageBlockTwo[i][(j + 2 * n)] = noiseBlock[r][i][j];
					}//end if else
				}//end for
			}//end for
		}//end for


		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 2*width; j++)
			{
				fputc(imageBlockOne[i][j], noiseImageFileOne);
				fputc(imageBlockTwo[i][j], noiseImageFileTwo);
			}//end for
		}//end for

	}//end while

	return SUCCESSFUL;
}//end visualEncrypt


int visualDecrypt(char* shareOneFileName, char* shareTwoFileName)
{
	FILE* output;
	FILE* shareOne;
	FILE* shareTwo;
	short* bufferOne;
	short* bufferTwo;
	char type[3];
	int width;
	int height;
	int grayscale;

	shareOne = fopen(shareOneFileName,"rb");
	shareTwo = fopen(shareTwoFileName, "rb");
	output = fopen("decrypted/visualDecrypted.pgm", "wb");

	if(shareOne == NULL || shareTwo == NULL || output == NULL){
		puts("Unable to open file...");
		return FAIL;
	}

	headerReader(shareOne, type, &height, &width, &grayscale);
	headerReader(shareTwo, type, &height, &width, &grayscale);
	fprintf(output, "%s\n%d %d\n%d\n", type, width, height, grayscale);

	bufferOne = (short*)malloc(width*sizeof(short));
	bufferTwo = (short*)malloc(width*sizeof(short));

	while(!feof(shareOne))
	{
		for (int j = 0; j < width; j++)
		{
			bufferOne[j] = fgetc(shareOne);
			bufferTwo[j] = fgetc(shareTwo);

			if ((bufferOne[j] == bufferTwo[j]) && (bufferOne[j] == WHITE))
				fputc(WHITE, output);
			else
				fputc(BLACK, output);
		}
	}
	
	fclose(shareOne);
	fclose(shareTwo);
	fclose(output);

	return SUCCESSFUL;
}

int encryptCarrierImage(char* payloadFileName, char* carrierFileName)
{
	int noiseBlock[6][2][2] = { 
				{ WHITE,BLACK,BLACK,WHITE },
				{ BLACK,BLACK,WHITE,WHITE },
				{ WHITE,BLACK,WHITE,BLACK },
				{ BLACK,WHITE,BLACK,WHITE },
				{ WHITE,WHITE,BLACK,BLACK },
				{ BLACK,WHITE,WHITE,BLACK } 
				};
	FILE* payloadFile;
	FILE* carrierFile;
	FILE* shareOne;
	FILE* shareTwo;
	int* imageBuffer;
	int* carrierImageBuffer;
	int** shareOneBlock;
	int** shareTwoBlock;
	int r;
	char typeImage[3];
	int widthImage;
	int heightImage;
	int grayscaleImage;
	char typeCarrier[3];
	int widthCarrier;
	int heightCarrier;
	int grayscaleCarrier;

	payloadFile = fopen(payloadFileName, "rb");
	carrierFile = fopen(carrierFileName, "rb");
	shareOne = fopen("encrypted/carrierShareOne.pgm","wb");
	shareTwo = fopen("encrypted/carrierShareTwo.pgm","wb");

	if(shareOne == NULL || shareTwo == NULL || payloadFile == NULL || carrierFile == NULL){
		puts("Unable to open File...");
		return FAIL;
	}

	//reading the headers of the payload image and the carrier image
	headerReader(payloadFile,typeImage,&heightImage,&widthImage,&grayscaleImage);
	headerReader(carrierFile,typeCarrier,&heightCarrier,&widthCarrier,&grayscaleCarrier);

	//if the dimensions of the images do not match the funtion will terminate will a fail 
	if(heightImage != heightCarrier || widthImage != widthCarrier)
		return FAIL;


	//allocating the memory for the buffers of the payload and carrier
	imageBuffer = (int*)malloc(widthImage*sizeof(int));
	carrierImageBuffer = (int*)malloc(widthCarrier*sizeof(int));

	//allocating the memory for the square matrices that will contain the encrypted payload
	shareOneBlock = (int**)malloc(2*sizeof(int*));
	*(shareOneBlock) = (int*)malloc(2*widthImage*sizeof(int));
	*(shareOneBlock + 1) = (int*)malloc(2*widthImage*sizeof(int));

	shareTwoBlock = (int**)malloc(2*sizeof(int*));
	*(shareTwoBlock) = (int*)malloc(2*widthImage*sizeof(int));
	*(shareTwoBlock + 1) = (int*)malloc(2*widthImage*sizeof(int));

	//writing the header to the two shares of the carrier image
	fprintf(shareOne,"%s%c%d%c%d%c%d%c", typeImage, 10, 2 * widthImage, 32, 2 * heightImage, 10, grayscaleImage, 10);
	fprintf(shareTwo, "%s%c%d%c%d%c%d%c", typeImage, 10, 2 * widthImage, 32, 2 * heightImage, 10, grayscaleImage, 10);
	
	//process of encryption
	while(!feof(payloadFile))
	{
		//read one line of bytes from the payload and the carrier
		for(int i = 0; i < widthImage; i++)
		{
			imageBuffer[i] = fgetc(payloadFile);
			carrierImageBuffer[i] = fgetc(carrierFile);
		}

		//creation of the two shares of the carrier image
		for (int n = 0; n < widthImage; n++)
		{
			r = rand() % 6;
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++)
				{
					shareOneBlock[i][(j + 2*n)] = noiseBlock[r][i][j];

					if (imageBuffer[n] == BLACK)
					{
						shareTwoBlock[i][(j + 2 * n)] = noiseBlock[5-r][i][j];
					}
					else
					{
						shareTwoBlock[i][(j + 2 * n)] = noiseBlock[r][i][j];
					}//end if else
				}//end for
			}//end for
		}//end for

		for (int n = 0; n < widthImage; n++)
		{
			for (int i = 0; i < 2; i++)
			{
				for (int j = 2 * n; j < 2 * (n + 1);j++)
				{	
					if(shareOneBlock[i][j] == WHITE)
						shareOneBlock[i][j] = carrierImageBuffer[n];

					if(shareTwoBlock[i][j] == WHITE)
						shareTwoBlock[i][j] = carrierImageBuffer[n];
				}//end for
			}//end for
		}//end for

		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 2 * widthImage; j++)
			{
				fputc(shareOneBlock[i][j], shareOne);
				fputc(shareTwoBlock[i][j], shareTwo);
			}//end for
		}//end for

		//exit(0);
	}//end while

	fclose(shareOne);
	fclose(shareTwo);
	fclose(payloadFile);
	fclose(carrierFile);

	return SUCCESSFUL;

}//end encryptCarrierImage

//decryption function for the two share images
int decryptCarrierImage(char* shareOneFileName, char* shareTwoFileName)
{
	int pixelOne;
	int pixelTwo;
	char type[3];
	int width;
	int height;
	int grayscale;
	FILE* outputFile;
	FILE* shareOne;
	FILE* shareTwo;

	shareOne = fopen(shareOneFileName, "rb");
	shareTwo = fopen(shareTwoFileName, "rb");
	outputFile = fopen("decrypted/carrierDecrypted.pgm", "wb");
	
	if(shareOne == NULL || shareTwo == NULL || outputFile == NULL){
		puts("Unable to open file...");
		return FAIL;
	}

	headerReader(shareOne, type, &height, &width, &grayscale);
	headerReader(shareTwo, type, &height, &width, &grayscale);

	fprintf(outputFile, "%s%c%d%c%d%c%d%c", type, 10, width, 32, height, 10, grayscale, 10);

	while (!feof(shareOne))
	{
		pixelOne = fgetc(shareOne);
		pixelTwo = fgetc(shareTwo);

		if (pixelOne != BLACK && pixelTwo != BLACK)
			fputc(pixelOne, outputFile);
		else
			fputc(BLACK, outputFile);
	}

	fclose(shareOne);
	fclose(shareTwo);
	fclose(outputFile);

	return SUCCESSFUL;
}
