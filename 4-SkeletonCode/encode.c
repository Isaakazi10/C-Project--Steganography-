#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    if (strcmp(strstr(argv[2], "."), ".bmp") == 0)
    {
        encInfo->src_image_fname = argv[2];
    }
    else
    {
        fprintf(stderr, "\n\n[-- ERROR --: ( Stego Image %s format should be .bmp file )--]\n", argv[2]);
        return e_failure;
    }

    if (argv[3] != NULL)
    {
        char *ptr;
        if ((ptr = (strchr(argv[3], '.'))) != NULL)
        {
            strcpy(encInfo->extn_secret_file, ptr);
            encInfo->secret_fname = argv[3];
        }
        else
        {
            printf("\n\n[-- ERROR --: ( Extention Not Found )--]\n");
            return e_failure;
        }
    }

    if (argv[4] != NULL)
    {
        encInfo->stego_image_fname = argv[4];
    }
    else
    {
        encInfo->stego_image_fname = "Default_stego.bmp";
    }

    return e_success;
}

/*Do Encoding and calling different functions*/
Status do_encoding(EncodeInfo *encInfo)
{
    if (open_files(encInfo) == e_success)
    {
        printf("\n[---( 2 Open Files -> SUCCESSFUL )---]");
    }
    else
    {
        printf("\n\n[-- ERROR --: ( Open Files -> FAILED )--]\n");
        return e_failure;
    }

    if ((check_capacity(encInfo)) == e_success)
    {
        printf("\n[---( 3 Checking Capacity -> SUCCESSFUL )---]");
    }
    else
    {
        printf("\n\n[-- ERROR --: ( Check Capacity -> FAILED )--]\n");
        return e_failure;
    }

    if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
    {
        printf("\n[---( 4 BMP Header -> COPIED )---]");
    }
    else
    {
        printf("\n\n[-- ERROR --: ( BMP header -> NOT COPIED )--]\n");
        return e_failure;
    }

    if (encode_magic_string(MAGIC_STRING, encInfo) == e_success)
    {
        printf("\n[---( 5 Encode Magic -> ENCODED )---]");
    }
    else
    {
        printf("\n\n[-- ERROR --: ( Encode Magic String -> FAILED )--]\n");
        return e_failure;
    }

    if (encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_success)
    {
        printf("\n[---( 6 Encode Secret File Extention ->  ENCODED )---]");
    }
    else
    {
        printf("\n\n[-- ERROR --: ( Encode Secret File Extention -> FAILED )--]\n");
        return e_failure;
    }

    if (encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_success)
    {
        printf("\n[---( 7 Encode Secret File Size -> ENCODED )---]");
    }
    else
    {
        printf("\n\n[-- ERROR --: ( Encode Secret File Size -> FAILED )--]\n");
        return e_failure;
    }

    if (encode_secret_file_data(encInfo) == e_success)
    {
        printf("\n[---( 8 Encode Secret File Data -> ENCODED )---]");
    }
    else
    {
        printf("\n\n[-- ERROR --: ( Encode Secret File Data -> FAILED )--]\n");
        return e_failure;
    }

    if ((copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image)) == 0)
    {
        printf("\n[---( 9 Copy Remaining Image data -> ENCODED )---]");
    }
    else
    {
        printf("\n\n[-- ERROR --: ( Copy Remaining Image data -> FAILED )--]\n");
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
Status open_files(EncodeInfo *encInfo)
{
    // Making a FILE * for .bmp image file.
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");

    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "\n\n[-- ERROR --: ( Unable to Open Image %s )--]\n\n", encInfo->src_image_fname);

        return e_failure;
    }

    // Making a FILE * for secret file.
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "\n\n[-- ERROR --: ( Unable to Open Image %s )--]\n\n", encInfo->secret_fname);

        return e_failure;
    }

    // Making a FILE * for output file.
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "\n\n[-- ERROR --: ( Unable to Open Image %s )--]\nn", encInfo->stego_image_fname);

        return e_failure;
    }

    // No failure return e_success
    return e_success;
}

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

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);

    // Return image capacity
    return width * height * 3;
}

// Checking Capacity of .bmp file.
Status check_capacity(EncodeInfo *encInfo)
{
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);

    if ((get_image_size_for_bmp(encInfo->fptr_src_image)) > (8 * (strlen(MAGIC_STRING) + sizeof(int) + sizeof(int) + 4 + encInfo->size_secret_file)))
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

// Function to get the file size of secret file
uint get_file_size(FILE *fptr)
{
    uint sec_file_size = 0;
    fseek(fptr, 0, SEEK_END);
    sec_file_size = ftell(fptr);
    rewind(fptr);
    return sec_file_size;
}

// Function for copying .bmp header to ouput file.
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    // For setting the offset position to 0.
    rewind(fptr_src_image);
    // The array is used as a buffer.
    char str[54];

    if (fread(str, 1, 54, fptr_src_image) == 0)
    {
        return e_failure;
    }
    if (fwrite(str, 1, 54, fptr_dest_image) == 0)
    {
        return e_failure;
    }
    return e_success;
}

// Function Defination to encode magic string into the outut image.
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    if ((encode_data_to_image((char *)magic_string, strlen(magic_string), encInfo->fptr_src_image, encInfo->fptr_stego_image)) == e_success)
    {
        return e_success;
    }
    else
    {
        printf("\n\n[-- ERROR --: ( Encode Data to Image -> FAILED )--]\n");
        return e_failure;
    }
}

// Function Defination to encode secret file extention into the outut image.
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    if ((encode_data_to_image((char *)file_extn, 4, encInfo->fptr_src_image, encInfo->fptr_stego_image)) == e_success)
    {
        return e_success;
    }
    else
    {
        printf("\n\n[-- ERROR --: ( Encode Data to Image -> FAILED )--]\n");
        return e_failure;
    }
}

// Function Defination to encode secret file size into the outut image.
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    if (encode_int_data_to_image(file_size, 1, encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
    {
        return e_success;
    }
    else
    {
        printf("\n\n[-- ERROR --: ( Encode Integer Data to Image -> FAILED )--]\n");
        return e_failure;
    }
}

// Function Defination to encode secret file data into the outut image.
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char arr[encInfo->size_secret_file];
    fread(arr, 1, encInfo->size_secret_file, encInfo->fptr_secret);

    if (encode_data_to_image(arr, encInfo->size_secret_file, encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
    {
        return e_success;
    }
    else
    {
        printf("\n\n[-- ERROR --: ( Encode Data to Image -> FAILED )--]\n");
        return e_failure;
    }
}

// Function Defination to encode remaining data into the outut image.
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char remaining_data[1];

    while (fread(remaining_data, 1, 1, fptr_src))
    {
        if ((fwrite(remaining_data, 1, 1, fptr_dest)) == 0)
        {
            return e_failure;
        }
    }
    return e_success;
}

// Below 2 successive Functions are for encoding char to lsb.
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    for (int i = 0; i < size; i++)
    {
        char str[8];

        if (fread(str, 1, 8, fptr_src_image) == 0)
        {
            return e_failure;
        }

        if (encode_byte_to_lsb(data[i], str) != e_success)
        {
            return e_failure;
        }

        if (fwrite(str, 1, 8, fptr_stego_image) == 0)
        {
            return e_failure;
        }
    }
    return e_success;
}
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for (int j = 0; j < 8; j++)
    {
        image_buffer[j] = ((image_buffer[j] & (~1)) | ((data & (1 << (7 - j))) >> (7 - j)));
    }
    return e_success;
}

// Below 2 successive Functions are for encoding integer to lsb .
Status encode_int_data_to_image(long data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    for (int i = 0; i < size; i++)
    {
        char str[32];

        if (fread(str, 1, 32, fptr_src_image) == 0)
        {
            return e_failure;
        }

        if (encode_int_to_lsb(data, str) != e_success)
        {
            return e_failure;
        }

        if (fwrite(str, 1, 32, fptr_stego_image) == 0)
        {
            return e_failure;
        }
    }
    return e_success;
}
Status encode_int_to_lsb(long data, char *image_buffer)
{
    int j;
    for (j = 0; j < 32; j++)
    {
        image_buffer[j] = ((image_buffer[j] & ~1) | ((data & (1 << (31 - j))) >> (31 - j)));
    }

    if (j >= 32)
        return e_success;
    else
        return e_failure;
}