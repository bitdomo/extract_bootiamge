#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct __header{
    char magic[4];                      // "FBPK"
    int unknown_1;
    int unknown_2;
    int unknown_3;
    char board_name[16];            // not sure
    char bootloader_version[68];    // not sure about the length
    int unknown_4;                  // maybe block size
    int image_file_count;
    int bootloader_image_size;
}__header;

typedef struct __image_file_table{
    int unknown_1;
    char image_name[76];            // not sure about the length
    int offset_in_bootloader_image;
    int unknown_2;
    int image_size;
    int unknown_3;
    int unknown_4;
    int unknown_5;
}__image_file_table;

int main(int argc, char* argv[]){
    char buff;
    __header header;
    __image_file_table* image_file_table = NULL;
    FILE* input = NULL;
    FILE* output = NULL;
    char filename[256] = {0};
    if(argc == 1){
        printf("Not enough argument\n");
        return -1;
    }
    if(argc > 2){
        printf("Too many arguments\n");
        return -1;
    }

    input = fopen(argv[1], "rb");
    if(input == NULL){
        printf("Failed to open: %s", argv[1]);
        return -1;
    }

    fread(&header, sizeof(header), 1, input);

    if(header.magic[0] != 'F' || header.magic[1] != 'B' || header.magic[2] != 'P' || header.magic[3] != 'K'){
        printf("Invalid magic!\nExpected: FBPK\nGot: %c%c%c%c", header.magic[0], header.magic[1], header.magic[2], header.magic[3]);
        return -1;
    }

    printf("(INFO) magic: %c%c%c%c\n", header.magic[0], header.magic[1], header.magic[2], header.magic[3]);
    printf("(INFO) unknown_1: 0x%08X\n", header.unknown_1);
    printf("(INFO) unknown_2: 0x%08X\n", header.unknown_2);
    printf("(INFO) unknown_3: 0x%08X\n", header.unknown_3);
    printf("(INFO) board name: %s\n", header.board_name);
    printf("(INFO) bootloader version: %s\n", header.bootloader_version);
    printf("(INFO) unknown_4: 0x%08X\n", header.unknown_4);
    printf("(INFO) image file count: %d\n", header.image_file_count);
    printf("(INFO) bootloader image size: %d\n", header.bootloader_image_size);

    image_file_table = (__image_file_table*)malloc(sizeof(__image_file_table)*header.image_file_count);
    if(image_file_table == NULL){
        printf("Failed to allocate memory for \"image_file_table\"\n");
        return -1;
    }

    fread(image_file_table, sizeof(__image_file_table), header.image_file_count, input);

    for(int i = 0; i < header.image_file_count; i++){
        printf("(INFO) image file: %d\n", i+1);
        printf("(INFO)\t\tunknown_1: 0x%08X\n", image_file_table[i].unknown_1);
        printf("(INFO)\t\timage name: %s\n", image_file_table[i].image_name);
        printf("(INFO)\t\toffset in bootloader image: 0x%08X\n", image_file_table[i].offset_in_bootloader_image);
        printf("(INFO)\t\tunknown_2: 0x%08X\n", image_file_table[i].unknown_2);
        printf("(INFO)\t\timage size: %d\n", image_file_table[i].image_size);
        printf("(INFO)\t\tunknown_3: 0x%08X\n", image_file_table[i].unknown_3);
        printf("(INFO)\t\tunknown_4: 0x%08X\n", image_file_table[i].unknown_4);
        printf("(INFO)\t\tunknown_5: 0x%08X\n", image_file_table[i].unknown_5);
    }

    for(int i = 0; i < header.image_file_count; i++){

        if(i == 2 || i == 3 || i == 4 || i == 5){
            image_file_table[i].image_name[9] = '_';
        }
        switch(i){
            case 0:
                strcpy(filename, image_file_table[i].image_name);
                strcat(filename, "_UFSZ.bin");
                break;
            case 1:
                strcpy(filename, image_file_table[i].image_name);
                strcat(filename, "_UFSP.bin");
                break;
            default:
                strcpy(filename, image_file_table[i].image_name);
                strcat(filename, ".bin");
        }

        output = fopen(filename, "wb");
        if(output == NULL){
            printf("Failed to open %s\n", image_file_table[i].image_name);
            return -1;
        }

        fseek(input, image_file_table[i].offset_in_bootloader_image, SEEK_SET);
        for(int j = 0; j < image_file_table[i].image_size; j++){
            buff = fgetc(input);
            fputc(buff, output);
        }
        fclose(output);
    }
    free(image_file_table);
    fclose(input);
    return 0;
}
