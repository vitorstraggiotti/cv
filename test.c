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

	img_t *InputImage;

	img_t *ImgToGrayAverage;
	img_t *ImgToGrayLumi;
	img_t *ImgToGrayAprox;

	img_t *ImgRedFilter;
	img_t *ImgGreenFilter;
	img_t *ImgBlueFilter;

	InputImage = read_BMP(argv[1]);

	/*===========================================================================*/
	/*                        TESTING: RGB_to_grayscale()                        */
	/*===========================================================================*/
	printf("Converting to grayscale (GRAY_AVERAGE) ...\n");
	ImgToGrayAverage = copy_BMP(InputImage);
	if(RGB_to_grayscale(ImgToGrayAverage, GRAY_AVERAGE) == -1)
		exit_msg("Error: conversion to grayscale failed on \"GRAY_AVERAGE\"\n",
					EXIT_FAILURE);

	printf("Converting to grayscale (GRAY_LUMI_PERCEP) ...\n");
	ImgToGrayLumi = copy_BMP(InputImage);
	if(RGB_to_grayscale(ImgToGrayLumi, GRAY_LUMI_PERCEP) == -1)
		exit_msg("Error: conversion to grayscale failed on \"GRAY_LUMI_PPERCEP\"\n",
					EXIT_FAILURE);

	printf("Converting to grayscale (GRAY_APROX_GAM_LUMI_PERCEP) ...\n");
	ImgToGrayAprox = copy_BMP(InputImage);
	if(RGB_to_grayscale(ImgToGrayAprox, GRAY_APROX_GAM_LUMI_PERCEP) == -1)
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
	ImgRedFilter = copy_BMP(InputImage);
	if(channel_pass_filter(ImgRedFilter, PASS_RED_CHANNEL) == -1)
		exit_msg("Error: red channel pass filter failed.\n", EXIT_FAILURE);

	printf("Image filtering (GREEN CHANNEL) ...\n");
	ImgGreenFilter = copy_BMP(InputImage);
	if(channel_pass_filter(ImgGreenFilter, PASS_GREEN_CHANNEL) == -1)
		exit_msg("Error: green channel pass filter failed.\n", EXIT_FAILURE);

	printf("Image filtering (BLUE CHANNEL) ...\n");
	ImgBlueFilter = copy_BMP(InputImage);
	if(channel_pass_filter(ImgBlueFilter, PASS_BLUE_CHANNEL) == -1)
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
	/*                       TESTING:                       */
	/*===========================================================================*/
		
	free_img(InputImage);

	return 0;
}
