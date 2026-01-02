#include "types.h"    //Header files
#include "decode.h"
#include "common.h"

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo) // Function to read and validate decode arguments
{
    if (strstr(argv[2], ".bmp") != NULL) // Check if the second argument is a valid BMP file
    {
        if (checks_extension(argv[2], ".bmp") == e_success) // Check the extension is ".bmp"
        {
            decInfo->stego_image_fname = argv[2];
        }
        else
        {
            return e_failure;
        }
    }
    else
    {
        printf("Error : The input file name must contain .bmp extenstion.\n");
        fprintf(stderr, "For Decoding : ./a.out -d <.bmp file> [output file]\n");
        return e_failure;
    }

    if (argv[3] == NULL) // Check if the third argument is NULL or not a valid file name
    {
        printf("INFO : Selected default file name : Output\n");
        strcpy(decInfo->output_filename, "Output"); // Set a default output file name
    }
    else
    {
        if (argv[3][0] == '.')
        {
            printf("INFO : Selected default file name : Output\n");
            strcpy(decInfo->output_filename, "Output");
            return e_success;
        }
        else
        {
            strcpy(decInfo->output_filename, argv[3]); // Set the output file name from the argument
        }
    }

    return e_success;
}

Status stego_image_open_file(DecodeInfo *decInfo) // Function to open the stego image file for reading
{
    if ((decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r")) == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR : Unable to open file %s\n", decInfo->stego_image_fname);
        return e_failure;
    }
    printf("INFO : Opened %s File\n", decInfo->stego_image_fname);
    return e_success;
}

Status output_image_open_file(DecodeInfo *decInfo) // Function to open the output file for writing
{
    if ((decInfo->fptr_output = fopen(decInfo->output_filename, "w")) == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR : Unable to open file %s\n", decInfo->output_filename);
        return e_failure;
    }
    printf("INFO : Opened %s File\n", decInfo->output_filename);
    return e_success;
}

Status do_decoding(DecodeInfo *decInfo) // Function to perform the decoding process
{
    printf("INFO :  ## Decoding Procedure Started ##\n");
    printf("INFO : Opening %s File\n", decInfo->stego_image_fname);
    if (stego_image_open_file(decInfo) == e_failure) // call function to open the stego image file
    {
        printf("ERROR : Failed to open input file\n");
        return e_failure;
    }
    printf("INFO : Done\n");

    printf("INFO : Skipping BMP Header\n");
    if (skip_bmp_header(decInfo->fptr_stego_image) == e_failure) // call function to skip the BMP header
    {
        return e_failure;
    }
    printf("INFO : Done\n");

    printf("INFO : Decoding Magic String Signature\n");
    if (decode_magic_string(MAGIC_STRING, decInfo) == e_failure) // call function to decode the magic string
    {
        return e_failure;
    }
    printf("INFO : Done\n");

    printf("INFO : Decoding Secret File Size And Extension\n");
    if (decode_secret_file_extn_size(decInfo) == e_failure) // call function to decode the secret file extension size
    {
        return e_failure;
    }
    else
    {
        if (decode_secret_file_extn(decInfo) == e_failure) // call function to decode the secret file extension
        {
            return e_failure;
        }
    }
    printf("INFO : Done\n");

    printf("INFO : Opening %s File\n", decInfo->output_filename);
    if (output_image_open_file(decInfo) == e_failure) // call function to open the output file
    {
        printf("ERROR : Failed to open output file\n");
        return e_failure;
    }
    printf("INFO : Done\n");

    printf("INFO : Decoding Secret File Size And Data\n");
    if (decode_secret_file_size(decInfo) == e_failure) // call function to decode the secret file size
    {
        return e_failure;
    }
    else
    {
        if (decode_secret_file_data(decInfo) == e_failure) // call function to decode the secret file data
        {
            return e_failure;
        }
    }
    printf("INFO : Done\n");

    printf("INFO : ##  DECODING DONE SUCESSFULLY  ##\n");
    return e_success;
}

Status skip_bmp_header(FILE *fptr_stego_image) // Function to skip the BMP header in the stego image file
{
    if (fseek(fptr_stego_image, 54, SEEK_SET) != 0)
    {
        perror("fseek");
        fprintf(stderr, "ERROR : Failed to skip BMP header\n");

        return e_failure;
    }
    return e_success;
}

Status decode_magic_string(char *magic_string, DecodeInfo *decInfo) // Function to decode the magic string from the stego image file
{
    char str[32]; // create a buffer to store user input
    printf("\n");
    printf("Enter the magic string : ");
    scanf(" %[^\n]", str);
    printf("\n");

    char buff[32];
    for (int i = 0; i < strlen(magic_string); i++) // loop runs for the length of the magic string
    {
        if (decode_byte_from_lsb(&buff[i], decInfo->fptr_stego_image) == e_failure) // call function to decode each byte of the magic string
        {
            printf("ERROR : Failed to decode magic string\n");
            return e_failure;
        }
    }
    buff[strlen(magic_string)] = '\0'; // add null terminator to the end of the decoded string

    if (strcmp(buff, str) != 0) // compare the decoded magic string with the user input
    {
        printf("ERROR : Magic string does not match\n");
        return e_failure;
    }
    else
    {
        printf("INFO : Magic String Verified\n");
        printf("\n");
        return e_success;
    }
}

Status decode_secret_file_extn_size(DecodeInfo *decInfo) // Function to decode the size of the secret file extension
{
    if (decode_size_from_lsb(&decInfo->ext_size, decInfo->fptr_stego_image) == e_failure) // call function to decode the size of the secret file extension
    {
        printf("ERROR : Failed to decode size of extenstion\n");
        return e_failure;
    }
    printf("INFO : Decoded Output File Extension Size\n");
    return e_success;
}

Status decode_secret_file_extn(DecodeInfo *decInfo) // Function to decode the secret file extension from the stego image file
{
    char buff[decInfo->ext_size + 1];

    for (int i = 0; i < decInfo->ext_size; i++) // loop runs for the size of the secret file extension
    {
        if (decode_byte_from_lsb(&buff[i], decInfo->fptr_stego_image) == e_failure) // call function to decode each byte of the secret file extension
        {
            printf("ERROR : Failed to decode secret file extension\n");
            return e_failure;
        }
    }
    buff[decInfo->ext_size] = '\0'; // add null terminator to the end of the decoded string
    printf("INFO : Decoded Output File Extension is %s\n", buff);

    if (strstr(decInfo->output_filename, buff) == NULL) // check if the output file name already contains the extension
    {
        char *ext;
        if (ext = strchr(decInfo->output_filename, '.')) // find the position of the '.'
        {
            strcpy(ext, buff); // replace the extension with the decoded one
        }
        else
        {
            sprintf(decInfo->output_filename, "%s%s", decInfo->output_filename, buff); // append the decoded extension to the output file name
        }
    }
    else
    {
        char *ext = strchr(decInfo->output_filename, '.'); // find the position of the '.'
        strcpy(ext, buff);                                 // update name with decoded extenstion
    }
    printf("INFO : Output File Name is %s\n", decInfo->output_filename);
    return e_success;
}

Status decode_byte_from_lsb(char *data, FILE *image_buffer) // decode byte from lsb function
{
    char buff[8];
    if (fread(buff, sizeof(buff), 1, image_buffer) != 1) // read 8 bytes from the image buffer
    {
        printf("ERROR : Failed to read byte from image buffer\n");
        return e_failure;
    }

    char byte = 0; // initialize byte to 0
    for (int i = 0; i < 8; i++)
    {
        char get = buff[i] & 1;       // get the least significant bit of the byte
        byte = byte | get << (7 - i); // move the bit to its correct position and combine it with the byte
    }
    *data = byte; // assign the decoded byte to the data pointer
    return e_success;
}

Status decode_size_from_lsb(int *data, FILE *image_buffer) // decode size from lsb function
{
    char buff[32];

    if (fread(buff, sizeof(buff), 1, image_buffer) != 1) // read 32 bytes from the image buffer
    {
        printf("ERROR : Failed to read byte from image buffer\n");
        return e_failure;
    }

    int byte = 0;
    for (int i = 0; i < 32; i++)
    {
        int get = buff[i] & 1;         // get the least significant bit of the byte
        byte = byte | get << (31 - i); // move the bit to its correct position and combine it with the byte
    }
    *data = byte; // assign the decoded byte to the data pointer
    return e_success;
}

Status decode_secret_file_size(DecodeInfo *decInfo) // Function to decode the size of the secret file
{
    int val;
    if (decode_size_from_lsb(&val, decInfo->fptr_stego_image) == e_failure) // calll fuction to decode size of the secret file
    {
        printf("ERROR : Failed to decode size of secret file\n");
        return e_failure;
    }
    decInfo->size_secret_file = val; // assign the decoded size to the DecodeInfo structure
    printf("INFO : Decoded Secret File Size\n");
    return e_success;
}

Status decode_secret_file_data(DecodeInfo *decInfo) // Function to decode the secret file data from the stego image file
{
    char buff;

    for (int i = 0; i < decInfo->size_secret_file; i++) // loop runs for the size of the secret file
    {
        if (decode_byte_from_lsb(&buff, decInfo->fptr_stego_image) == e_failure) // call function to decode each byte of the secret file data
        {
            printf("ERROR : Failed to decode secret file data\n");
            return e_failure;
        }

        if (fwrite(&buff, sizeof(buff), 1, decInfo->fptr_output) != 1) // write the decoded byte to the output file
        {
            printf("ERROR : Failed to write secret file data to output file\n");
            return e_failure;
        }
    }
    printf("INFO : Decoded Secret File Data\n");
    return e_success;
}

Status checks_extension(char *name, char *extension) // function to validate extenstion
{
    char str[20];
    char *ext = strchr(name, '.');   // Find the position of the '.'
    strcpy(str, ext);                // Copy the extension to a new string
    if (strcmp(str, extension) == 0) // Check if the extension is same or not
    {
        return e_success;
    }
    else
    {
        fprintf(stderr, "The file name %s is not vaild.\n", name);
        fprintf(stderr, "For Decoding : ./a.out -d <.bmp file> [output file]\n");
        return e_failure;
    }
}
