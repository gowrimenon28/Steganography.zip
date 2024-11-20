#ifndef DECODE_H
#define DECODE_H
#include <stdio.h>
#include <string.h>
#include "types.h" // Contains user defined types

/* 
 * Structure to store information required for
 * decoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4


typedef struct _DecodeInfo
{

    /* Secret File Info */

    int ext_size;
    int file_size;
    char secret_fname[20];
    FILE *fptr_secret;
    char extn_secret_file[MAX_FILE_SUFFIX];
    char secret_data;


    /* Stego Image Info */ 

    char *stego_image_fname;
    FILE *fptr_stego_image;
    char magic_string[2];
    
    /* Buffer*/

    char image_data[40];
    int _32_bit_size;
    int file_name_passed;

}DecodeInfo;

/* Decoding function prototype */

/* Check operation type */

OperationType check_operation_type(char *argv[]);


/* Read and validate Decode args from argv */

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);


/* Perform the decoding */

Status do_decoding(DecodeInfo *encInfo);


/* function to Get File pointers for i/p and o/p files */

Status open_image(DecodeInfo *decInfo);
Status open_secret(DecodeInfo *decInfo);

/* function decode Magic String */

Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo);


/* function Decode file extention size */

Status decode_size(int size, DecodeInfo *decInfo);


 //function Decode a extention size into LSB of image data array

Status decode_size_from_lsb(int size, char *image_buffer,DecodeInfo *decInfo);


//function  Decode secret file extenstion 

Status decode_secret_file_extn(DecodeInfo *decInfo);


// function  to Decode secret file size 

Status decode_file_size(int size, DecodeInfo *decInfo);


// function Decode secret file data

Status decode_secret_file_data(DecodeInfo *decInfo);


//Decode function, which does the real decoding 

Status decode_data_from_image(int size,char *decoded_data, DecodeInfo *decInfo);


// function  Decode a byte into LSB of image data array 

Status decode_byte_from_lsb(char *image_buffer,DecodeInfo *decInfo);


#endif
