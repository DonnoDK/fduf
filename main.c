struct dirnames_t{
    char** names;
    int count;
};

struct paths_s{
    const char* name;
    struct paths_s* next;
};

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
char* concatinate_paths(const char* first_part, const char* last_part){
    char last_of_first_part = first_part[strlen(first_part) - 1];
    char first_of_last_part = last_part[0];
    if(first_of_last_part == '/'){
        fprintf(stderr, "warning: invalid path concatination for paths: %s and %s", first_part, last_part);
        return NULL;
    }
    int should_append_divider = 1;
    if(last_of_first_part == '/'){
        should_append_divider = 0;
    }
    char* path = (char*)malloc(sizeof(char*) * (strlen(first_part) + strlen(last_part) + 1 + should_append_divider));
    path[0] = '\0';
    strcat(path, first_part);
    if(should_append_divider == 1){
        strcat(path, "/");
    }
    strcat(path, last_part);
    return path;
}
struct dirnames_t* find_directories_in_path(const char* path){
    // TODO: check for NULL path
    DIR* directory = opendir(path);
    if(directory == NULL){
        if(errno == EACCES){
            fprintf(stderr, "error: permission denied: %s\n", path);
        }else{
            fprintf(stderr, "error: could not open path as directory: %s\n", path);
        }
        return NULL;
    }
    // TODO: check permission for dir (executable)
    closedir(directory);
    struct dirnames_t* dirnames = (struct dirnames_t*)malloc(sizeof(struct dirnames_t*));
    dirnames->count = 1;
    dirnames->names = (char**)malloc(sizeof(char**));
    dirnames->names[dirnames->count - 1] = strdup(path);
    

    /* refactoring begin here */
    struct paths_s* head = (struct paths_s*)malloc(sizeof(struct paths_s*));
    struct paths_s* current_path = head;
    struct paths_s* last_elem = head;
    head->name = strdup(path);
    head->next = NULL;

    while(current_path != NULL){
        DIR* directory = opendir(current_path->name);
        if(directory == NULL){
            if(errno == EACCES){
                fprintf(stderr, "error: insufficient permission for path: %s\n", current_path->name);
            }else{
                fprintf(stderr, "error: failed to open path: %s\n", current_path->name);
            }
            current_path = current_path->next;
            continue;
        }
        struct dirent* entry = NULL;
        while((entry = readdir(directory)) && entry != NULL){
            if(entry->d_type == DT_DIR){
                if(!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")){
                    continue;
                }
                struct paths_s* elem = malloc(sizeof(struct paths_s*));
                elem->name = concatinate_paths(current_path->name, entry->d_name);
                last_elem->next = elem;
                last_elem = elem;
            }
        }
        closedir(directory);
        current_path = current_path->next;
    }
    /* refactoring end here */

    int current = 0;
    while(current != dirnames->count){
        const char* current_path = dirnames->names[current];
        DIR* directory = opendir(current_path);
        if(directory == NULL){
            fprintf(stderr, "error: could not open path as directory: %s\n", current_path);
            current++;
            continue;
        }
        struct dirent* entry = NULL;
        while((entry = readdir(directory)) && entry != NULL){
            if(entry->d_type == DT_DIR){
                if(!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")){
                    continue;
                }
                dirnames->count++;
                dirnames->names = (char**)realloc(dirnames->names, sizeof(char*) * dirnames->count);
                char* dirname = concatinate_paths(current_path, entry->d_name);
                dirnames->names[dirnames->count - 1] = dirname;
            }
        }
        closedir(directory);
        current++;
    }
    return dirnames;
}
int main(int argc, char** argv){
    // TODO: checkout input
    struct dirnames_t* dirnames = find_directories_in_path(argv[1]);
    if(dirnames == NULL){
        fprintf(stderr, "error: invalid base path\n");
        return 1;
    }
    for(int i = 0; i < dirnames->count; i++){
        printf("%s\n", dirnames->names[i]);
    }
    return 0;
}
