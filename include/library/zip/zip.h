#ifndef __ZIP__H__
#define __ZIP__H__

#include <stdint.h>

/* https://en.wikipedia.org/wiki/ZIP_(file_format) */

#define ZIP_SEEK_SET    (0)
#define ZIP_SEEK_CUR    (1)
#define ZIP_SEEK_END    (2)
#define NUM_OF_ZIP_SEEK (3)

#define ZIP_NULL_STREAM (0)

typedef enum {
    ZIP_OK,
    ZIP_FAIL,
    ZIP_FAIL_MEMORY_ALLOC,
    ZIP_NOT_SUPPORTED,
} zip_result_t;

typedef struct {
    uintptr_t(*zip_open)(uintptr_t end_point);
    void(*zip_close)(uintptr_t stream);
    int32_t(*zip_tell)(uintptr_t stream);
    zip_result_t(*zip_seek)(uintptr_t stream, int32_t offset, int32_t origin);
    zip_result_t(*zip_read)(uintptr_t stream, uint8_t *buf, int32_t len);
    uintptr_t(*zip_malloc)(size_t size);
    uintptr_t(*zip_calloc)(size_t count, size_t size);
    void(*zip_free)(uintptr_t addr);
} zip_ops_t;

typedef struct zip_structure zip_t;

zip_t *zip_initialize(zip_ops_t *ops);
zip_result_t zip_parse(zip_t *handle, uintptr_t end_point);
zip_result_t zip_decompress(zip_t *handle);
void zip_deinitialize(zip_t *handle);

#endif  //!__ZIP__H__
