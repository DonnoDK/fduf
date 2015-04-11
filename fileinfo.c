#include "fileinfo.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <openssl/md5.h>

struct fileinfo* fileinfo_new(){
    struct fileinfo* elem = malloc(sizeof(struct fileinfo));
    elem->name = NULL;
    elem->next = NULL;
    elem->size = 0;
    elem->unique = 0;
    elem->checksum = 0;
    elem->md5 = NULL;
    return elem;
}

void fileinfo_delete(struct fileinfo* fi){
    if(fi->name != NULL){
        free(fi->name);
    }
    if(fi->md5 != NULL){
        free(fi->md5);
    }
    free(fi);
}

void fileinfo_list_delete(struct fileinfo* list){
    while(list != NULL){
        struct fileinfo* temp = list;
        list = list->next;
        fileinfo_delete(temp);
    }
}

static unsigned int size_for_file_at_path(const char* path){
    FILE* file = fopen(path, "rb");
    if(file == NULL){
        fprintf(stderr, "error: could not open file at path: %s\n", path);
        return 0;
    }
    fseek(file, 0, SEEK_END);
    unsigned int size = ftell(file);
    fclose(file);
    return size;
}

void fileinfo_get_filesize_for_list(struct fileinfo* list){
    while(list != NULL){
        list->size = size_for_file_at_path(list->name);
        list = list->next;
    }
}

static unsigned int checksum_for_filepath(const char* path, unsigned int filesize){
    FILE* file = fopen(path, "rb");
    if(file == NULL){
        fprintf(stderr, "could not open file at path: %s\n", path);
        return 0;
    }
    unsigned int buffersize = 1024;
    if(filesize < 1024){
        buffersize = filesize;
    }
    unsigned long long checksum = 0;
    unsigned char buffer[buffersize];
    memset(buffer, 0, buffersize);
    fseek(file, filesize - buffersize, SEEK_SET);
    fread(buffer, buffersize, sizeof(unsigned char), file);
    for(int i = 0; i < buffersize; i++){
        checksum = checksum + (buffer[i] % 255);
    }
    fclose(file);
    return checksum;
}

void fileinfo_get_checksum_for_list(struct fileinfo* list){
    while(list != NULL){
        list->checksum = checksum_for_filepath(list->name, size_for_file_at_path(list->name));
        list = list->next;
    }
}

static unsigned char* md5_for_filepath(const char* path, unsigned int filesize){
    FILE* file = fopen(path, "rb");
    if(file == NULL){
        fprintf(stderr, "could not open file at path: %s\n", path);
        return 0;
    }
    unsigned char* result = malloc(sizeof(unsigned char) * MD5_DIGEST_LENGTH);
    MD5_CTX md5_context;
    unsigned char buffer[1024];
    unsigned int bytes;
    MD5_Init(&md5_context);
    while((bytes = fread(buffer, 1, 1024, file)) != 0){
        MD5_Update(&md5_context, buffer, bytes);
    }
    MD5_Final(result, &md5_context);
    fclose(file);
    return result;
}

void fileinfo_get_md5_for_list(struct fileinfo* list){
    while(list != NULL){
        // TODO: implement
        list->md5 = md5_for_filepath(list->name, size_for_file_at_path(list->name));
        list = list->next;
    }
}

static int compare_fileinfo_size(const void* a, const void* b){
    const struct fileinfo* fa = *(const struct fileinfo**)a;
    const struct fileinfo* fb = *(const struct fileinfo**)b;
    if(fa->size < fb->size){
        return -1;
    }
    if(fa->size == fb->size){
        return fileinfo_equals(fa, fb);
    }
    if(fa->size > fb->size){
        return 1;
    }
    return 0;
}


struct fileinfo* fileinfo_list_sort_on_filesize(struct fileinfo* list){
    int count = 0;
    for(struct fileinfo* temp = list; temp != NULL; temp = temp->next){
        count++;
    }
    struct fileinfo* array[count];
    int i = 0;
    for(i = 0; i < count; i++){
        array[i] = list;
        list = list->next;
    }
    qsort(array, count, sizeof(struct fileinfo*), compare_fileinfo_size);
    array[count - 1]->next = NULL;
    struct fileinfo* head = array[0];
    for(i = 0; i < count - 1; i++){
        array[i]->next = array[i + 1];
    }
    return head;
}

unsigned int fileinfo_equals(const struct fileinfo* f1, const struct fileinfo* f2){
    if(f1->size == f2->size && f1->checksum == f2->checksum){
        if(f1->md5 != NULL && f2->md5 != NULL){
            for(int i = 0; i < MD5_DIGEST_LENGTH; i++){
                if(f1->md5[i] != f2->md5[i]){
                    return 0;
                }
            }
        }
        return 1;
    }
    return 0;
}
