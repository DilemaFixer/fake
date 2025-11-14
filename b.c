#include "b.h"

#define PROG_NAME "main"
#define BIN_FOLDER "./bin"
#define BUILD_FOLDER "./build"
#define SRC_FOLDER "./src"
#define GC "gcc"
#define GC_FLAGS "-Wall -Wextra -g -O2"

void make_dirs(void) {
    if (!dir_exists(BIN_FOLDER))
        make_dir(BIN_FOLDER, 0755);
    if (!dir_exists(BUILD_FOLDER))
        make_dir(BUILD_FOLDER, 0755);
    if (!dir_exists(SRC_FOLDER))
        make_dir(SRC_FOLDER, 0755);
}

Array* make_o_files(Array *c_files) {
    Array *o_files = array_new(c_files->count);
    
    for (size_t i = 0; i < c_files->count; i++) {
        char* name = path_basename((char *)c_files->items[i]);
        char* obj_file_path = pathjoin(BIN_FOLDER, name);
        obj_file_path = change_extension(obj_file_path, "o");
        
        RUN(GC, "-c", (char *)c_files->items[i], "-o", obj_file_path, 
            GC_FLAGS);
        
        array_add(o_files, obj_file_path);
    }
    return o_files;
}

void link_o(Array *o_files) {
    char *objs = o_files->items[0];
    for(size_t i = 1; i < o_files->count; i++){
        objs = strcat_new(objs, " ");
        objs = strcat_new(objs, o_files->items[i]); 
    }
    
    RUN(GC, objs, "-o", BUILD_FOLDER"/"PROG_NAME);
}

int main(void) {
    INFO("Start building\n");
    make_dirs();
    
    Array *c_files = find_all_files("./src", "c");
    Array *o_files = make_o_files(c_files);
    link_o(o_files);
    
    return 0;
}
