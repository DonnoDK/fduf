#include "fileinfo.h"
#include "getfiles.h"
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static char* concatinate_paths(const char* first_part, const char* last_part){
    char first_of_last_part = last_part[0];
    if(first_of_last_part == '/'){
        fprintf(stderr, "error: invalid path concatination for paths: %s and %s", first_part, last_part);
        return NULL;
    }
    char last_of_first_part = first_part[strlen(first_part) - 1];
    int should_append_divider = 1;
    if(last_of_first_part == '/'){
        should_append_divider = 0;
    }
    char* path = malloc(sizeof(char) * (strlen(first_part) + strlen(last_part) + 1 + should_append_divider));
    path[0] = '\0';
    strcat(path, first_part);
    if(should_append_divider == 1){
        strcat(path, "/");
    }
    strcat(path, last_part);
    return path;
}

static struct fileinfo* find_type_in_paths(struct fileinfo* paths, unsigned char type){
    struct fileinfo* head = NULL;
    struct fileinfo* last_elem = NULL;
    struct fileinfo* current = paths;
    while(current != NULL){
        DIR* directory = opendir(current->name);
        if(directory == NULL){
            if(errno == EACCES){
                fprintf(stderr, "error: permission denied: %s\n", current->name);
            }else{
                fprintf(stderr, "error: could not open path as directory: %s\n", current->name);
            }
            if(head == NULL){
                return NULL;
            }
            current = current->next;
            continue;
        }
        if(head == NULL && type == DT_DIR){
            head = current;
            last_elem = head;
        }
        struct dirent* entry = NULL;
        while((entry = readdir(directory)) && entry != NULL){
            if(entry->d_type == type){
                if(!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")){
                    continue;
                }
                struct fileinfo* elem = fileinfo_new();
                elem->name = concatinate_paths(current->name, entry->d_name);
                if(last_elem == NULL){
                    last_elem = elem;
                    head = elem;
                }else{
                    last_elem->next = elem;
                    last_elem = elem;
                }
            }
        }
        closedir(directory);
        current = current->next;
    }
    return head;
}

struct fileinfo* filenames_in_path(const char* path, int recursive){
    struct fileinfo* base = fileinfo_new();
    base->name = strdup(path);
    if(recursive == 1){
        struct fileinfo* directories = find_type_in_paths(base, DT_DIR);
        struct fileinfo* files = find_type_in_paths(directories, DT_REG);
        fileinfo_list_delete(directories);
        return files;
    }
    struct fileinfo* files = find_type_in_paths(base, DT_REG);
    fileinfo_delete(base);
    return files;
}
