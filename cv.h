/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **
 * Header file of the library for computer vision functions				*
 *																		*
 * Author: Vitor Henrique Andrade Helfensteller Straggiotti Silva		*
 * Created on: 05/01/2022 (DD/MM/YYYY)									*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
 
 #ifndef __CV_H__
 #define __CV_H__

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

 #include "bitmap.h"
 
/*******************************************************************************
 *                                   STRUCTURES                                *
 *******************************************************************************/
/* Template to use with cross correlation and convolution with images */
struct kernel
{
	int32_t	Width;
	int32_t	Height;
	float	**Weight;
};
typedef struct kernel kernel_t;

/* Hold arguments to do multithreaded cross correlation and convolution
	Interval for correlation or convolution is NOT closed i.e. [StartRow:EndRow[ */
struct cross_correlation_work
{
	int32_t		StartRow;
	int32_t		EndRow;
	int32_t		BorderHandling;
	kernel_t	*Kernel;
	img_t		*InputImage;
	
};
typedef struct cross_correlation_work correlation_work_t;

/*******************************************************************************
 *                                 DEFINITIONS                                 *
 *******************************************************************************/
/* Conversion method selection for "RGB_to_grayscale" function */
enum GrayMethod 
{
	GRAY_AVERAGE,
	GRAY_LUMI_PERCEP,
	GRAY_APROX_GAM_LUMI_PERCEP
};

/* Channel pass selection for "channel_pass_filter" function */
enum ChannelPass
{
	PASS_RED_CHANNEL,
	PASS_GREEN_CHANNEL,
	PASS_BLUE_CHANNEL
};

/* Border handling on cross correlation and convolution */
enum BorderHandling
{
	BORDER_BLACK,
	BORDER_WHITE,
	BORDER_REPLICATE,
	BORDER_REFLECT
};
/*******************************************************************************
 *                                  FUNCTIONS                                  *
 *******************************************************************************/

//------------------------------------------------------------------------------
//Convert RGB to grayscale. Return -1 if fail or 0 on success. 
////Method = GRAY_AVERAGE               (channels average) 
////method = GRAY_LUMI_PERCEP           (channel-dependent luminance perception)  
////method = GRAY_APROX_GAM_LUMI_PERCEP (linear aproximation of gamma and luminance perception) 
int RGB_to_grayscale(img_t *InputImage, int Method);
//------------------------------------------------------------------------------
//Channel pass filter. Return -1 if fail or 0 on success.
////ChannelSelect = PASS_RED_CHANNEL
////ChannelSelect = PASS_GREEN_CHANNEL
////ChannelSelect = PASS_BLUE_CHANNEL
int channel_pass_filter(img_t *InputImage, int ChannelSelect);

#error "Paste function declarations on header file"


 #endif
 
