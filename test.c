/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **
 * Program to test cv library													*
 *																				*
 * Author: Vitor Henrique Andrade Helfensteller Straggiotti Silva				*
 * Created on: 09/01/2022 (DD/MM/YYYY)											*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/

#include <stdio.h>
#include <stdlib.h>

#include "cv.h"
#include "bitmap.h"

void exit_msg(const char *Message, int Code)
{
	printf(Message);
	exit(Code);
}

int main(int argc, char* argv[])
{
	if(argc != 2)
	{
		printf("Error: wrong number of arguments.\n");
		printf("Use: test <path_to_file>\n");
		exit(EXIT_FAILURE);
	}

	img_t		*InputImage;

	img_t		*ImgToGrayAverage;
	img_t		*ImgToGrayLumi;
	img_t		*ImgToGrayAprox;

	img_t		*ImgRedFilter;
	img_t		*ImgGreenFilter;
	img_t		*ImgBlueFilter;

	kernel_t	*LowPassKernel;
	kernel_t	*HighPassKernel;

	int32_t		ThreadNum;

	img_t		*ImgLowPassBlack;
	img_t		*ImgLowPassWhite;
	img_t		*ImgHighPassBlack;
	img_t		*ImgHighPassWhite;

	img_t		*ImgConvLowPassBlack;
	img_t		*ImgConvLowPassWhite;
	img_t		*ImgConvHighPassBlack;
	img_t		*ImgConvHighPassWhite;

	InputImage = read_BMP(argv[1]);

	/*===========================================================================*/
	/*                        TESTING: RGB_to_grayscale()                        */
	/*===========================================================================*/
	printf("Converting to grayscale (GRAY_AVERAGE) ...\n");
	ImgToGrayAverage = RGB_to_grayscale(InputImage, GRAY_AVERAGE);
	if(ImgToGrayAverage == NULL)
		exit_msg("Error: conversion to grayscale failed on \"GRAY_AVERAGE\"\n",
					EXIT_FAILURE);

	printf("Converting to grayscale (GRAY_LUMI_PERCEP) ...\n");
	ImgToGrayLumi = RGB_to_grayscale(InputImage, GRAY_LUMI_PERCEP);
	if(ImgToGrayLumi == NULL)
		exit_msg("Error: conversion to grayscale failed on \"GRAY_LUMI_PPERCEP\"\n",
					EXIT_FAILURE);

	printf("Converting to grayscale (GRAY_APROX_GAM_LUMI_PERCEP) ...\n");
	ImgToGrayAprox = RGB_to_grayscale(InputImage, GRAY_APROX_GAM_LUMI_PERCEP);
	if(ImgToGrayAprox == NULL)
		exit_msg("Error: conversion to grayscale failed on \"GRAY_APROX_GAM_LUMIPERCEP\"\n",
					EXIT_FAILURE);

	printf("Saving grayscale images ...\n\n");

	if(save_BMP(ImgToGrayAverage, "saida1-Gray_average.bmp") == -1)
		exit_msg("Error: Could not save \"Gray_average\" image file.\n", EXIT_FAILURE);

	if(save_BMP(ImgToGrayLumi, "saida2-Gray_lumi.bmp") == -1)
		exit_msg("Error: Could not save \"Gray_lumi\" image file.\n", EXIT_FAILURE);

	if(save_BMP(ImgToGrayAprox, "saida3-Gray_aprox.bmp") == -1)
		exit_msg("Error: Could not save \"Gray_aprox\" image file.\n", EXIT_FAILURE);
		
	free_img(ImgToGrayAverage);
	free_img(ImgToGrayLumi);
	free_img(ImgToGrayAprox);

	/*===========================================================================*/
	/*                       TESTING: channel_pass_filter()                      */
	/*===========================================================================*/
	printf("Image filtering (RED CHANNEL) ...\n");
	ImgRedFilter = channel_pass_filter(InputImage, PASS_RED_CHANNEL);
	if(ImgRedFilter == NULL)
		exit_msg("Error: red channel pass filter failed.\n", EXIT_FAILURE);

	printf("Image filtering (GREEN CHANNEL) ...\n");
	ImgGreenFilter = channel_pass_filter(InputImage, PASS_GREEN_CHANNEL);
	if(ImgGreenFilter == NULL)
		exit_msg("Error: green channel pass filter failed.\n", EXIT_FAILURE);

	printf("Image filtering (BLUE CHANNEL) ...\n");
	ImgBlueFilter = channel_pass_filter(InputImage, PASS_BLUE_CHANNEL);
	if(ImgBlueFilter == NULL)
		exit_msg("Error: blue channel pass filter failed.\n", EXIT_FAILURE);

	printf("Saving filtered images ...\n\n");

	if(save_BMP(ImgRedFilter, "saida4-Red_filter.bmp") == -1)
		exit_msg("Error: Could not save \"Red_filter\" image file.\n", EXIT_FAILURE);

	if(save_BMP(ImgGreenFilter, "saida5-Green_filter.bmp") == -1)
		exit_msg("Error: Could not save \"Green_filter\" image file.\n", EXIT_FAILURE);

	if(save_BMP(ImgBlueFilter, "saida6-Blue_filter.bmp") == -1)
		exit_msg("Error: Could not save \"Blue_filter\" image file.\n", EXIT_FAILURE);

	free_img(ImgRedFilter);
	free_img(ImgGreenFilter);
	free_img(ImgBlueFilter);

	/*===========================================================================*/
	/*                TESTING: create_kernel_low_pass_filter()                   */
	/*===========================================================================*/
	printf("Creating low pass filter kernel ...\n");
	LowPassKernel = create_kernel_low_pass_filter(5, 5, NEIGHBOR_AVERAGE);
	if(LowPassKernel == NULL)
		exit_msg("Error: Could not create low pass filter kernel.\n", EXIT_FAILURE);

	/*===========================================================================*/
	/*                TESTING: create_kernel_high_pass_filter()                   */
	/*===========================================================================*/
	printf("Creating high pass filter kernel ...\n\n");
	HighPassKernel = create_kernel_high_pass_filter(3, 3, LAPLACIAN_OPERATOR);
	if(HighPassKernel == NULL)
		exit_msg("Error: Could not create high pass filter kernel.\n", EXIT_FAILURE);

	/*===========================================================================*/
	/*                 TESTING: parallel_cross_correlation()                     */
	/*===========================================================================*/
	ThreadNum = 4;
	ImgToGrayAverage = RGB_to_grayscale(InputImage, GRAY_AVERAGE);
	
	printf("Making cross correlation (LOW_PASS, BLACK_BORDER) ...\n");
	ImgLowPassBlack = parallel_cross_correlation(ImgToGrayAverage, LowPassKernel, ThreadNum, BORDER_BLACK);
	if(ImgLowPassBlack == NULL)
		exit_msg("Error: Could not make cross correlation (LOW_PASS, BLACK_BORDER).\n",
					EXIT_FAILURE);

	printf("Making cross correlation (LOW_PASS, WHITE_BORDER) ...\n");
	ImgLowPassWhite = parallel_cross_correlation(ImgToGrayAverage, LowPassKernel, ThreadNum, BORDER_WHITE);
	if(ImgLowPassWhite == NULL)
		exit_msg("Error: Could not make cross correlation (LOW_PASS, BLACK_WHITE).\n",
					EXIT_FAILURE);

	printf("Making cross correlation (HIGH_PASS, BLACK_BORDER) ...\n");
	ImgHighPassBlack = parallel_cross_correlation(ImgToGrayAverage, HighPassKernel, ThreadNum, BORDER_BLACK);
	if(ImgHighPassBlack == NULL)
		exit_msg("Error: Could not make cross correlation (HIGH_PASS, BLACK_BORDER).\n",
					EXIT_FAILURE);

	printf("Making cross correlation (HIGH_PASS, WHITE_BORDER) ...\n");
	ImgHighPassWhite = parallel_cross_correlation(ImgToGrayAverage, HighPassKernel, ThreadNum, BORDER_WHITE);
	if(ImgHighPassWhite == NULL)
		exit_msg("Error: Could not make cross correlation (HIGH_PASS, BLACK_WHITE).\n",
					EXIT_FAILURE);

	printf("Saving cross correlated images ...\n\n");

	if(save_BMP(ImgLowPassBlack, "saida7-LowPassBlack.bmp") == -1)
		exit_msg("Error: Could not save \"LowPassBlack\" image file.\n", EXIT_FAILURE);

	if(save_BMP(ImgLowPassWhite, "saida8-LowPassWhite.bmp") == -1)
		exit_msg("Error: Could not save \"LowPassWhite\" image file.\n", EXIT_FAILURE);

	if(save_BMP(ImgHighPassBlack, "saida9-HighPassBlack.bmp") == -1)
		exit_msg("Error: Could not save \"HighPassBlack\" image file.\n", EXIT_FAILURE);

	if(save_BMP(ImgHighPassWhite, "saida10-HighPassWhite.bmp") == -1)
		exit_msg("Error: Could not save \"HighPassWhite\" image file.\n", EXIT_FAILURE);

	free_img(ImgLowPassBlack);
	free_img(ImgLowPassWhite);
	free_img(ImgHighPassBlack);
	free_img(ImgHighPassWhite);

	/*===========================================================================*/
	/*                 TESTING: parallel_convolution()                           */
	/*===========================================================================*/
	printf("Making convolution (LOW_PASS, BLACK_BORDER) ...\n");
	ImgConvLowPassBlack = parallel_convolution(ImgToGrayAverage, LowPassKernel, ThreadNum, BORDER_BLACK);
	if(ImgConvLowPassBlack == NULL)
		exit_msg("Error: Could not make convolution (LOW_PASS, BLACK_BORDER).\n",
					EXIT_FAILURE);

	printf("Making convolution (LOW_PASS, WHITE_BORDER) ...\n");
	ImgConvLowPassWhite = parallel_convolution(ImgToGrayAverage, LowPassKernel, ThreadNum, BORDER_WHITE);
	if(ImgConvLowPassWhite == NULL)
		exit_msg("Error: Could not make convolution (LOW_PASS, BLACK_WHITE).\n",
					EXIT_FAILURE);

	printf("Making convolution (HIGH_PASS, BLACK_BORDER) ...\n");
	ImgConvHighPassBlack = parallel_convolution(ImgToGrayAverage, HighPassKernel, ThreadNum, BORDER_BLACK);
	if(ImgConvHighPassBlack == NULL)
		exit_msg("Error: Could not make convolution (HIGH_PASS, BLACK_BORDER).\n",
					EXIT_FAILURE);

	printf("Making convolution (HIGH_PASS, WHITE_BORDER) ...\n");
	ImgConvHighPassWhite = parallel_convolution(ImgToGrayAverage, HighPassKernel, ThreadNum, BORDER_WHITE);
	if(ImgConvHighPassWhite == NULL)
		exit_msg("Error: Could not make convolution (HIGH_PASS, BLACK_WHITE).\n",
					EXIT_FAILURE);

	printf("Saving convoluted images ...\n\n");

	if(save_BMP(ImgConvLowPassBlack, "saida11-ConvLowPassBlack.bmp") == -1)
		exit_msg("Error: Could not save \"ConvLowPassBlack\" image file.\n", EXIT_FAILURE);

	if(save_BMP(ImgConvLowPassWhite, "saida12-ConvLowPassWhite.bmp") == -1)
		exit_msg("Error: Could not save \"ConvLowPassWhite\" image file.\n", EXIT_FAILURE);

	if(save_BMP(ImgConvHighPassBlack, "saida13-ConvHighPassBlack.bmp") == -1)
		exit_msg("Error: Could not save \"ConvHighPassBlack\" image file.\n", EXIT_FAILURE);

	if(save_BMP(ImgConvHighPassWhite, "saida14-ConvHighPassWhite.bmp") == -1)
		exit_msg("Error: Could not save \"ConvHighPassWhite\" image file.\n", EXIT_FAILURE);

	free_img(ImgConvLowPassBlack);
	free_img(ImgConvLowPassWhite);
	free_img(ImgConvHighPassBlack);
	free_img(ImgConvHighPassWhite);

	/*===========================================================================*/
	/*                           TESTING: free_kernel()                          */
	/*===========================================================================*/
	free_kernel(LowPassKernel);
	free_kernel(HighPassKernel);

	/*===========================================================================*/
	/*                           TESTING: histogram()                            */
	/*===========================================================================*/
	printf("Generating histogram ...\n\n");
	histogram(ImgToGrayAverage);

	
	free_img(InputImage);
	free_img(ImgToGrayAverage);

	return 0;
}
