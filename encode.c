#include "encode.h"           //Header files
#include "types.h"
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
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

    printf("INFO :Opened %s\n", encInfo->src_image_fname);

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

        return e_failure;
    }

    printf("INFO :Opened %s\n", encInfo->secret_fname);

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

        return e_failure;
    }

    printf("INFO :Opened %s\n", encInfo->stego_image_fname);
    // No failure return e_success
    return e_success;
}

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    /*first cond -> check the .bmp is present or not(argv[2])
      if true, store the file in src_image_fname variable
      if false, return e_failure
    */
    if (strstr(argv[2], ".bmp") != NULL)
    {
        if (check_extension(argv[2], ".bmp") == e_success) // Check if the extension is ".bmp"
        {
            encInfo->src_image_fname = argv[2];
        }
        else
        {
            return e_failure;
        }
    }
    else
    {
        fprintf(stderr, "Error : The source file name must contain .bmp extenstion.\n");
        fprintf(stderr, "For Encoding : ./a.out -e <.bmp file> <.txt file> [output file]\n");
        return e_failure;
    }

    /* second cond -> check the extension based on your idea (like : ".txt",".bmp",".sh")
       true, store the file in secret_fname variable
       false, return e_failrue
    */

    char *extenstion[3] = {".txt", ".c", ".sh"};
    int flag = 0;
    for (int i = 0; i < 3; i++)
    {
        if (strstr(argv[3], extenstion[i]) != NULL)
        {
            if (check_extension(argv[3], extenstion[i]) == e_success) // Checking extension is valid or not
            {
                encInfo->secret_fname = argv[3];
                flag++;
                break;
            }
            else
            {
                return e_failure;
            }
        }
    }
    if (!flag)
    {
        fprintf(stderr, "Error : The secret file name must contain any text file extenstion.\n");
        fprintf(stderr, "For Encoding : ./a.out -e <.bmp file> <.txt file> [output file]\n");
        return e_failure;
    }

    /*3rd cond : check the argv[4] is NULL or not
       true ,check the .bmp present or not(argv[4])
                         true,store the file in stego_image_fname variable
                         false, return e_failure
        false, store the default file name in stego_fname variable
                 stego_image_fname = "stego.bmp"
        */
    // return e_sucess
    if (argv[4] != NULL)
    {
        if (argv[4][0] == '.')
        {
            printf("INFO : Output file always need a name with .bmp extension\n");
            printf("INFO : Selected default file name : Stego.bmp\n");
            encInfo->stego_image_fname = "Stego.bmp";
            return e_success;
        }
        else if (strstr(argv[4], ".bmp") != NULL)
        {
            if (check_extension(argv[4], ".bmp") == e_success) // Check if the extension is ".bmp"
            {
                encInfo->stego_image_fname = argv[4];
                return e_success;
            }
            else
            {
                return e_failure;
            }
        }
        else
        {
            fprintf(stderr, "Error : The output file name must contain .bmp extenstion.\n");
            fprintf(stderr, "For Encoding : ./a.out -e <.bmp file> <.txt file> [output file]\n");
            return e_failure;
        }
    }
    else
    {
        printf("INFO : Output file always need a name with .bmp extension\n");
        printf("INFO : Selected default file name : Stego.bmp\n");
        encInfo->stego_image_fname = "Stego.bmp";
        return e_success;
    }
}

Status do_encoding(EncodeInfo *encInfo) // Encoding function
{
    printf("INFO :  ## Encoding Procedure Started ##\n");
    printf("INFO :Opening required files\n");
    if (open_files(encInfo) == e_failure) // call function to opening files
    {
        printf("INFO : Open File Failed\n");
        return e_failure;
    }
    printf("INFO : Done\n");

    printf("INFO : Checking for %s capacity to handle %s\n", encInfo->src_image_fname, encInfo->secret_fname);
    if (check_capacity(encInfo) == e_failure) // call function to check capacity
    {
        return e_failure;
    }
    printf("INFO : Done, %s file have the capacity to handle %s file data\n", encInfo->src_image_fname, encInfo->secret_fname);

    printf("INFO : Copying Image Header\n");
    if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure) // call function to copy header file data
    {
        return e_failure;
    }
    printf("INFO : Done\n");

    printf("INFO : Encoding Magic String Signature\n");
    if (encode_magic_string(MAGIC_STRING, encInfo) == e_failure) // call function to encode magic string data
    {
        return e_failure;
    }
    printf("INFO : Done\n");

    printf("INFO : Encoding %s File Size And Extension\n", encInfo->secret_fname);
    char *extenstion[3] = {".txt", ".c", ".sh"}; // character array of possible file extensions
    char *ext_name;
    for (int i = 0; i < 3; i++)
    {
        if (strstr(encInfo->secret_fname, extenstion[i]) != NULL) // check if the secret file extension is present in the character array
        {
            ext_name = extenstion[i];
            break;
        }
    }

    if (encode_secret_file_extn(ext_name, encInfo) == e_failure) // call function to encode secret file extension
    {
        return e_failure;
    }
    printf("INFO : Done\n");

    printf("INFO : Encoding %s File Size\n", encInfo->secret_fname);
    if (encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_failure) // call function to encode secret file size
    {
        return e_failure;
    }
    printf("INFO : Done\n");

    printf("INFO : Encoding %s File Data\n", encInfo->secret_fname);
    if (encode_secret_file_data(encInfo) == e_failure) // call function to encode secret file data
    {
        return e_failure;
    }
    printf("INFO : Done\n");

    printf("INFO : Copying Remaining Source Image Data\n");
    if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure) // call function to copy remaining source image data
    {
        return e_failure;
    }
    printf("INFO : Done\n");

    printf("INFO : ##  ENCODING DONE SUCESSFULLY  ##\n");
}

Status check_capacity(EncodeInfo *encInfo) // check capacity function
{
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image); // call function to get image size
    printf("Source image capacity : %d bytes\n", encInfo->image_capacity);
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret); // call function to get secret file size
    printf("Size of secret file : %ld\n", encInfo->size_secret_file);

    int val = 54 + ((2 + 4 + 4 + 4 + encInfo->size_secret_file) * 8);
    if (encInfo->image_capacity < val) // check the source image capacity(bytes) is greater than required bytes
    {
        printf("ERROR : Not enough space to encode data\n");
        return e_failure;
    }
}

uint get_file_size(FILE *fptr) // get file size function
{
    fseek(fptr, 0, SEEK_END);
    return ftell(fptr);
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    fseek(fptr_src_image, 0, SEEK_SET);  // set both pointers to begining of the file
    fseek(fptr_dest_image, 0, SEEK_SET); // we can use rewind also

    char buffer[54];                        // create a buffer to store header file
    fread(buffer, 54, 1, fptr_src_image);   // fread is used to read hexa values from src_image to buffer
    fwrite(buffer, 54, 1, fptr_dest_image); // fwrite is used to read hexa from buffer to dest_image(output file)

    if (ftell(fptr_src_image) != ftell(fptr_dest_image)) // Error handling
    {
        printf("Error : Failed to write Header data of 54 bytes to stego(output) file.\n");
        return e_failure;
    }
}

Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    for (int i = 0; i < strlen(magic_string); i++) // encoding each character of magic string
    {
        char buff[8];
        fread(buff, 8, 1, encInfo->fptr_src_image); // read 8 bytes from source image file

        if (encode_byte_to_lsb(magic_string[i], buff) == e_failure) // call function to encode byte to lsb
        {
            printf("Error : Encoding Magic string failed\n");
            return e_failure;
        }

        fwrite(buff, 8, 1, encInfo->fptr_stego_image); // write 8 bytes to stego image file

        if (ftell(encInfo->fptr_src_image) != ftell(encInfo->fptr_stego_image))
        {
            printf("Error : Encoding Magic string failed\n");
            return e_failure;
        }
    }
}

Status encode_byte_to_lsb(char data, char *image_buffer) // encode byte to lsb function
{
    if (image_buffer == NULL)
    {
        printf("Error : image_buffer is NULL in encode_byte_to_lsb.\n");
        return e_failure;
    }

    for (int i = 0; i < 8; i++) // encoding each bit of data
    {
        image_buffer[i] = image_buffer[i] & ~1;    // clear the least significant bit
        unsigned char get = data & (1 << (7 - i)); // get the ith bit of data
        get = get >> (7 - i);                      // shift the bit to the rightmost position
        image_buffer[i] = image_buffer[i] | get;   // set the least significant bit to the ith bit of data
    }
    return e_success;
}

Status encode_size_to_lsb(int data, char *image_buffer) // encode size to lsb function
{
    if (image_buffer == NULL) // Error handling
    {
        printf("Error : image_buffer is NULL in encode_size_to_lsb.\n");
        return e_failure;
    }

    for (int i = 0; i < 32; i++) // encoding each bit of size
    {
        image_buffer[i] = image_buffer[i] & ~1;     // clear the least significant bit
        unsigned char get = data & (1 << (31 - i)); // get the ith bit of data
        get = get >> (31 - i);                      // shift the bit to the rightmost position
        image_buffer[i] = image_buffer[i] | get;    // set the least significant bit to the ith bit of data
    }
    return e_success;
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo) // encode secret file extension function
{
    int size = strlen(file_extn);

    char buff[32];                               // create a buffer of 32 bytes
    fread(buff, 32, 1, encInfo->fptr_src_image); // read 32 bytes from source image file

    if (encode_size_to_lsb(size, buff) == e_failure) // call function to encode secret file extension size
    {
        printf("Error : Failed to encode secret file extension size\n");
        return e_failure;
    }

    fwrite(buff, 32, 1, encInfo->fptr_stego_image); // write 32 bytes to stego image file

    if (ftell(encInfo->fptr_src_image) != ftell(encInfo->fptr_stego_image))
    {
        printf("Error : Failed to encode secret file extension size\n");
        return e_failure;
    }

    for (int i = 0; i < size; i++) // encoding each character of secret file extension
    {
        char buff[8];

        fread(buff, 8, 1, encInfo->fptr_src_image);              // read 8 bytes from source image file
        if (encode_byte_to_lsb(file_extn[i], buff) == e_failure) // call function to encode byte to lsb
        {
            printf("Error : Failed to encode secret file extension\n");
            return e_failure;
        }
        fwrite(buff, 8, 1, encInfo->fptr_stego_image); // write 8 bytes to stego image file

        if (ftell(encInfo->fptr_src_image) != ftell(encInfo->fptr_stego_image))
        {
            printf("Error : Failed to encode secret file extension\n");
            return e_failure;
        }
    }
}

Status encode_secret_file_size(int file_size, EncodeInfo *encInfo) // encode secret file size function
{
    char buff[32];
    fread(buff, 32, 1, encInfo->fptr_src_image); // read 32 bytes from source image file

    if (encode_size_to_lsb(file_size, buff) == e_failure) // call function to encode secret file size
    {
        printf("Error : Failed to encode secret file size.\n");
        return e_failure;
    }

    fwrite(buff, 32, 1, encInfo->fptr_stego_image); // write 32 bytes to stego image file

    if (ftell(encInfo->fptr_src_image) != ftell(encInfo->fptr_stego_image))
    {
        printf("Error : Failed to encode secret file size.\n");
        return e_failure;
    }
}

Status encode_secret_file_data(EncodeInfo *encInfo) // encode secret file data function
{
    rewind(encInfo->fptr_secret);                                    // rewind the secret file pointer to the beginning
    char buff[encInfo->size_secret_file];                            // create a buffer of size secret file size
    fread(buff, encInfo->size_secret_file, 1, encInfo->fptr_secret); // read secret file data into buffer

    if (encode_data_to_image(buff, encInfo->size_secret_file, encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure) // call function to encode data to image
    {
        printf("Error : Failed to encode secret file data.\n");
        return e_failure;
    }
}

Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image) // encode data to image function
{
    for (int i = 0; i < size; i++) // encoding each byte of secret file data
    {
        char buff[8];

        fread(buff, 8, 1, fptr_src_image);                  // read 8 bytes from source image file
        if (encode_byte_to_lsb(data[i], buff) == e_failure) // call function to encode byte to lsb
        {
            return e_failure;
        }
        fwrite(buff, 8, 1, fptr_stego_image); // write 8 bytes to stego image file

        if (ftell(fptr_src_image) != ftell(fptr_stego_image))
        {
            return e_failure;
        }
    }
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch;
    int val = 0;

    while ((val = fread(&ch, 1, 1, fptr_src)) > 0) // read remaining bytes from source image file
    {
        if (fwrite(&ch, 1, 1, fptr_dest) != val) // write remaining bytes to stego image file
        {
            printf("Error : Failed to encode remaining source file data.\n");
            return e_failure;
        }
    }
    return e_success;
}

Status check_extension(char *name, char *extension) // function to validate extenstion
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
        fprintf(stderr, "For Encoding : ./a.out -e <.bmp file> <.txt file> [output file]\n");
        return e_failure;
    }
}

