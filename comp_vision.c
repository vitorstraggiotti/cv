/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **
 * Source code for computer vision functions									*
 *																				*
 * Author: Vitor Henrique Andrade Helfensteller Satraggiotti Silva				*
 * Start date: 06/06/2021 (DD/MM/YYYY)											*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
 
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../include/bitmap.h"
#include "../include/comp_vision.h"
#include "../include/load_animation_v1.0.h"


/*******************************************************************************/
//Convert RGB to grayscale [OK]
void RGB_to_grayscale(dimensions_t Dimension, pixel_t **PixelMatrix, int Method)
{
////(method: channels average) ==> GRAY_AVERAGE
////Pixel = (Red + Green + Blue)/3
////(method: channel-dependent luminance perception) ==> GRAY_LUMI_PERCEP
////Pixel = (0.2126Red + 0.7152Green + 0.0722Blue)/1
////(method: linear aproximation of gamma and luminance perception) ==> GRAY_APROX_GAM_LUMI_PERCEP
////Pixel = (0.299Red + 0.587Green + 0.114Blue)/1

	int RedWeight, GreenWeight, BlueWeight;
	
	switch(Method)
	{
		case GRAY_AVERAGE :
			RedWeight = 3333;
			GreenWeight = 3334;
			BlueWeight = 3333;
			break;
			
		case GRAY_LUMI_PERCEP :
			RedWeight = 2126;
			GreenWeight = 7152;
			BlueWeight = 722;
			break;
			
		case GRAY_APROX_GAM_LUMI_PERCEP :
			RedWeight = 2990;
			GreenWeight = 5870;
			BlueWeight = 1140;
			break;
			
		default :
			printf("Error: invalid \"Method\" input on RGB_to_grayscale function. Should be:\n");
			printf("       GRAY_AVERAGE, GRAY_LUMI_PERCEP or GRAY_APROX_GAM_LUMI_PERCEP\n\n");
			exit(EXIT_FAILURE);
	}
	
	for(int row = 0; row < Dimension.Height; row++)
	{
		for(int column = 0; column < Dimension.Width; column++)
		{
			PixelMatrix[row][column].Red = ((PixelMatrix[row][column].Red * RedWeight) 
			                               + (PixelMatrix[row][column].Green * GreenWeight)
			                               + (PixelMatrix[row][column].Blue * BlueWeight))/10000;

			PixelMatrix[row][column].Green = PixelMatrix[row][column].Red;
			PixelMatrix[row][column].Blue = PixelMatrix[row][column].Red;
		}
		
		print_progress((long) row, 0, (long) Dimension.Height - 1, 50);
	}
}

/*******************************************************************************/
//Channel pass filter
void channel_pass_filter(dimensions_t Dimension, pixel_t **PixelMatrix, int ChannelSelect)
{
////ChannelSelect = PASS_RED_CHANNEL
////ChannelSelect = PASS_GREEN_CHANNEL
////ChannelSelect = PASS_BLUE_CHANNEL

	switch(ChannelSelect)
	{
		case PASS_RED_CHANNEL :
			for(int row = 0; row < Dimension.Height; row++)
			{
				for(int column = 0; column < Dimension.Width; column++)
				{
					PixelMatrix[row][column].Green = 0;
					PixelMatrix[row][column].Blue = 0;
				}
				
				//Progress bar
				print_progress((long) row, 0, (long) Dimension.Height - 1, 50);
			}
			break;
			
		case PASS_GREEN_CHANNEL :
			for(int row = 0; row < Dimension.Height; row++)
			{
				for(int column = 0; column < Dimension.Width; column++)
				{
					PixelMatrix[row][column].Red = 0;
					PixelMatrix[row][column].Blue = 0;
				}
				
				//Progress bar
				print_progress((long) row, 0, (long) Dimension.Height - 1, 50);
			}
			break;
			
		case PASS_BLUE_CHANNEL :
			for(int row = 0; row < Dimension.Height; row++)
			{
				for(int column = 0; column < Dimension.Width; column++)
				{
					PixelMatrix[row][column].Red = 0;
					PixelMatrix[row][column].Green = 0;
				}
				
				//Progress bar
				print_progress((long) row, 0, (long) Dimension.Height - 1, 50);
			}
			break;
			
		default :
			printf("Error: invalid \"ChannelSelect\" input on channel_pass_filter function. Should be:\n");
			printf("       PASS_RED_CHANNEL, PASS_GREEN_CHANNEL or PASS_BLUE_CHANNEL\n\n");
			exit(EXIT_FAILURE);
	}
}

/******************************************************************************/
//Box blur kernel convolution. Edge handling: Crop. Kernel size: 3x3.
pixel_t** box_blur_kernel_conv(dimensions_t Dimension, pixel_t **PixelMatrix)
{
	pixel_t **FinalPixelMatrix;
	int SumRed, SumGreen, SumBlue;
	
	//Allocating memory for processed pixel matrix
	FinalPixelMatrix = malloc(Dimension.Height * sizeof(pixel_t*));
	for(int row = 0; row < Dimension.Height; row++)
	{
		FinalPixelMatrix[row] = malloc(Dimension.Width * sizeof(pixel_t));
	}
	
	//Executing kernel convolution
	for(int row = 0; row < Dimension.Height; row++)
	{	
		for(int column = 0; column < Dimension.Width; column++)
		{
			if((row > 0) && (row < Dimension.Height - 1) && (column > 0) && (column < Dimension.Width - 1))
			{
				SumRed = PixelMatrix[row + 1][column - 1].Red + //Top Left
				         PixelMatrix[row + 1][column].Red     + //Top center
				         PixelMatrix[row + 1][column + 1].Red + //Top right
				         PixelMatrix[row][column - 1].Red     + //Mid left
				         PixelMatrix[row][column].Red         + //Current pixel
				         PixelMatrix[row][column + 1].Red     + //Mid right
				         PixelMatrix[row - 1][column - 1].Red + //Low left
				         PixelMatrix[row - 1][column].Red     + //Low center
				         PixelMatrix[row - 1][column + 1].Red;  //Low right
				      
				SumGreen = PixelMatrix[row + 1][column - 1].Green + //Top Left
				           PixelMatrix[row + 1][column].Green     + //Top center
				           PixelMatrix[row + 1][column + 1].Green + //Top right
				           PixelMatrix[row][column - 1].Green     + //Mid left
				           PixelMatrix[row][column].Green         + //Current pixel
				           PixelMatrix[row][column + 1].Green     + //Mid right
				           PixelMatrix[row - 1][column - 1].Green + //Low left
				           PixelMatrix[row - 1][column].Green     + //Low center
				           PixelMatrix[row - 1][column + 1].Green;  //Low right
				      
				SumBlue = PixelMatrix[row + 1][column - 1].Blue + //Top Left
				          PixelMatrix[row + 1][column].Blue     + //Top center
				          PixelMatrix[row + 1][column + 1].Blue + //Top right
				          PixelMatrix[row][column - 1].Blue     + //Mid left
				          PixelMatrix[row][column].Blue         + //Current pixel
				          PixelMatrix[row][column + 1].Blue     + //Mid right
				          PixelMatrix[row - 1][column - 1].Blue + //Low left
				          PixelMatrix[row - 1][column].Blue     + //Low center
				          PixelMatrix[row - 1][column + 1].Blue;  //Low right

				FinalPixelMatrix[row][column].Red = (unsigned char) (SumRed / 9);
				FinalPixelMatrix[row][column].Green = (unsigned char) (SumGreen / 9);
				FinalPixelMatrix[row][column].Blue = (unsigned char) (SumBlue / 9);
				
			}else
			{
				FinalPixelMatrix[row][column].Red = PixelMatrix[row][column].Red;
				FinalPixelMatrix[row][column].Green = PixelMatrix[row][column].Green;
				FinalPixelMatrix[row][column].Blue = PixelMatrix[row][column].Blue;
			}
		}
		
		//Progress bar
		print_progress((long) row, 0, (long) Dimension.Height - 1, 50);
	}
	
	return FinalPixelMatrix;
}
/*******************************************************************************/
//Gaussian blur kernel convolution. Edge handling: Crop. Kernel size: 3x3.
pixel_t** gauss_blur_kernel_conv(dimensions_t Dimension, pixel_t** PixelMatrix)
{
	pixel_t **FinalPixelMatrix;
	int SumRed, SumGreen, SumBlue;
	
	//Allocating memory for processed pixel matrix
	FinalPixelMatrix = malloc(Dimension.Height * sizeof(pixel_t*));
	for(int row = 0; row < Dimension.Height; row++)
	{
		FinalPixelMatrix[row] = malloc(Dimension.Width * sizeof(pixel_t));
	}
	
	//Executing kernel convolution
	for(int row = 0; row < Dimension.Height; row++)
	{	
		for(int column = 0; column < Dimension.Width; column++)
		{
			if((row > 0) && (row < Dimension.Height - 1) && (column > 0) && (column < Dimension.Width - 1))
			{
				SumRed = PixelMatrix[row + 1][column - 1].Red       + //Top Left
				         PixelMatrix[row + 1][column].Red      * 2  + //Top center
				         PixelMatrix[row + 1][column + 1].Red       + //Top right
				         PixelMatrix[row][column - 1].Red      * 2  + //Mid left
				         PixelMatrix[row][column].Red          * 4  + //Current pixel
				         PixelMatrix[row][column + 1].Red      * 2  + //Mid right
				         PixelMatrix[row - 1][column - 1].Red       + //Low left
				         PixelMatrix[row - 1][column].Red      * 2  + //Low center
				         PixelMatrix[row - 1][column + 1].Red;        //Low right
				      
				SumGreen = PixelMatrix[row + 1][column - 1].Green       + //Top Left
				           PixelMatrix[row + 1][column].Green      * 2  + //Top center
				           PixelMatrix[row + 1][column + 1].Green       + //Top right
				           PixelMatrix[row][column - 1].Green      * 2  + //Mid left
				           PixelMatrix[row][column].Green          * 4  + //Current pixel
				           PixelMatrix[row][column + 1].Green      * 2  + //Mid right
				           PixelMatrix[row - 1][column - 1].Green       + //Low left
				           PixelMatrix[row - 1][column].Green      * 2  + //Low center
				           PixelMatrix[row - 1][column + 1].Green;        //Low right
				      
				SumBlue = PixelMatrix[row + 1][column - 1].Blue       + //Top Left
				          PixelMatrix[row + 1][column].Blue      * 2  + //Top center
				          PixelMatrix[row + 1][column + 1].Blue       + //Top right
				          PixelMatrix[row][column - 1].Blue      * 2  + //Mid left
				          PixelMatrix[row][column].Blue          * 4  + //Current pixel
				          PixelMatrix[row][column + 1].Blue      * 2  + //Mid right
				          PixelMatrix[row - 1][column - 1].Blue       + //Low left
				          PixelMatrix[row - 1][column].Blue      * 2  + //Low center
				          PixelMatrix[row - 1][column + 1].Blue;        //Low right

				FinalPixelMatrix[row][column].Red = (unsigned char) (SumRed / 16);
				FinalPixelMatrix[row][column].Green = (unsigned char) (SumGreen / 16);
				FinalPixelMatrix[row][column].Blue = (unsigned char) (SumBlue / 16);
				
			}else
			{
				FinalPixelMatrix[row][column].Red = PixelMatrix[row][column].Red;
				FinalPixelMatrix[row][column].Green = PixelMatrix[row][column].Green;
				FinalPixelMatrix[row][column].Blue = PixelMatrix[row][column].Blue;
			}
		}
		
		//Progress bar
		print_progress((long) row, 0, (long) Dimension.Height - 1, 50);
	}
	
	return FinalPixelMatrix;
}
/*******************************************************************************/
//Sobel edge kernel convolution for RGB channels. Edge handling: Crop. Kernel size: 3x3.
pixel_t** sobel_edge_kernel_conv(dimensions_t Dimension, pixel_t** PixelMatrix)
{
	pixel_t **FinalPixelMatrix;
	int SumRedSobelX, SumGreenSobelX, SumBlueSobelX; //Edge magnitude on X axis 
	int SumRedSobelY, SumGreenSobelY, SumBlueSobelY; //Edge magnitude on Y axis
	int RedSobelMag, GreenSobelMag, BlueSobelMag;    //Resultant magnitude for each channel
	
	FinalPixelMatrix = malloc(Dimension.Height * sizeof(pixel_t*));
	for(int row = 0; row < Dimension.Height; row++)
	{
		FinalPixelMatrix[row] = malloc(Dimension.Width * sizeof(pixel_t));
	}
	
	for(int row = 0; row < Dimension.Height; row++)
	{	
		for(int column = 0; column < Dimension.Width; column++)
		{
			if((row > 0) && (row < Dimension.Height - 1) && (column > 0) && (column < Dimension.Width - 1))
			{
				// Sobel - X
				SumRedSobelX = PixelMatrix[row + 1][column - 1].Red  * (-1)		+ //Top Left
								//PixelMatrix[row + 1][column].Red				+ //Top center
								PixelMatrix[row + 1][column + 1].Red			+ //Top right
								PixelMatrix[row][column - 1].Red      * (-2)	+ //Mid left
								//PixelMatrix[row][column].Red					+ //Current pixel
								PixelMatrix[row][column + 1].Red      * 2		+ //Mid right
								PixelMatrix[row - 1][column - 1].Red  * (-1)	+ //Low left
								//PixelMatrix[row - 1][column].Red				+ //Low center
								PixelMatrix[row - 1][column + 1].Red;			  //Low right
				      
				SumGreenSobelX = PixelMatrix[row + 1][column - 1].Green  * (-1)	+ //Top Left
								//PixelMatrix[row + 1][column].Green			+ //Top center
								PixelMatrix[row + 1][column + 1].Green			+ //Top right
								PixelMatrix[row][column - 1].Green      * (-2)	+ //Mid left
								//PixelMatrix[row][column].Green				+ //Current pixel
								PixelMatrix[row][column + 1].Green      * 2		+ //Mid right
								PixelMatrix[row - 1][column - 1].Green  * (-1)	+ //Low left
								//PixelMatrix[row - 1][column].Green			+ //Low center
								PixelMatrix[row - 1][column + 1].Green;			  //Low right
				      
				SumBlueSobelX = PixelMatrix[row + 1][column - 1].Blue  * (-1)	+ //Top Left
								//PixelMatrix[row + 1][column].Blue				+ //Top center
								PixelMatrix[row + 1][column + 1].Blue			+ //Top right
								PixelMatrix[row][column - 1].Blue      * (-2)	+ //Mid left
								//PixelMatrix[row][column].Blue					+ //Current pixel
								PixelMatrix[row][column + 1].Blue      * 2		+ //Mid right
								PixelMatrix[row - 1][column - 1].Blue  * (-1)	+ //Low left
								//PixelMatrix[row - 1][column].Blue				+ //Low center
								PixelMatrix[row - 1][column + 1].Blue;			  //Low right

				// Sobel - Y
				SumRedSobelY = PixelMatrix[row + 1][column - 1].Red  * (-1)		+ //Top Left
								PixelMatrix[row + 1][column].Red     * (-2)		+ //Top center
								PixelMatrix[row + 1][column + 1].Red * (-1)		+ //Top right
								//PixelMatrix[row][column - 1].Red				+ //Mid left
								//PixelMatrix[row][column].Red					+ //Current pixel
								//PixelMatrix[row][column + 1].Red				+ //Mid right
								PixelMatrix[row - 1][column - 1].Red			+ //Low left
								PixelMatrix[row - 1][column].Red     * 2		+ //Low center
								PixelMatrix[row - 1][column + 1].Red;			  //Low right
				      
				SumGreenSobelY = PixelMatrix[row + 1][column - 1].Green  * (-1)	+ //Top Left
								PixelMatrix[row + 1][column].Green       * (-2)	+ //Top center
								PixelMatrix[row + 1][column + 1].Green   * (-1)	+ //Top right
								//PixelMatrix[row][column - 1].Green			+ //Mid left
								//PixelMatrix[row][column].Green				+ //Current pixel
								//PixelMatrix[row][column + 1].Green			+ //Mid right
								PixelMatrix[row - 1][column - 1].Green			+ //Low left
								PixelMatrix[row - 1][column].Green       * 2	+ //Low center
								PixelMatrix[row - 1][column + 1].Green;			  //Low right
				      
				SumBlueSobelY = PixelMatrix[row + 1][column - 1].Blue  * (-1)	+ //Top Left
								PixelMatrix[row + 1][column].Blue      * (-2)	+ //Top center
								PixelMatrix[row + 1][column + 1].Blue  * (-1)	+ //Top right
								//PixelMatrix[row][column - 1].Blue				+ //Mid left
								//PixelMatrix[row][column].Blue					+ //Current pixel
								//PixelMatrix[row][column + 1].Blue				+ //Mid right
								PixelMatrix[row - 1][column - 1].Blue			+ //Low left
								PixelMatrix[row - 1][column].Blue      * 2		+ //Low center
								PixelMatrix[row - 1][column + 1].Blue;			  //Low right
								
				RedSobelMag = (int) sqrt(pow((double) SumRedSobelX, 2.0) + pow((double) SumRedSobelY, 2.0));
				GreenSobelMag = (int) sqrt(pow((double) SumGreenSobelX, 2.0) + pow((double) SumGreenSobelY, 2.0));
				BlueSobelMag = (int) sqrt(pow((double) SumBlueSobelX, 2.0) + pow((double) SumBlueSobelY, 2.0));
				
				if(RedSobelMag > 255) RedSobelMag = 255;
				if(RedSobelMag < 0) RedSobelMag = 0;
				
				if(GreenSobelMag > 255) GreenSobelMag = 255;
				if(GreenSobelMag < 0) GreenSobelMag = 0;
				
				if(BlueSobelMag > 255) BlueSobelMag = 255;
				if(BlueSobelMag < 0) BlueSobelMag = 0;
				
				FinalPixelMatrix[row][column].Red = (unsigned char) RedSobelMag;
				FinalPixelMatrix[row][column].Green = (unsigned char) GreenSobelMag;
				FinalPixelMatrix[row][column].Blue = (unsigned char) BlueSobelMag;
				
			}else
			{
				FinalPixelMatrix[row][column].Red = PixelMatrix[row][column].Red;
				FinalPixelMatrix[row][column].Green = PixelMatrix[row][column].Green;
				FinalPixelMatrix[row][column].Blue = PixelMatrix[row][column].Blue;
			}
		}
		
		//Progress bar
		print_progress((long) row, 0, (long) Dimension.Height - 1, 50);
	}
	
	return FinalPixelMatrix;
}

