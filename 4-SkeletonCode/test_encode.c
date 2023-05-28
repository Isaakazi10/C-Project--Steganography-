/*
Name        : ISAA KAZI
Date        : 14/03/2023
*/
#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

int main(int argc, char *argv[])
{
    // Calling Check_operation Function to is if the argument is -e(encoding) or -d(decoding)
    if (check_operation_type(argv) == e_encode)
    {
        // For encoding the argument count has to within range of 4 and 5 else print error
        if (argc < 4 || argc > 5)
        {
            printf("\n[-- ERROR --: ( Argument count should be within range of 4 and 5 )--]\n");
            return 0;
        }

        printf("[---------( SELECTED ENCODING )---------]\n");

        // Declaration of EncodeInfo varible.
        EncodeInfo encInfo;

        if (read_and_validate_encode_args(argv, &encInfo) == e_success)
        {
            printf("\n[---( 1 Read and validate -> SUCCESSFUL )---]");

            if (do_encoding(&encInfo) == e_success)
            {
                printf("\n\n[---------( ENCODING IS SUCCESSFUL )---------]\n");
                return e_success;
            }
            else
            {
                printf("\n\n[-- ERROR --: ( Encoding is FAILURE )--]\n");
                return e_failure;
            }
        }
        else
        {
            printf("\n\n[-- ERROR --: ( Read and validate is FAILURE )--]\n");
            printf("\n\n[---------( ENCODING FAILED )---------]\n");
            return e_failure;
        }
    }
    else if (check_operation_type(argv) == e_decode)
    {
        // For Decoding the argument count has to within range of 3 and 4 else error.
        if (argc < 3 || argc > 4)
        {
            printf("\n[-- ERROR --: ( Argument count should be withi range of 3 and 4 )--]\n");
            return 0;
        }

        printf("[---------( SELECTED DECODING )---------]\n");

        // Declaration of DecodeInfo varible.
        DecodeInfo decInfo;

        if (read_and_validate_decode_args(argv, &decInfo) == e_success)
        {
            printf("\n[---( 1 Read and validate -> SUCCESSFUL )---]");

            if (do_decoding(&decInfo) == e_success)
            {
                printf("\n\n[---------( DECODING IS SUCCESSFUL )---------]\n");
                return e_success;
            }
            else
            {
                printf("\n\n[-- ERROR --: ( Decoding is FAILURE )--]\n");
                return e_failure;
            }
        }
        else
        {
            printf("\n\n[-- ERROR --: ( Read and validate is FAILURE )--]\n");
            printf("\n\n[---------( DECODING FAILED )---------]\n");
            return e_failure;
        }
    }
    else
    {
        printf("\n\n[-- ERROR --: ( Please Pass The Correct Options )--]\n");
        return e_failure;
    }
}

// Function Defination.
OperationType check_operation_type(char *argv[])
{
    if ((strcmp(argv[1], "-e")) == 0)
    {
        return e_encode;
    }
    else if ((strcmp(argv[1], "-d")) == 0)
    {
        return e_decode;
    }
    else
    {
        return e_unsupported;
    }
}