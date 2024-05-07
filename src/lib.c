#include "lib.h"

void str_free(Str* str)
{
    free(str->data);
}

// Read file in 2MB chunks.
#define READ_CHUNKSIZE 2097152

// Based on https://stackoverflow.com/a/44894946
bool load_file(char* path, Str* out)
{
    if (!path || !out) return false;

    FILE* f = fopen(path, "r");

    if (ferror(f)) return false;

    char* data = NULL;
    char* temp;
    u32 size = 0;
    u32 used = 0;
    u32 n;

    while (true)
    {
        if (used + READ_CHUNKSIZE + 1 > size)
        {
            size = used + READ_CHUNKSIZE + 1;

            temp = realloc(data, size);
            if (!temp)
            {
                // OOMed here. just move on for now, something else will complain
                free(data);
                fclose(f);
                return false;
            }

            data = temp;
        }

        n = fread(data + used, 1, READ_CHUNKSIZE, f);
        // Done reading?
        if (n == 0) break;
        used += n;
    }

    temp = realloc(data, used + 1);
    if (!temp)
    {
        // OOMed here. just move on for now, something else will complain
        free(data);
        fclose(f);
        return false;
    }

    data = temp;
    data[used] = '\0';

    *out = (Str){
        .data = data,
        .size = used + 1,
    };

    fclose(f);
    return true;
}
