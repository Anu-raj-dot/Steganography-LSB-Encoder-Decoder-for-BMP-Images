#ifndef DECODE_H
#define DECODE_H

/*
 * Structure to store information required for
 * decoding secret file data from Stego Image file
 * and store that decoded data in a output file
 */
#include "types.h"

typedef struct DecodeInfo
{
    /*Stego Image info*/
    char *stego_image_fname;
    FILE *fptr_stego_image;
    char imag_buffer[8];

    /*Output File info*/
    char output_filename[100];
    FILE *fptr_output;
    int ext_size;
    char extn_secret_file[4];
    long size_secret_file;

} DecodeInfo;

/*Decoding function prototype*/

/* Read and validate Decode args from argv*/
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the decoding*/
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files*/
Status stego_image_open_file(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files*/
Status output_image_open_file(DecodeInfo *decInfo);

/* Skip bmp image header*/
Status skip_bmp_header(FILE *fptr_stego_image);

/* Decode Magic string*/
Status decode_magic_string(char *magic_string, DecodeInfo *decInfo);

/* Decode Secret file extenstion size*/
Status decode_secret_file_extn_size(DecodeInfo *decInfo);

/* Decode Secret file extenstion*/
Status decode_secret_file_extn(DecodeInfo *decInfo);

/* Decode Secret file size*/
Status decode_secret_file_size(DecodeInfo *decInfo);

/* Decode secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Decode data from the lsb of stego data array */
Status decode_byte_from_lsb(char *data, FILE *image_buffer);

/* Decode size from the lsb of stego data array */
Status decode_size_from_lsb(int *data, FILE *image_buffer);

/* Check extension is in valid format or not*/
Status checks_extension(char *name, char *extension);

#endif

