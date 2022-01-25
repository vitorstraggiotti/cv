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
#include <stdint.h>
#include <pthread.h>

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
	img_t		*OutputImage;
	
};
typedef struct cross_correlation_work correlation_work_t;

/*******************************************************************************
 *                                 DEFINITIONS                                 *
 *******************************************************************************/
/* Conversion method selection for "RGB_to_grayscale" function */
enum gray_method 
{
	GRAY_AVERAGE,
	GRAY_LUMI_PERCEP,
	GRAY_APROX_GAM_LUMI_PERCEP
};

/* Channel pass selection for "channel_pass_filter" function */
enum channel_pass
{
	PASS_RED_CHANNEL,
	PASS_GREEN_CHANNEL,
	PASS_BLUE_CHANNEL
};

/* Border handling on cross correlation and convolution */
enum border_handling
{
	BORDER_BLACK,
	BORDER_WHITE
/*	BORDER_REPLICATE,    (not implemented yet)*/
/*	BORDER_REFLECT       (not implemented yet)*/
};

/* Defines the type of low pass filter kernel */
enum lf_kernel_filter
{
	NEIGHBOR_AVERAGE
};

/* Defines the type of high pass filter kernel */
enum hf_kernel_filter
{
	LAPLACIAN_OPERATOR_NORM,  /* Normalized kernel */
	LAPLACIAN_OPERATOR
};
/*******************************************************************************
 *                                  FUNCTIONS                                  *
 *******************************************************************************/

/* Convert RGB to grayscale. Return NULL if fail 
	Method --> GRAY_AVERAGE               (channels average) 
	           GRAY_LUMI_PERCEP           (channel-dependent luminance perception)  
	           GRAY_APROX_GAM_LUMI_PERCEP (linear aproximation of gamma and luminance perception) */
img_t *RGB_to_grayscale(img_t *InputImage, int Method);


/* Channel pass filter. Return NULL if fail
	ChannelSelect --> PASS_RED_CHANNEL
	                  PASS_GREEN_CHANNEL
	                  PASS_BLUE_CHANNEL */
img_t *channel_pass_filter(img_t *InputImage, int ChannelSelect);


/* Frees memory allocated by the kernel template */
void free_kernel(kernel_t *Kernel);


/* Create a low pass filter kernel with given odd dimension. Return NULL if fail
   Type --> NEIGHBOR_AVERAGE */
kernel_t *create_kernel_low_pass_filter(int32_t Height, int32_t Width, int Type);


/* Create a high pass filter kernel with given odd dimension. Return NULL if fail
   Type --> LAPLACIAN_OPERATOR */
kernel_t *create_kernel_high_pass_filter(int32_t Height, int32_t Width, int Type);


/* Makes cross correlation betwen the kernel and image using multiple threads.
   Return NULL if fail
	Img     --> Pointer to source image. This image will also be the output.
	Kernel  --> Pointer to the the kernel to be used.
	Threads --> Number of threads to be used in parallel on computacion
	Border  --> BORDER_BLACK
	            BORDER_WHITE */
img_t *parallel_cross_correlation(img_t *Img, kernel_t *Kernel, int32_t Threads, int Border);


/* Makes convolution betwen the kernel and image using multiple threads.
   Return NULL if fail
	Img     --> Pointer to source image. This image will also be the output.
	Kernel  --> Pointer to the the kernel to be used.
	Threads --> Number of threads to be used in parallel on computacion
	Border  --> BORDER_BLACK
	            BORDER_WHITE */
img_t *parallel_convolution(img_t *Img, kernel_t *Kernel, int32_t Threads, int Border);

/* Generate the histogram for a given image */
void histogram(img_t *Img);


#endif
 
