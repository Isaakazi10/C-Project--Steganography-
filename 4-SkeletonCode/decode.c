#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if (strcmp(strstr(argv[2], "."), ".bmp") == 0)
    {
        decInfo->stego_image_fname = argv[2];
    }
    else
    {
        fprintf(stderr, "\n\n[-- ERROR --: ( Stego Image %s format should be .bmp file )--]\n", argv[2]);
        return e_failure;
    }

    if (argv[3] != NULL)
    {
        decInfo->secret_fname = argv[3];
    }
    else
    {
        decInfo->secret_fname = "Default_output";
    }

    return e_success;
}

/*Do Encoding and calling different functions*/
Status do_decoding(DecodeInfo *decInfo)
{
    if (open_decode_files(decInfo) == e_success)
    {
        printf("\n[---( 2 Open Files -> SUCCESSFUL )---]");
    }
    else
    {
        printf("\n\n[-- ERROR --: ( Open Files -> FAILED )--]\n");
        return e_failure;
    }

    if (decode_magic_string(MAGIC_STRING, decInfo) == e_success)
    {
        printf("\n[---( 3 Magic String -> FOUND )---]");
    }
    else
    {
        printf("\n\n[-- ERROR --: ( Magic String -> FAILED )--]\n");
        return e_failure;
    }

    if (decode_secret_file_extn(decInfo) == e_success)
    {
        printf("\n[---( 4 File Extension -> FOUND )---]");
    }
    else
    {
        printf("\n\n[-- ERROR --: ( File Extention -> FAILED )--]\n");
        return e_failure;
    }

    if (decode_secret_file_size(decInfo) == e_success)
    {
        printf("\n[---( 5 Secret File Size -> FOUND )---]");
    }
    else
    {
        printf("\n\n[-- ERROR --: ( Secret File Size -> FAILED )--]\n");
        return e_failure;
    }

    if (decode_secret_file_data(decInfo) == e_success)
    {
        printf("\n[---( 6 Decode Secret File Size -> FOUND )---]");
    }
    else
    {
        printf("\n\n[-- ERROR --: ( Decode Secret File Data -> FAILED )--]\n");
        return e_failure;
    }

    return e_success;
}

/*
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_decode_files(DecodeInfo *decInfo)
{
    // Src Image file
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");

    // Do Error handling
    if (decInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "\n\n[-- ERROR --: ( Unable to Open Image %s )--]\n\n", decInfo->stego_image_fname);
        return e_failure;
    }

    // No failure return e_success
    return e_success;
}

Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo)
{
    char array[2] = {0};
    fseek(decInfo->fptr_stego_image, 54, SEEK_SET);

    if ((decode_image_to_data(array, strlen(magic_string), decInfo)) == e_success)
    {
        if (strcmp(array, magic_string) == 0)
        {
            return e_success;
        }
        else
        {
            return e_failure;
        }
    }
    else
    {
        printf("\n\n[-- ERROR --: ( Decode Image to Data -> FAILED )--]\n");
        return e_failure;
    }
}

Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    if (decode_image_to_data(decInfo->extn_secret_file, 4, decInfo) == e_success)
    {
        char str[100];
        strcpy(str, decInfo->secret_fname);
        strcat(str, decInfo->extn_secret_file);
        decInfo->fptr_secret = fopen(str, "w");
    }
    else
    {
        printf("\n\n[-- ERROR --: ( Decode Image to data -> FAILED )--]\n");
        return e_failure;
    }
    return e_success;
}

Status decode_secret_file_size(DecodeInfo *decInfo)
{
    if (decode_image_to_int_data(1, decInfo) == e_success)
    {
        return e_success;
    }
    else
    {
        printf("\n\n[-- ERROR --: ( Decode Image to Integer Data -> FAILED )--]\n");
        return e_failure;
    }
}

Status decode_secret_file_data(DecodeInfo *decInfo)
{
    int size = decInfo->size_secret_file;
    char arr[size];

    if (decode_image_to_data(arr, decInfo->size_secret_file, decInfo) == e_success)
    {
        fwrite(arr, 1, decInfo->size_secret_file, decInfo->fptr_secret);
        return e_success;
    }
    else
    {
        printf("\n\n[-- ERROR --: ( Decode Image to Data -> FAILED )--]\n");
        return e_failure;
    }
}

Status decode_image_to_data(char array[], int size, DecodeInfo *decInfo)
{
    for (int i = 0; i < size; i++)
    {
        char str[8];

        if (fread(str, 1, 8, decInfo->fptr_stego_image) == 0)
        {
            return e_failure;
        }

        if (decode_lsb_to_byte(&array[i], str, decInfo) != e_success)
        {
            return e_failure;
        }
    }
    return e_success;
}

Status decode_lsb_to_byte(char array[], char *image_buffer, DecodeInfo *decInfo)
{
    array[0] = 0;
    for (int i = 0; i < 8; i++)
    {
        *array = (*array | ((image_buffer[i] & 1) << (7 - i)));
    }
    return e_success;
}

Status decode_image_to_int_data(int size, DecodeInfo *decInfo)
{
    for (int i = 0; i < size; i++)
    {
        char str[32];

        if (fread(str, 1, 32, decInfo->fptr_stego_image) == 0)
        {
            return e_failure;
        }

        if (decode_int_to_lsb(decInfo, str) != e_success)
        {
            return e_failure;
        }
    }
    return e_success;
}

Status decode_int_to_lsb(DecodeInfo *decInfo, char *image_buffer)
{
    int j;
    for (j = 0; j < 32; j++)
    {
        decInfo->size_secret_file = ((decInfo->size_secret_file) | (image_buffer[j] & 1) << (31 - j));
    }

    if (j >= 32)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}