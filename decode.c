#include "decode.h"
#include "types.h"
#include "common.h"

Status open_image(DecodeInfo *decInfo)
{
    printf("INFO: Opening required files\n");
    // Stego Image file
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");
    // Do Error checking
    if (decInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);
        return e_failure;
    }

    // moving to 54th byte
    fseek(decInfo->fptr_stego_image, 54, SEEK_SET);

    // No failure return e_success
    return e_success;
}

/* Read and validate decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    // validating .bmp file
    if (strstr(argv[2], ".bmp"))
        decInfo->stego_image_fname = argv[2];
    else
    {
        fprintf(stderr, "ERROR: %s is not a .bmp file\n", argv[2]);
        return e_failure;
    }

    if (argv[3] == NULL)
    {
        strcpy(decInfo->secret_fname, "Decoded");
        decInfo->file_name_passed = 0;
    }
    else
        strcpy(decInfo->secret_fname, argv[3]);

    return e_success;
}

/* function to Perform the decoding */
Status do_decoding(DecodeInfo *decInfo)
{
    /* opening the files */
    if (open_image(decInfo) == e_success)
    {
        printf("INFO: Opened %s\n", decInfo->stego_image_fname);
        // decoding the magic string
        if (decode_magic_string(MAGIC_STRING, decInfo) == e_success)
        {
            printf("INFO: Magic string Decoded successfully\n");

            // decoding the extension size
            if (decode_size(sizeof(int), decInfo) == e_success)
            {
                decInfo->ext_size = decInfo->_32_bit_size;
                printf("INFO: Decoded extension size: %d\n", decInfo->ext_size);

                // decoding the file extension
                if (decode_secret_file_extn(decInfo) == e_success)
                {
                    printf("INFO: Decoding Output File Extension\n");
                    printf("INFO: Decoded extension: %s\n", decInfo->extn_secret_file);

                    // Opening output file
                    if (open_secret(decInfo) == e_success)
                    {
                        printf("INFO: Opened all required files\n");

                        // decoding the secret file size
                        if (decode_file_size(sizeof(int), decInfo) == e_success)
                        {
                            decInfo->file_size = decInfo->_32_bit_size;
                            printf("INFO: Decoded secret file size: %d\n", decInfo->file_size);

                            // decoding the secret file data
                            if (decode_secret_file_data(decInfo) == e_success)
                            {
                                printf("Hurray :) ----DECODING SUCCESS----\n");
                                printf("__DONE__\n");
                                return e_success;
                            }
                            else
                            {
                                printf("*ERROR* Secret data decoding failed\n");
                                return e_failure;
                            }
                        }
                        else
                        {
                            printf("*ERROR* File size decoding failed\n");
                            return e_failure;
                        }
                    }
                }
                else
                {
                    printf("*ERROR* Extension decoding failed\n");
                    return e_failure;
                }
            }
            else
            {
                printf("ERROR: File extension size decoding failed\n");
                return e_failure;
            }
        }
        else
        {
            printf("ERROR: Magic string decoding failure\n");
            return e_failure;
        }
    }
}

/* Decoding Magic String */
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo)
{
    /* Decode function, which does the real decoding */
    printf("Decoding Magic String Signature\n");
    decode_data_from_image(strlen(MAGIC_STRING), decInfo->magic_string, decInfo);
    decInfo->magic_string[strlen(MAGIC_STRING)] = '\0';
    if (strcmp(decInfo->magic_string, MAGIC_STRING) == 0)
        return e_success;
    else
        return e_failure;
}

/* Decode function, which does the real decoding */
Status decode_data_from_image(int size, char *decoded_data, DecodeInfo *decInfo)
{
    for (int i = 0; i < size; i++)
    {
        // storing 8 bytes for source image
        fread(decInfo->image_data, 8, 1, decInfo->fptr_stego_image);

        /* decode an LSB of 8 bytes of image data */
        decode_byte_from_lsb(decInfo->image_data, decInfo);
        decoded_data[i] = decInfo->secret_data;
    }
    return e_success;
}

/* decode a byte into a character */
Status decode_byte_from_lsb(char *image_buffer, DecodeInfo *decInfo)
{
    char data = 0;
    // clearing lsb bit of each byte and storing each bit to the data
    for (int i = 0; i < 8; i++)
    {
        data = (image_buffer[i] & 0X1) | (((data) << 1));
    }
    decInfo->secret_data = data;
    return e_success;
}

// Decode file extension size
Status decode_size(int size, DecodeInfo *decInfo)
{
    // getting 32 bytes from source image
    fread(decInfo->image_data, size * 8, 1, decInfo->fptr_stego_image);

    // decoding integer value of size into 32 bytes
    if(decode_size_from_lsb(size * 8, decInfo->image_data, decInfo)==e_success)
    {
        decInfo->ext_size=decInfo->_32_bit_size;
        printf("INFO:Decoded extension size:%d\n",decInfo->ext_size);
        return e_success;
    }
    else
    {
        printf("**ERROR failed to decode size\n");
        return e_failure;
    }
}

// decoding extension size from LSB of image
Status decode_size_from_lsb(int n_bytes, char *image_buffer, DecodeInfo *decInfo)
{
    decInfo->_32_bit_size = 0;

    // decoding the lsb bits from each byte
    for (int i = 0; i < n_bytes; i++){

        decInfo->_32_bit_size = (image_buffer[i] & 0X01) | (decInfo->_32_bit_size << 1);
    }
    printf("Debug:decoded size from lsb:%d\n",decinfo->_32_bit_size);
    return e_success;
}

// decoding secret file extension
Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    printf("INFO: Decoding Output File Extension\n");

    // decoding data from image
    if (decode_data_from_image(decInfo->ext_size, decInfo->extn_secret_file, decInfo) == e_success)
    {
        decInfo->extn_secret_file[decInfo->ext_size] = '\0';
        return e_success;
    }
    else
        return e_failure;
}

// Decode secret file size
Status decode_file_size(int size, DecodeInfo *decInfo)
{
    // getting 32 bytes from source image
    fread(decInfo->image_data, size * 8, 1, decInfo->fptr_stego_image);

    // decoding the size
    if (decode_size_from_lsb(size * 8, decInfo->image_data, decInfo) == e_success)
    {
        decInfo->file_size=decInfo->_32_bit_size;
        printf("INFO:decoded secret file size:%d\n",decInfo->file_size);
        return e_success;
    }    
    else
    {
        printf("Error failed to decode file size\n");
        return e_failure;
    }
}

Status open_secret(DecodeInfo *decInfo)
{
    if (!decInfo->file_name_passed)
        printf("File name not mentioned. Creating %s as default\n", decInfo->secret_fname);
    if (!(strstr(decInfo->secret_fname, ".txt")))
        strcat(decInfo->secret_fname, decInfo->extn_secret_file);
    decInfo->fptr_secret = fopen(decInfo->secret_fname, "w");
    return e_success;
}

// Decoding secret file data
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    for (int i = 0; i < decInfo->file_size; i++)
    {
        // reading the image data
        fread(decInfo->image_data, 8, 1, decInfo->fptr_stego_image);

        // decoding the lsb bits
        decode_byte_from_lsb(decInfo->image_data, decInfo);

        // writing the encoded data into stego image
        fwrite(&(decInfo->secret_data), 1, 1, decInfo->fptr_secret);
    }
    return e_success;
}