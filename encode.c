/*/*
Name          :Gowri Menon
Date          :12-07-24
Description   :Image Steganography
Sample Input  :
Sample Output :
*/
#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel 
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
Status read_and_validate_encode_args(char *argv[],EncodeInfo *encInfo)
{
	if(strcmp(strstr(argv[2],".") ,".bmp") == 0)
	{
		encInfo -> src_image_fname = argv[2] ;
	}
	else
	{ 
		return e_failure ;
	}
	if(strcmp(strstr(argv[3],"."),".txt") == 0)
	{
		encInfo ->secret_fname = argv[3] ;
	}
	else
	{
		return e_failure ;
	}
	if(argv[4] != NULL)   
	{
		encInfo -> stego_image_fname =argv[4] ;
	}
	else
	{
		encInfo -> stego_image_fname = "stego.bmp" ;
	}
	return e_success;
}

			
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}
uint get_file_size(FILE *fptr_secret)
{
	fseek(fptr_secret, 0,SEEK_END) ;
	return ftell(fptr_secret) ;
}
Status check_capacity(EncodeInfo *encInfo)
{  
	encInfo->image_capacity =get_image_size_for_bmp(encInfo->fptr_src_image) ;    encInfo->size_secret_file=get_file_size(encInfo -> fptr_secret);
	if(encInfo ->image_capacity >(54+(2+4+4+4+encInfo ->size_secret_file)*8))
	//checking if image capacity is greater than the secret file size
	{
		return e_success ;
	}
	else
	{
		return e_failure ;
	}
}
Status copy_bmp_header(FILE *fptr_src,FILE *fptr_stego)
{
	//copying 54 bytes of header as such
	fseek(fptr_src,0,SEEK_SET) ;
	char str[54] ;
	fread(str,sizeof(char),54,fptr_src);
	fwrite(str,sizeof(char),54,fptr_stego) ;
	return e_success;
}
Status encode_byte_to_lsb(char data,char * image_buffer)
{  int i ;
	for(i=0; i<8 ;i++)
	{
image_buffer[i] = (image_buffer[i] & 0XFE)| ((data >> ( 7-i )) & 1) ;
	 }
}
Status encode_data_to_image(char *data,int size,FILE *fptr_src_img,FILE *fptr_stego_img,EncodeInfo *encInfo )
{

	//call encode byte to lsb
	//each time pass 1 byte data along with 8byte 
	for(int i=0 ;i<size ;i++)
	{
		fread(encInfo ->image_data,8,1,fptr_src_img);
		encode_byte_to_lsb(data[i],encInfo->image_data) ;
		fwrite(encInfo->image_data,8,1,fptr_stego_img) ;
	}
	return e_success ;
}	
Status encode_magic_string(const char *magic_string,EncodeInfo *encInfo)
{   //reusable function
	encode_data_to_image((char *)magic_string,strlen(magic_string),encInfo->fptr_src_image,encInfo ->fptr_stego_image,encInfo) ;
	return e_success ;
}
Status encode_secret_file_extn(char *file_extn,EncodeInfo *encInfo)
{
	encode_data_to_image(file_extn,strlen(file_extn),encInfo ->fptr_src_image,encInfo ->fptr_stego_image,encInfo) ;
	return e_success ;
}
Status encode_secret_file_data(EncodeInfo *encInfo)
{
	char str[encInfo -> size_secret_file] ;
	fseek(encInfo -> fptr_secret,0,SEEK_SET) ;
	fread(str,encInfo -> size_secret_file,1,encInfo->fptr_secret) ;
	encode_data_to_image(str,strlen(str),encInfo->fptr_src_image,encInfo -> fptr_stego_image,encInfo) ;
}
Status encode_extn_size(int size,EncodeInfo *encInfo)

{
	char str[32] ;
	fread(str,32,1,encInfo ->fptr_src_image) ;
	encode_size_to_lsb(size,str) ;
	fwrite(str,32,1,encInfo -> fptr_stego_image) ;
	return e_success ;
}
Status encode_secret_file_size(int file_size,EncodeInfo *encInfo)
{
	char str[32] ;
	fread(str,32,1,encInfo -> fptr_src_image) ;
	encode_size_to_lsb(file_size,str) ;
	fwrite(str,32,1,encInfo ->fptr_stego_image) ;
	return e_success ;
}
Status encode_size_to_lsb(int size,char *image_buffer)
{
	int i ;
	for(i=0 ;i<32;i++)
	{
		image_buffer[i] =(image_buffer[i] & 0XFE) | ((size >>(31 -i)) &1) ;
	}

}
Status copy_remaining_img_data(FILE *fptr_src_image,FILE *fptr_dest_image)
{
	char ch ;
	while(fread(&ch,1,1,fptr_src_image) >0)
	{
		fwrite(&ch,1,1,fptr_dest_image) ;
	}
	return e_success ;
				}
Status do_encoding(EncodeInfo *encInfo)
{
	if(open_files(encInfo) == e_success)
	{
		printf("Opened file successfully\n") ;
		printf("started Encoding\n") ;
		if(check_capacity(encInfo) == e_success)
		{
			printf("Secret data can be encoded in .bmp\n") ;
			//copy 54 bytes header
			if(copy_bmp_header(encInfo ->fptr_src_image,encInfo -> fptr_stego_image) == e_success)
			{
				printf("Copied bmp header successfully \n") ;

			
			if(encode_magic_string(MAGIC_STRING,encInfo) == e_success)
			{
				printf("Encoded magic string succesfully\n") ;
			    strcpy(encInfo ->extn_secret_file,strstr(encInfo -> secret_fname, "."));
					if(encode_extn_size(strlen(encInfo -> extn_secret_file),encInfo) == e_success)
					{
						printf("Encoded secret file extension size successfully\n") ;
						if(encode_secret_file_extn(encInfo ->extn_secret_file,encInfo)== e_success)
						{
						 	printf("Encoded secret file extension successfully\n") ;
							if(encode_secret_file_size(encInfo -> size_secret_file,encInfo) == e_success)
							{
								printf("Encoded secret file size successfully\n") ;
								if(encode_secret_file_data(encInfo) == e_success)
								{
									printf("Encoded secret file data successfully\n") ;
								   if(copy_remaining_img_data(encInfo -> fptr_src_image,encInfo -> fptr_stego_image)== e_success)
								   {
									   printf("Copied remaining data successfully\n") ;

								   }	   
								   else
								   {
									   printf("Failed:( Copying remaining data\n") ;
									   return e_failure ;
								   }
								}
								else
								{
									printf("Failed:( Encoding secret file data\n") ;
								}
							}
							else
							{
								printf("Failed:(Encoding secret file size\n") ;
								return e_failure ;
							}


						}
						else
						{
						printf("Failed:( encoding secret file extension\n") ;
						}

					}
						else
						{
							printf("Failed:(encode the extn size\n");
                             return e_failure ;
						}
					}
			
			
			else
			{
				printf("Failed:(Encoding Magic string\n") ;
				return e_failure ;
			}
		}
		
			else
			{
				printf("Failed:(Copying the header file\n") ;
				return e_failure ;
			}
			
		}
		else
		{
			printf("Failed:( Encoding is not possible\n") ;
			return e_failure ;

		}
	}
	else
	{
		printf("Failed:( Opening files\n") ;
		return e_failure ;

	}
	return e_success ;
}
