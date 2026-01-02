/*                                      DOCUMENTATION
       NAME    : ANURAJ R
       ROLL NO : 25005_010
       BATCH   : 25005B
       PROJECT : STEGANOGRAPHY

*/

#include "encode.h"    //Header files
#include "decode.h"
#include "types.h"

int main(int argc, char *argv[])
{
   if (argc < 2)
   {
      fprintf(stderr, "For Encoding : ./a.out -e <.bmp file> <.txt file> [output file]\n");
      fprintf(stderr, "For Decoding : ./a.out -d <.bmp file> [output file]\n");
      return e_failure;
   }

   EncodeInfo encode; // created a structue variable to store all encoding related data

   DecodeInfo decode; // created a structure variable to store all decoding related data

   switch (check_operation_type(argv)) // check user selected encoding or decoding
   {

   case e_encode:
      if (argc == 4 || argc == 5) // check the number of arguments passed to program is 5 or 4
      {
         printf("Selected Encoding\n");
         if (read_and_validate_encode_args(argv, &encode) == e_success)
         {
            /*perform the encoding */
            do_encoding(&encode);
         }
         else
            printf("Error : Read and validate failed\n");
      }
      else
      {
         fprintf(stderr, "For Encoding : ./a.out -e <.bmp file> <.txt file> [output file]\n");
         printf("Error : Invalid number of arguments for encoding\n"); // Handle wrong number of arguments
      }
      break;

   case e_decode:
      if (argc == 3 || argc == 4) // check the number of arguments passed to program is 3 or 4
      {
         printf("Selected Decoding\n");
         if (read_and_validate_decode_args(argv, &decode) == e_success)
         {
            /* perform the decoding*/
            do_decoding(&decode);
         }
         else
            printf("Error : Read and validate failed\n");
      }
      else
      {
         fprintf(stderr, "For Decoding : ./a.out -d <.bmp file> [output file]\n");
         printf("Error : Invalid number of arguments for decoding\n"); // Handle wrong number of arguments
      }
      break;

   default:
      printf("Error :Invalid Operation. Please use -e for encoding and -d for decoding\n"); // Handle unsupported operations
   }

   return 0;
}

OperationType check_operation_type(char *argv[]) // Function to find user selected encoding or decoding
{
   if(!strcmp(argv[1], "-e"))
   {
      return e_encode;
   }

   if(!strcmp(argv[1], "-d"))
   {
      return e_decode;
   }

   return e_unsupported;
}
