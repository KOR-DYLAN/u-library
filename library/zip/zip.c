#include <stdbool.h>
#include <string.h>
#include "library/zip/zip.h"
#include "zip_internal.h"

static inline uintptr_t zip_open(zip_ops_t *ops, uintptr_t end_point) {
    return ops->zip_open(end_point);
}
static inline void zip_close(zip_ops_t *ops, uintptr_t stream) {
    return ops->zip_close(stream);
}
static inline int32_t zip_tell(zip_ops_t *ops, uintptr_t stream) {
    return ops->zip_tell(stream);
}
static inline zip_result_t zip_seek(zip_ops_t *ops, uintptr_t stream, int32_t offset, int32_t origin) {
    return ops->zip_seek(stream, offset, origin);
}
static inline zip_result_t zip_read(zip_ops_t *ops, uintptr_t stream, uint8_t *buf, int32_t len) {
    return ops->zip_read(stream, buf, len);
}
static inline uintptr_t zip_malloc(zip_ops_t *ops, size_t size) {
    return ops->zip_malloc(size);
}
static inline uintptr_t zip_calloc(zip_ops_t *ops, size_t count, size_t size) {
    return ops->zip_calloc(count, size);
}
static inline void zip_free(zip_ops_t *ops, uintptr_t addr) {
    ops->zip_free(addr);
}

static zip_result_t zip_search_eocd(zip_t *handle);
static zip_result_t zip_search_cdfh(zip_t *handle);
static zip_result_t zip_search_lf(zip_t *handle);

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
    zip_search_entry entries[] = {
        zip_search_eocd,
        zip_search_cdfh,
        zip_search_lf,
    };
    int32_t i;

    if (handle != NULL) {
        handle->end_point = end_point;
        for (i = 0; i < (sizeof(entries) / sizeof(zip_search_entry)); i++) {
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

static zip_result_t zip_search_eocd(zip_t *handle)
{
    zip_result_t result;
    uint32_t signature;
    const int32_t eocd_unit_len = (const int32_t)ZIP_OFFSETOF(zip_eocd_t, commnet);
    int32_t i, cmp;
    bool is_exit_loop = false;

    handle->stream = zip_open(&handle->ops, handle->end_point);
    if (handle->stream != ZIP_NULL_STREAM) {
        result = zip_seek(&handle->ops, handle->stream, -eocd_unit_len, ZIP_SEEK_END);
        if (result != ZIP_FAIL) {
            for(i = 0; i < UINT16_MAX; i++) {
                result = zip_read(&handle->ops, handle->stream, (uint8_t *)&signature, sizeof(signature));
                if (result == ZIP_OK) {
                    if (signature == ZIP_EOCD_SIGNATURE) {
                        handle->eocd.signature = signature;
                        result = zip_read(&handle->ops, handle->stream, (uint8_t *)&handle->eocd.num_of_disk, eocd_unit_len - sizeof(signature));
                        if (result == ZIP_OK) {
                            if (0 < handle->eocd.comment_len) {
                                handle->eocd.commnet = (char *)zip_calloc(&handle->ops, handle->eocd.comment_len + 1, 1);
                                if (handle->eocd.commnet != NULL) {
                                    result = zip_read(&handle->ops, handle->stream, handle->eocd.commnet, handle->eocd.comment_len);
                                    if (result != ZIP_OK) {
                                        zip_free(&handle->ops, (uintptr_t)handle->eocd.commnet);
                                    }
                                }
                                else {
                                    result = ZIP_FAIL;
                                }
                            }
                        }

                        is_exit_loop = true;
                    }
                    else {
                        result = zip_seek(&handle->ops, handle->stream, -((int32_t)sizeof(signature) + 1), ZIP_SEEK_CUR);
                        if (result != ZIP_OK) {
                            is_exit_loop = true;
                        }
                    }
                }
                else {
                    is_exit_loop = true;
                }

                if (is_exit_loop == true) {
                    break;
                }
            }
        }
    }
    else {
        result = ZIP_FAIL;
    }

    return result;
}

static zip_result_t zip_search_cdfh(zip_t *handle)
{
    return ZIP_NOT_SUPPORTED;
}

static zip_result_t zip_search_lf(zip_t *handle)
{
    return ZIP_NOT_SUPPORTED;
}