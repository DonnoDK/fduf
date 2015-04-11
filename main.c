#include <stdio.h>
#include <stdlib.h>
#include <openssl/md5.h>
#include "fileinfo.h"
#include "getfiles.h"

struct fileinfo* prune_unique_files(struct fileinfo* list){
    if(list == NULL){
        fprintf(stderr, "cannot prune empty list\n");
        return NULL;
    }
    struct fileinfo* elem = list;
    struct fileinfo* prev = list;
    while(elem != NULL){
        int is_unique = 1;
        struct fileinfo* temp = NULL;
        for(temp = list; temp != NULL; temp = temp->next){
            if(temp == elem){
                continue;
            }
            if(fileinfo_equals(temp, elem)){
                is_unique = 0;
                break;
            }
        }
        if(is_unique == 1){
            if(elem == list){
                struct fileinfo* temp = elem;
                elem = elem->next;
                list = elem;
                prev = elem;
                fileinfo_delete(temp);
            }else{
                prev->next = elem->next;
                fileinfo_delete(elem);
                elem = prev->next;
            }
        }else{
            prev = elem;
            elem = elem->next;
        }
    }
    return list;
}


int main(int argc, char** argv){
    /* TODO: checkout input */
    if(argv[1] == NULL){
        fprintf(stderr, "error: no path given\n");
        return 1;
    }
    struct fileinfo* files = filenames_in_path(argv[1], 1);
    if(files == NULL){
        fprintf(stderr, "no files found in path\n");
        return 1;
    }
    fileinfo_get_filesize_for_list(files);
    struct fileinfo* elem = NULL;
    int count = 0;
    unsigned long long sizes = 0;
    for(elem = files; elem != NULL; elem = elem->next){
        count++;
        sizes += elem->size;
    }
    fprintf(stderr, "files found: %d\n", count);
    fprintf(stderr, "total size: %.2fMiB\n", sizes / 1024.0f / 1024.0f);

    files = prune_unique_files(files);
    int pruned_count = 0;
    for(elem = files; elem != NULL; elem = elem->next){
        pruned_count++;
    }
    fprintf(stderr, "files after filesize pruning: %d\n", pruned_count);

    fileinfo_get_checksum_for_list(files);
    files = prune_unique_files(files);
    pruned_count = 0;
    unsigned long long pruned_sizes = 0;
    for(elem = files; elem != NULL; elem = elem->next){
        pruned_count++;
        pruned_sizes += elem->size;
    }
    fprintf(stderr, "files after checksum pruning: %d\n", pruned_count);
    fprintf(stderr, "MiB pruned: %.2f\n", (sizes - pruned_sizes) / 1024.0f / 1024.0f);
    fprintf(stderr, "MiB left for scanning: %.2f\n", pruned_sizes / 1024.0f / 1024.0f);

    fileinfo_get_md5_for_list(files);
    files = prune_unique_files(files);

    files = fileinfo_list_sort_on_filesize(files);
    sizes = 0;
    for(elem = files; elem != NULL; elem = elem->next){
        printf("%s ", elem->name);
        /*
        printf("%s size:%d chsum:%lld md5:", elem->name, elem->size, elem->checksum);
        */
        for(int i = 0; i < MD5_DIGEST_LENGTH; i++){
            printf("%02x", elem->md5[i]);
        }
        printf("\n");
        if(elem->next != NULL){
            if(fileinfo_equals(elem, elem->next) == 0){
                printf("\n");
            }
        }
        sizes += elem->size;
    }
    fprintf(stderr, "MiB occupied by duplicates: %.2f\n", sizes / 1024.0f / 1024.0f);
    fileinfo_list_delete(files);
    return 0;
}
