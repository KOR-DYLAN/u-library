#include <stdio.h>
#include <stdlib.h>
#include "library/zip/zip.h"

static uintptr_t test_zip_open(uintptr_t end_point);
static void test_zip_close(uintptr_t stream);
static int32_t test_zip_tell(uintptr_t stream);
static zip_result_t test_zip_seek(uintptr_t stream, int32_t offset, int32_t origin);
static zip_result_t test_zip_read(uintptr_t stream, uint8_t *buf, int32_t len);
static uintptr_t test_zip_malloc(size_t size);
static uintptr_t test_zip_calloc(size_t count, size_t size);
static void test_zip_free(uintptr_t addr);

int main(int argc, char *argv[])
{
    zip_ops_t ops = {
        .zip_malloc = test_zip_malloc,
        .zip_calloc = test_zip_calloc,
        .zip_free = test_zip_free,
        .zip_open = test_zip_open,
        .zip_close = test_zip_close,
        .zip_read = test_zip_read,
        .zip_seek = test_zip_seek,
        .zip_tell = test_zip_tell,
    };
    zip_t *handle;

    handle = zip_initialize(&ops);
    if (handle != NULL) {
        zip_parse(handle, (uintptr_t)argv[1]);
    }
    else {
        printf("zip_initialize() is failed...\n");
    }

    return 0;
}

static uintptr_t test_zip_open(uintptr_t end_point)
{
    FILE *fp;

    fp = fopen((const char *)end_point, "rb");
    if (fp == NULL) {
        printf("can not open '%s'...\n", (const char *)end_point);
    }

    return (uintptr_t)fp;
}

static void test_zip_close(uintptr_t stream)
{
    (void)fclose((FILE *)stream);
}

static int32_t test_zip_tell(uintptr_t stream)
{
    return (int32_t)ftell((FILE *)stream);
}

static zip_result_t test_zip_seek(uintptr_t stream, int32_t offset, int32_t origin)
{
    zip_result_t result; 
    const int whence[NUM_OF_ZIP_SEEK] = {
        SEEK_SET,
        SEEK_CUR,
        SEEK_END,
    };

    if (fseek((FILE *)stream, (long)offset, whence[origin]) == 0) {
        result = ZIP_OK;
    }
    else {
        result = ZIP_FAIL;
    }

    return result;
}

static zip_result_t test_zip_read(uintptr_t stream, uint8_t *buf, int32_t len)
{
    zip_result_t result;

    if (0 < fread(buf, len, 1, (FILE *)stream)) {
        result = ZIP_OK;
    }
    else {
        result = ZIP_FAIL;
    }

    return result;
}

static uintptr_t test_zip_malloc(size_t size)
{
    return (uintptr_t)malloc(size);
}

static uintptr_t test_zip_calloc(size_t count, size_t size)
{
    return (uintptr_t)calloc(count, size);
}

static void test_zip_free(uintptr_t addr)
{
    free((void *)addr);
}
