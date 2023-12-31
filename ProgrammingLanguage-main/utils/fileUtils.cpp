
#include "include/fileUtils.h"
//-----------------------------------------------------------------------------

FILE* get_file (const char file_name[], const char* mode)
{
    FILE* tmp_file = fopen(file_name, mode);

    if (tmp_file)
    {
        printf("File name is %s\n", file_name);
        return tmp_file;
    }

    printf("Failed to open the file %s.\n", file_name);

    return nullptr;
}


FILE* close_file (FILE* file, char* name)
{
    assert (file != nullptr);

    printf ("Succesfully closing the file: %s\n", name);

    fclose (file);

    return 0;
}
