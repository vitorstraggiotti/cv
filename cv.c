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
static int is_out_of_bound(int32_t KerRow, int32_t KerColumn, int32_t ImgRow, int32_t ImgColumn,
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
/* Receive "correlation_work_t" type and makes cross correlation on given row interval*/
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

	float	Acc = 0.0;
	float	KerWeight;
	int32_t	ImgTmpRow, ImgTmpCol;

	for(int32_t ImgRow = StartRow; ImgRow < EndRow; ImgRow++)
	{
		for(int32_t ImgColumn = 0; ImgColumn < ImgWidth; ImgColumn++)
		{
			/* For one element on a certain line inside a given interval do...*/

			/* Do cross correlation in one pixel (assuming grayscale with 3 channel) */
			for(int32_t KerRow = 0; KerRow < KerHeight; KerRow++)
			{
				for(int32_t KerColumn = 0; KerColumn < KerWidth; KerColumn++)
				{
					ImgTmpRow = KerRow - KerHeight/2 + ImgRow;
					ImgTmpCol = KerColumn - KerWidth/2 + ImgColumn;
					KerWeight = Args->Kernel->Weight[KerRow][KerColumn];
					
					/* Check if correspondent image pixel is out of bound */
					if(is_out_of_bound(KerRow, KerColumn, ImgRow, ImgColumn,
										ImgHeight, ImgWidth, KerHeight, KerWidth))
					{
						switch(Args->BorderHandling)
						{
							case BORDER_BLACK:
								/* Do nothing because in this case: Acc += 0 */
								break;

							case BORDER_WHITE:
								Acc += KerWeight * 255;
								break;

							default:
								printf("Error: selected border handling not suported.\n");
								exit(EXIT_FAILURE);
						}
					}
					else
					{
						Acc += KerWeight * Args->InputImage->Pixel24[ImgTmpRow][ImgTmpCol].Red;
					}
				}
			}
			
			if(Acc > 255)
				Acc = 255.0;
			else if(Acc < 0)
				Acc = 0;
				
			Args->OutputImage->Pixel24[ImgRow][ImgColumn].Red = (uint8_t)Acc;
			Args->OutputImage->Pixel24[ImgRow][ImgColumn].Green = (uint8_t)Acc;
			Args->OutputImage->Pixel24[ImgRow][ImgColumn].Blue = (uint8_t)Acc;
			Acc = 0.0;
		}
	}
}
/*******************************************************************************/
static void *convolution(void *ThreadArg)
{
	/* Args->(int32_t StartRow, int32_t EndRow kernel_t *Kernel img_t *InputImage) */
	correlation_work_t *Args = (correlation_work_t *)ThreadArg;

	float		**NewWeight;
	int32_t		TmpRow, TmpCol;

	/* Allocate memory for convolution kernel */		
	NewWeight = (float **)malloc(sizeof(float *) * Args->Kernel->Height);
	for(int32_t Row = 0; Row < Args->Kernel->Height; Row++)
	{
		NewWeight[Row] = (float *)malloc(sizeof(float) * Args->Kernel->Width);
	}

	/* Convolution is a cross correlation with the kernel matrix rotated 180° */
	/* Rotating kernel matrix 180° */
	for(int32_t Row = 0; Row < Args->Kernel->Height; Row++)
	{
		for(int32_t Column = 0; Column < Args->Kernel->Width; Column++)
		{
			TmpRow = Args->Kernel->Height - (1 + Row);
			TmpCol = Args->Kernel->Width - (1 + Column);
			NewWeight[Row][Column] = Args->Kernel->Weight[TmpRow][TmpCol];
		}
	}

	for(int32_t Row = 0; Row < Args->Kernel->Height; Row++)
	{
		free(Args->Kernel->Weight[Row]);
	}
	free(Args->Kernel->Weight);

	Args->Kernel->Weight = NewWeight;

	cross_correlation((void *)Args);
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
img_t *RGB_to_grayscale(img_t *InputImage, int Method)
{
	if(InputImage == NULL)
		return NULL;

	int32_t RedWeight, GreenWeight, BlueWeight;
	img_t	*OutImg;
	
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
			return NULL;
	}

	OutImg = new_BMP_as_size(InputImage);
	
	for(int32_t Row = 0; Row < InputImage->Height; Row++)
	{
		for(int32_t Column = 0; Column < InputImage->Width; Column++)
		{
			OutImg->Pixel24[Row][Column].Red = ((InputImage->Pixel24[Row][Column].Red * RedWeight) 
			                                  + (InputImage->Pixel24[Row][Column].Green * GreenWeight)
			                                  + (InputImage->Pixel24[Row][Column].Blue * BlueWeight))/10000;

			OutImg->Pixel24[Row][Column].Green = OutImg->Pixel24[Row][Column].Red;
			OutImg->Pixel24[Row][Column].Blue = OutImg->Pixel24[Row][Column].Red;
		}
	}

	return OutImg;
}

/*******************************************************************************/
/* Channel pass filter. Return -1 if fail or 0 on success.
	ChannelSelect --> PASS_RED_CHANNEL
	                  PASS_GREEN_CHANNEL
	                  PASS_BLUE_CHANNEL */
img_t *channel_pass_filter(img_t *InputImage, int ChannelSelect)
{
	if(InputImage == NULL)
		return NULL;

	img_t	*OutImg;

	OutImg = new_BMP_as_size(InputImage);

	switch(ChannelSelect)
	{
		case PASS_RED_CHANNEL :
			for(int32_t Row = 0; Row < InputImage->Height; Row++)
			{
				for(int32_t Column = 0; Column < InputImage->Width; Column++)
				{
					OutImg->Pixel24[Row][Column].Red = InputImage->Pixel24[Row][Column].Red;
					OutImg->Pixel24[Row][Column].Green = 0;
					OutImg->Pixel24[Row][Column].Blue = 0;
				}
			}
			break;
			
		case PASS_GREEN_CHANNEL :
			for(int32_t Row = 0; Row < InputImage->Height; Row++)
			{
				for(int32_t Column = 0; Column < InputImage->Width; Column++)
				{
					OutImg->Pixel24[Row][Column].Red = 0;
					OutImg->Pixel24[Row][Column].Green = InputImage->Pixel24[Row][Column].Green;
					OutImg->Pixel24[Row][Column].Blue = 0;
				}
			}
			break;
			
		case PASS_BLUE_CHANNEL :
			for(int32_t Row = 0; Row < InputImage->Height; Row++)
			{
				for(int Column = 0; Column < InputImage->Width; Column++)
				{
					OutImg->Pixel24[Row][Column].Red = 0;
					OutImg->Pixel24[Row][Column].Green = 0;
					OutImg->Pixel24[Row][Column].Blue = InputImage->Pixel24[Row][Column].Blue;
				}
			}
			break;
			
		default :
			printf("Error: invalid \"ChannelSelect\" input on channel_pass_filter function. Should be:\n");
			printf("       PASS_RED_CHANNEL, PASS_GREEN_CHANNEL or PASS_BLUE_CHANNEL\n\n");
			free_img(OutImg);
			return NULL;
	}

	return OutImg;
}
/*******************************************************************************/
/* Frees memory allocated by the kernel template */
void free_kernel(kernel_t *Kernel)
{
	for (int32_t Row = 0; Row < Kernel->Height; Row++)
	{
		free(Kernel->Weight[Row]);
	}
	free(Kernel->Weight);
	free(Kernel);
}
/*******************************************************************************/
/* Create a low pass filter kernel with given odd dimension. Return NULL if fail
   Type --> NEIGHBOR_AVERAGE */
kernel_t *create_kernel_low_pass_filter(int32_t Height, int32_t Width, int Type)
{
	kernel_t	*Kernel;

	/* Check if dimensions are odd */
	if(((Height % 2) == 0) || ((Width % 2) == 0))
	{
		printf("Error: kernel dimensions need to be odd.\n");
		return NULL;
	}

	/* Allocating kernel */
	Kernel = (kernel_t *)malloc(sizeof(kernel_t));
	Kernel->Weight = (float **)malloc(sizeof(float *) * Height);
	for(int32_t Row = 0; Row < Height; Row++)
	{
		Kernel->Weight[Row] = (float *)malloc(sizeof(float) * Width);
	}

	/* Assingning values */
	Kernel->Height = Height;
	Kernel->Width = Width;

	switch(Type)
	{
		case NEIGHBOR_AVERAGE:
			for(int32_t Row = 0; Row < Height; Row++)
			{
				for(int32_t Column = 0; Column < Width; Column++)
				{
					Kernel->Weight[Row][Column] = 1.0/(Height * Width);
				}
			}
			break;

		default:
			free_kernel(Kernel);
			printf("Error: Type not supported on low pass kernel creation.\n");
			return NULL;
	}

	return Kernel;
}
/*******************************************************************************/
/* Create a high pass filter kernel with given odd dimension. Return NULL if fail
   Type --> LAPLACIAN_OPERATOR */
kernel_t *create_kernel_high_pass_filter(int32_t Height, int32_t Width, int Type)
{
	kernel_t	*Kernel;

	/* Check if dimensions are odd */
	if(((Height % 2) == 0) || ((Width % 2) == 0))
	{
		printf("Error: kernel dimensions need to be odd.\n");
		return NULL;
	}	

	/* Allocating kernel */
	Kernel = (kernel_t *)malloc(sizeof(kernel_t));
	Kernel->Weight = (float **)malloc(sizeof(float *) * Height);
	for(int32_t Row = 0; Row < Height; Row++)
	{
		Kernel->Weight[Row] = (float *)malloc(sizeof(float) * Width);
	}

	/* Assingning values */
	Kernel->Height = Height;
	Kernel->Width = Width;

	switch(Type)
	{
		case LAPLACIAN_OPERATOR:
			for(int32_t Row = 0; Row < Height; Row++)
			{
				for(int32_t Column = 0; Column < Width; Column++)
				{
					if((Row == Height/2) && (Column == Width/2))
					{
						Kernel->Weight[Row][Column] = ((Height * Width) - 1)/(Height * Width);
					}
					else
					{
						Kernel->Weight[Row][Column] = -1.0/(Height * Width);
					}
				}
			}
			break;

		default:
			free_kernel(Kernel);
			printf("Error: Type not supported on high pass kernel creation.\n");
			return NULL;
	}

	return Kernel;
}
/*******************************************************************************/
/* Makes cross correlation betwen the kernel and image using multiple threads.
   Return NULL if fail
	Img     --> Pointer to source image. This image will also be the output.
	Kernel  --> Pointer to the the kernel to be used.
	Threads --> Number of threads to be used in parallel on computacion
	Border  --> BORDER_BLACK
	            BORDER_WHITE */
img_t *parallel_cross_correlation(img_t *Img, kernel_t *Kernel, int32_t Threads, int Border)
{
	if((Img == NULL) || (Kernel == NULL) || (Threads < 1))
		return NULL;

	correlation_work_t	ThreadArg;

	ThreadArg.StartRow = 0;
	ThreadArg.EndRow = Img->Height;
	ThreadArg.BorderHandling = Border;
	ThreadArg.Kernel = Kernel;
	ThreadArg.InputImage = Img;
	ThreadArg.OutputImage = new_BMP_as_size(Img);

	cross_correlation((void *)&ThreadArg);

	return ThreadArg.OutputImage;
}
/*******************************************************************************/
/* Makes convolution betwen the kernel and image using multiple threads.
   Return NULL if fail
	Img     --> Pointer to source image. This image will also be the output.
	Kernel  --> Pointer to the the kernel to be used.
	Threads --> Number of threads to be used in parallel on computacion
	Border  --> BORDER_BLACK
	            BORDER_WHITE */
img_t *parallel_convolution(img_t *Img, kernel_t *Kernel, int32_t Threads, int Border)
{
	if((Img == NULL) || (Kernel == NULL) || (Threads < 1))
		return NULL;

	correlation_work_t	ThreadArg;

	ThreadArg.StartRow = 0;
	ThreadArg.EndRow = Img->Height;
	ThreadArg.BorderHandling = Border;
	ThreadArg.Kernel = Kernel;
	ThreadArg.InputImage = Img;
	ThreadArg.OutputImage = new_BMP_as_size(Img);

	convolution((void *)&ThreadArg);

	return ThreadArg.OutputImage;
}











