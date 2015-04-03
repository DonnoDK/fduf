#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
typedef enum {false, true} bool;
char* concatinate_paths(const char* first_part, const char* last_part){
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

struct paths_s{
    unsigned int size;
    char* name;
    struct paths_s* next;
    unsigned char unique;
    unsigned long long checksum;
};


struct paths_s* element_with_path(char* path){
    struct paths_s* elem = malloc(sizeof(struct paths_s));
    elem->name = path;
    elem->next = NULL;
    elem->size = 0;
    elem->unique = 0;
    elem->checksum = 0;
    return elem;
}
struct paths_s* find_type_in_paths(struct paths_s* paths, unsigned char type){
    struct paths_s* head = NULL;
    struct paths_s* last_elem = NULL;
    struct paths_s* current = paths;
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
                struct paths_s* elem = element_with_path(concatinate_paths(current->name, entry->d_name));
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

void delete_element(struct paths_s* elem){
    free(elem->name);
    free(elem);
}
void delete_list(struct paths_s* list){
    while(list != NULL){
        struct paths_s* temp = list;
        list = list->next;
        delete_element(temp);
    }
}

unsigned int size_for_file_at_path(const char* path){
    FILE* file = fopen(path, "rb");
    if(file == NULL){
        fprintf(stderr, "error: could not open file at path: %s\n", path);
    }
    fseek(file, 0, SEEK_END);
    unsigned int size = ftell(file);
    fclose(file);
    return size;
}

void size_for_files_in_paths(struct paths_s* paths){
    while(paths != NULL){
        paths->size = size_for_file_at_path(paths->name);
        paths = paths->next;
    }
}

int filepath_has_unique_size(struct paths_s* path, struct paths_s* paths){
    if(path == NULL || paths == NULL){
        return 1;
    }
    while(paths != NULL){
        if(paths == path){
            paths = paths->next;
            continue;
        }
        if(paths->size == path->size){
            return 0;
        }
        paths = paths->next;
    }
    return 1;
}

int filepath_has_unique_short_checksum(struct paths_s* path, struct paths_s* paths){
    if(path == NULL || paths == NULL){
        return 1;
    }
    while(paths != NULL){
        if(paths == path){
            paths = paths->next;
            continue;
        }
        if(paths->checksum == path->checksum){
            return 0;
        }
        paths = paths->next;
    }
    return 1;

}

void mark_unique_filepaths_on_size(struct paths_s* paths){
    struct paths_s* head = paths;
    while(paths != NULL){
        paths->unique = filepath_has_unique_size(paths, head);
        paths = paths->next;
    }
}

void mark_unique_filepaths_on_short_checksum(struct paths_s* paths){
    struct paths_s* head = paths;
    while(paths != NULL){
        paths->unique = filepath_has_unique_short_checksum(paths, head);
        paths = paths->next;
    }
}

struct paths_s* prune_unique_filepaths(struct paths_s* paths){
    struct paths_s* head = paths;
    struct paths_s* previous = head;
    while(paths != NULL){
        if(paths->unique == 1){
            if(paths == head){
                struct paths_s* temp = paths;
                paths = paths->next;
                head = paths;
                previous = paths;
                delete_element(temp);
            }else{
                previous->next = paths->next;
                delete_element(paths);
                paths = previous->next;
            }
        }else{
            previous = paths;
            paths = paths->next;
        }
    }
    return head;
}

unsigned int checksum_for_filepath(const char* path){
    FILE* file = fopen(path, "rb");
    if(file == NULL){
        return 0;
    }
    unsigned long long checksum = 0;
    unsigned char buffer[1024];
    fread(buffer, 1024, sizeof(unsigned char), file);
    for(int i = 0; i < 1024; i++){
        checksum = checksum + (buffer[i] % 255);
    }
    fclose(file);
    return checksum;
}
void calculate_checksum_for_paths(struct paths_s* paths){
    while(paths != NULL){
        paths->checksum = checksum_for_filepath(paths->name);
        paths = paths->next;
    }
}

struct paths_s* all_files_in_base_path(const char* path, bool recursive){
    struct paths_s* base = element_with_path(strdup(path));
    if(recursive == true){
        struct paths_s* directories = find_type_in_paths(base, DT_DIR);
        struct paths_s* files = find_type_in_paths(directories, DT_REG);
        delete_list(directories);
        return files;
    }
    struct paths_s* files = find_type_in_paths(base, DT_REG);
    delete_element(base);
    return files;
}

int main(int argc, char** argv){
    /* TODO: checkout input */
    if(argv[1] == NULL){
        fprintf(stderr, "error: no path given\n");
        return 1;
    }
    struct paths_s* files = all_files_in_base_path(argv[1], true);
    size_for_files_in_paths(files);
    mark_unique_filepaths_on_size(files);
    files = prune_unique_filepaths(files);
    if(files == NULL){
        fprintf(stderr, "pruned all on filesize\n");
        return 0;
    }
    calculate_checksum_for_paths(files);
    mark_unique_filepaths_on_short_checksum(files);
    files = prune_unique_filepaths(files);
    if(files == NULL){
        fprintf(stderr, "pruned all 2\n");
        return 0;
    }
    struct paths_s* files_head = files;
    while(files != NULL){
        printf("%s, size: %d,  checksum: %lld\n", files->name, files->size, files->checksum);
        files = files->next;
    }
    delete_list(files_head);
    return 0;
}
