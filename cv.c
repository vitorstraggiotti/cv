/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **
 * Source code for computer vision functions									*
 *																				*
 * Author: Vitor Henrique Andrade Helfensteller Satraggiotti Silva				*
 * Start date: 06/01/2022 (DD/MM/YYYY)											*
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
 
#include "cv.h"

/*=============================================================================*/
/*##########                    HELPER FUNCTIONS                     ##########*/
/*=============================================================================*/
int is_out_of_bound(int32_t KerRow, int32_t KerColumn, int32_t ImgRow, int32_t ImgColumn,
					int32_t ImgHeight, int32_t ImgWidth, int32_t KerHeight, int32_t KerWidth)
{
	int32_t RowToCheck = (KerRow - KerHeight/2) + ImgRow;
	int32_t ColumnToCheck = (KerColumn - KerWidth/2) + ImgColumn;

	if((RowToCheck < 0) || (RowToCheck >= ImgHeight) || 
		(ColumnToCheck < 0) || (ColumnToCheck >= ImgWidth))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
/*******************************************************************************/
static void *cross_correlation(void *ThreadArg)
{
	/* Args->(int32_t StartRow, int32_t EndRow kernel_t *Kernel img_t *InputImage) */
	correlation_work_t *Args = (correlation_work_t *)ThreadArg;

	int32_t StartRow	= Args->StartRow;
	int32_t EndRow		= Args->EndRow;
	int32_t ImgHeight	= Args->InputImage->Height;
	int32_t ImgWidth	= Args->InputImage->Width;
	
	int32_t KerHeight	= Args->Kernel->Height;
	int32_t KerWidth	= Args->Kernel->Width;

	for(int32_t ImgRow = StartRow; ImgRow < EndRow; ImgRow++)
	{
		for(int32_t ImgColumn = 0; ImgColumn < ImgWidth; ImgColumn++)
		{
			/* For one element on a certain line inside a given interval do...*/

			/* Do cross correlation in one pixel (assuming grayscale with 3 channel) */
			for(int32_t KerRow = 0; KerRow < KerHeight; KerRow++)
			{
				for(int32_t KerColumn = 0; KerRow < KerWidth; KerColumn++)
				{
					/* Check if correspondent image pixel is out of bound */
					if(is_out_of_bound(KerRow, KerColumn, ImgRow, ImgColumn,
										ImgHeight, ImgWidth, KerHeight, KerWidth))
					{
						switch(Args->BorderHandling)
						{
							case BORDER_BLACK:
								#error "implement"
								break;

							case BORDER_WHITE:
								#error "implement"
								break;

							case BORDER_REPLICATE:
								#error "implement"
								break;

							case BORDER_REFLECT:
								#error "implment"
								break;

							default:
								printf("Error: selected border handling not suported.\n");
								exit(EXIT_FAILURE);
						}
					}
					else
					{
						#error "implement"
					}
				}
			}
		}
	}
}
/*******************************************************************************/
static void *convolution(void *ThreadArg)
{
	#error "implement convolution() function"
}

/*=============================================================================*/
/*##########                     MAIN FUNCTIONS                      ##########*/
/*=============================================================================*/
/*******************************************************************************/
/* Convert RGB to grayscale. Return -1 on failure or 0 on success. [NEED TEST]
	(method: channels average) ==> GRAY_AVERAGE
	Pixel = (Red + Green + Blue)/3
	(method: channel-dependent luminance perception) ==> GRAY_LUMI_PERCEP
	Pixel = (0.2126Red + 0.7152Green + 0.0722Blue)/1
	(method: linear aproximation of gamma and luminance perception) ==> GRAY_APROX_GAM_LUMI_PERCEP
	Pixel = (0.299Red + 0.587Green + 0.114Blue)/1 */
int RGB_to_grayscale(img_t *InputImage, int Method)
{
	if(InputImage == NULL)
		return -1;

	int32_t RedWeight, GreenWeight, BlueWeight;
	
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
			return -1;
	}
	
	for(int32_t Row = 0; Row < InputImage->Height; Row++)
	{
		for(int32_t Column = 0; Column < InputImage->Width; Column++)
		{
			InputImage->Pixel24[Row][Column].Red = ((InputImage->Pixel24[Row][Column].Red * RedWeight) 
			                                     + (InputImage->Pixel24[Row][Column].Green * GreenWeight)
			                                     + (InputImage->Pixel24[Row][Column].Blue * BlueWeight))/10000;

			InputImage->Pixel24[Row][Column].Green = InputImage->Pixel24[Row][Column].Red;
			InputImage->Pixel24[Row][Column].Blue = InputImage->Pixel24[Row][Column].Red;
		}
	}

	return 0;
}

/*******************************************************************************/
/* Channel pass filter. Return -1 if fail or 0 on success.
	ChannelSelect = PASS_RED_CHANNEL
	ChannelSelect = PASS_GREEN_CHANNEL
	ChannelSelect = PASS_BLUE_CHANNEL */
int channel_pass_filter(img_t *InputImage, int ChannelSelect)
{
	if(InputImage == NULL)
		return -1;

	switch(ChannelSelect)
	{
		case PASS_RED_CHANNEL :
			for(int32_t Row = 0; Row < InputImage->Height; Row++)
			{
				for(int32_t Column = 0; Column < InputImage->Width; Column++)
				{
					InputImage->Pixel24[Row][Column].Green = 0;
					InputImage->Pixel24[Row][Column].Blue = 0;
				}
			}
			break;
			
		case PASS_GREEN_CHANNEL :
			for(int32_t Row = 0; Row < InputImage->Height; Row++)
			{
				for(int32_t Column = 0; Column < InputImage->Width; Column++)
				{
					InputImage->Pixel24[Row][Column].Red = 0;
					InputImage->Pixel24[Row][Column].Blue = 0;
				}
			}
			break;
			
		case PASS_BLUE_CHANNEL :
			for(int32_t Row = 0; Row < InputImage->Height; Row++)
			{
				for(int Column = 0; Column < InputImage->Width; Column++)
				{
					InputImage->Pixel24[Row][Column].Red = 0;
					InputImage->Pixel24[Row][Column].Green = 0;
				}
			}
			break;
			
		default :
			printf("Error: invalid \"ChannelSelect\" input on channel_pass_filter function. Should be:\n");
			printf("       PASS_RED_CHANNEL, PASS_GREEN_CHANNEL or PASS_BLUE_CHANNEL\n\n");
			return -1;
	}

	return 0;
}
/*******************************************************************************/
kernel_t *create_kernel_low_pass_filter(void)
{
	#error "implement create_kernel_low_pass_filter() function"
}
/*******************************************************************************/
kernel_t *create_kernel_high_pass_filter(void)
{
	#error "implement create_kernel_high_pass_filter() function"
}
/*******************************************************************************/
void free_kernel(kernel_t *kernel)
{
	#error "implement free_kernel() function"
}
/*******************************************************************************/
/*	BORDER_BLACK,
	BORDER_WHITE,
	BORDER_REPLICATE,
	BORDER_REFLECT */
int parallel_cross_correlation(void)
{
	#error "implement parallel_cross_correlation() function"
}
/*******************************************************************************/
int parallel_convolution(void)
{
	#error "implement parallel_convolution() function"
}











