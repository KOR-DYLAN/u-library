#include <stdbool.h>
#include <string.h>
#include "library/zip/zip.h"
#include "zip_internal.h"

#define EOCD_BASIC_LEN  (int32_t)__builtin_offsetof(zip_eocd_t, commnet)

static zip_result_t zip_eocd_open_stream(zip_t *handle);
static zip_result_t zip_eocd_jump_to_eof(zip_t *handle);
static zip_result_t zip_eocd_detect(zip_t *handle);
static zip_result_t     zip_eocd_read_and_verify_signature(zip_t *handle);
static zip_result_t     zip_eocd_read_header(zip_t *handle);
static zip_result_t     zip_eocd_next(zip_t *handle);

zip_result_t zip_search_eocd(zip_t *handle)
{
    zip_result_t result;
    zip_iterator_entry it[] = {
        zip_eocd_open_stream,
        zip_eocd_jump_to_eof,
        zip_eocd_detect,
    };
    int32_t i;

    for(i = 0; i < (sizeof(it) / sizeof(zip_iterator_entry)); i++) {
        result = it[i](handle);
        if (result != ZIP_OK) {
            break;
        }
    }

    return result;
}

static zip_result_t zip_eocd_open_stream(zip_t *handle)
{
    zip_result_t result;

    handle->stream = zip_open(handle, handle->end_point);
    if (handle->stream != ZIP_NULL_STREAM) {
        result = ZIP_OK;
    }
    else {
        result = ZIP_ERROR_STREAM_OPEN;
    }

    return result;
}

static zip_result_t zip_eocd_jump_to_eof(zip_t *handle)
{
    return zip_seek(handle, handle->stream, -EOCD_BASIC_LEN, ZIP_SEEK_END);
}

static zip_result_t zip_eocd_detect(zip_t *handle)
{
    zip_result_t result;
    int32_t i;
    bool is_exitloop = false;

    for(i = 0; i < UINT16_MAX; i++) {
        result = zip_eocd_read_and_verify_signature(handle);
        if (result == ZIP_ERROR_BAD_SIGNATURE) {
            result = zip_eocd_next(handle);
            if (result != ZIP_OK) {
                is_exitloop = true;
            }
        }
        else {
            if (result == ZIP_OK) {
                result = zip_eocd_read_header(handle);
            }
            is_exitloop = true;
        }

        if (is_exitloop == true) {
            break;
        }
    }

    return result;
}

static zip_result_t zip_eocd_read_and_verify_signature(zip_t *handle)
{
    zip_result_t result;
    uint32_t signature;

    result = zip_read(handle, handle->stream, (uint8_t *)&signature, sizeof(signature));
    if (result == ZIP_OK) {
        if (signature == ZIP_EOCD_SIGNATURE) {
            result = ZIP_OK;
        }
        else {
            result = ZIP_ERROR_BAD_SIGNATURE;
        }
    }

    return result;
}

static zip_result_t zip_eocd_read_header(zip_t *handle)
{
    zip_result_t result;

    result = zip_seek(handle, handle->stream, -((int32_t)sizeof(handle->eocd.signature)), ZIP_SEEK_CUR);
    if (result == ZIP_OK) {
        result = zip_read(handle, handle->stream, (uint8_t *)&handle->eocd, EOCD_BASIC_LEN);
        if (result == ZIP_OK) {
            if (0 < handle->eocd.comment_len) {
                handle->eocd.commnet = (char *)zip_calloc(handle, (size_t)handle->eocd.comment_len + 1U, 1U);
                if (handle->eocd.commnet != NULL) {
                    result = zip_read(handle, handle->stream, (uint8_t *)handle->eocd.commnet, (int32_t)handle->eocd.comment_len);
                    if (result != ZIP_OK) {
                        zip_free(handle, (uintptr_t)handle->eocd.commnet);
                    }
                }
                else {
                    result = ZIP_ERROR_MEMORY_ALLOC;
                }
            }
        }
    }

    return result;
}

static zip_result_t zip_eocd_next(zip_t *handle)
{
    return zip_seek(handle, handle->stream, -((int32_t)sizeof(handle->eocd.signature) + 1), ZIP_SEEK_CUR);
}
