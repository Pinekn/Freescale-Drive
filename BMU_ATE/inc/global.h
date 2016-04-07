/*
*
*
*
*
*/

#ifndef __GLOBAL__H__
#define __GLOBAL__H__

#ifdef __cplusplus
extern "C" {
#endif

int get_module_file_name(char* pszName, int nLen);
char* get_module_file_path_name(char* pszName, int nLen);
int create_dir(const char* pszPathName, int nMode);
int is_dir(const char* pszPathName);
int is_file(const char* pszPathName);

#ifdef __cplusplus
};
#endif

#endif
