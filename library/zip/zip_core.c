#include <stdbool.h>
#include <string.h>
#include "library/zip/zip.h"
#include "zip_internal.h"

zip_t *zip_initialize(zip_ops_t *ops)
{
    zip_t *handle;
    size_t err_count = 0;

    if (ops != NULL) {
        if (ops->zip_calloc == NULL) {
            err_count += 1U;
        }
        if (ops->zip_malloc == NULL) {
            err_count += 1U;
        }
         if (ops->zip_free == NULL) {
            err_count += 1U;
        }
        if (ops->zip_open == NULL) {
            err_count += 1U;
        }
        if (ops->zip_close == NULL) {
            err_count += 1U;
        }
        if (ops->zip_read == NULL) {
            err_count += 1U;
        }
        if (ops->zip_seek == NULL) {
            err_count += 1U;
        }
        if (ops->zip_tell == NULL) {
            err_count += 1U;
        }
    }

    if (err_count == 0) {
        handle = (zip_t *)ops->zip_calloc(1, sizeof(zip_t));
        if (handle != NULL) {
            handle->ops = *ops;
        }
    }
    else {
        handle = NULL;
    }

    return handle;
}

zip_result_t zip_parse(zip_t *handle, uintptr_t end_point)
{
    zip_result_t result;
    zip_iterator_entry entries[] = {
        zip_search_eocd,
        zip_search_cdfh,
        zip_search_lf,
    };
    int32_t i;

    if (handle != NULL) {
        handle->end_point = end_point;
        for (i = 0; i < (sizeof(entries) / sizeof(zip_iterator_entry)); i++) {
            result = entries[i](handle);
            if (result != ZIP_OK) {
                break;
            }
        }
    }
    else {
        result = ZIP_FAIL;
    }

    return result;
}

zip_result_t zip_decompress(zip_t *handle)
{
    zip_result_t result;

    if (handle != NULL) {
        result = ZIP_OK;
    }
    else {
        result = ZIP_FAIL;
    }

    return result;
}

void zip_deinitialize(zip_t *handle)
{
}
