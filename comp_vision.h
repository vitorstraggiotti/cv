/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **
 * Header file of the library for computer vision functions				*
 *																		*
 * Author: Vitor Henrique Andrade Helfensteller Straggiotti Silva		*
 * Created on: 06/06/2021 (DD/MM/YYYY)									*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
 
 #ifndef __COMP_VISION_H__
 #define __COMP_VISION_H__
 

/*******************************************************************************
 *                            MACROS AND TYPEDEF                               *
 *******************************************************************************/

//Conversion method selection for "RGB_to_grayscale" function
#define GRAY_AVERAGE				1
#define GRAY_LUMI_PERCEP			2
#define GRAY_APROX_GAM_LUMI_PERCEP	3

//Channel pass selection for "channel_pass_filter" function
#define PASS_RED_CHANNEL			4
#define PASS_GREEN_CHANNEL			5
#define PASS_BLUE_CHANNEL			6


/*******************************************************************************
 *                                  FUNCTIONS                                  *
 *******************************************************************************/

//------------------------------------------------------------------------------
//Convert RGB to grayscale 
////Method = GRAY_AVERAGE               (channels average) 
////method = GRAY_LUMI_PERCEP           (channel-dependent luminance perception)  
////method = GRAY_APROX_GAM_LUMI_PERCEP (linear aproximation of gamma and luminance perception) 
void RGB_to_grayscale(dimensions_t Dimension, pixel_t **PixelMatrix, int Method);
//------------------------------------------------------------------------------
//Channel pass filter
////ChannelSelect = PASS_RED_CHANNEL
////ChannelSelect = PASS_GREEN_CHANNEL
////ChannelSelect = PASS_BLUE_CHANNEL
void channel_pass_filter(dimensions_t Dimension, pixel_t **PixelMatrix, int ChannelSelect);
//------------------------------------------------------------------------------
//Box blur kernel convolution. Edge handling: Crop. Kernel size: 3x3.
pixel_t** box_blur_kernel_conv(dimensions_t Dimension, pixel_t** PixelMatrix);
//------------------------------------------------------------------------------
//Gaussian blur kernel convolution. Edge handling: Crop. Kernel size: 3x3.
pixel_t** gauss_blur_kernel_conv(dimensions_t Dimension, pixel_t** PixelMatrix);
//------------------------------------------------------------------------------
//Sobel edge kernel convolution. Edge handling: Crop. Kernel size: 3x3.
pixel_t** sobel_edge_kernel_conv(dimensions_t Dimension, pixel_t** PixelMatrix);


/*******************************************************************************
 *                                   STRUCTURES                                *
 *******************************************************************************/

 #endif
 
 
 
 
 
 
 
 
 
