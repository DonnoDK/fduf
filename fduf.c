#include <stdio.h>
#include <unistd.h>
#include "fileinfo.h"
#include "getfiles.h"

struct fileinfo* prune_unique_files(struct fileinfo* list){
    if(list == NULL){
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

void usage(){
    printf("usage: fduf [options] [path]\n");
    printf("options:\n-v\tverbose output\n-r\tscan is recursive\n");
}


int main(int argc, char** argv){
    if(argc == 1){
        fprintf(stderr, "fduf: no path given\n");
        usage();
        return 1;
    }
    const char* path = argv[argc - 1];
    int verbose = 0;
    int recursive = 0;
    int opt;
    while((opt = getopt(argc, argv, "rv")) != -1){
        switch(opt){
            case 'r': recursive = 1; break;
            case 'v': verbose = 1; break;
            default: usage(); return 1;
        }
    }
    struct fileinfo* files = filenames_in_path(path, recursive);
    if(files == NULL){
        fprintf(stderr, "fduf: no files found in path: %s\n", path);
        return 1;
    }
    fileinfo_get_filesize_for_list(files);
    if(verbose == 1){
        printf("fduf: files found: %d\n", fileinfo_list_count(files));
        printf("fduf: total filesize: %db\n", fileinfo_list_total_filesize(files));
    }
    files = prune_unique_files(files);
    fileinfo_get_checksum_for_list(files);
    files = prune_unique_files(files);
    fileinfo_get_md5_for_list(files);
    files = prune_unique_files(files);
    if(files == NULL){
        if(verbose == 1){
            fprintf(stderr, "fduf: no duplicates found\n");
            return 0;
        }
    }
    if(verbose == 1){
        printf("fduf: files after pruning: %d\n", fileinfo_list_count(files));
        printf("fduf: bytes occupied by duplicates: %db\n", fileinfo_list_total_filesize(files));
    }
    files = fileinfo_list_sort_on_filesize(files);
    struct fileinfo* elem = NULL;
    for(elem = files; elem != NULL; elem = elem->next){
        printf("%s\n", elem->name);
        if(elem->next != NULL){
            if(fileinfo_equals(elem, elem->next) == 0){
                printf("\n");
            }
        }
    }
    fileinfo_list_delete(files);
    return 0;
}
