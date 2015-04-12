struct fileinfo{
    unsigned int size;
    char* name;
    unsigned char unique;
    unsigned long long checksum;
    unsigned char* md5;
    struct fileinfo* next;
};

struct fileinfo* fileinfo_new();
void fileinfo_delete(struct fileinfo* fi);
void fileinfo_list_delete(struct fileinfo* list);
unsigned int fileinfo_list_count(const struct fileinfo* list);
unsigned int fileinfo_list_total_filesize(const struct fileinfo* list);
void fileinfo_get_filesize_for_list(struct fileinfo* list);
void fileinfo_get_checksum_for_list(struct fileinfo* list);
void fileinfo_get_md5_for_list(struct fileinfo* list);
unsigned int fileinfo_equals(const struct fileinfo* f1, const struct fileinfo* f2);
struct fileinfo* fileinfo_list_sort_on_filesize(struct fileinfo* list);
