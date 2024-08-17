#ifndef __ZIP_INTERNAL__H__
#define __ZIP_INTERNAL__H__

#include <stdint.h>

#define ZIP_EOCD_SIGNATURE          (0x06054b50)
#define ZIP_CDFH_SIGNATURE          (0x02014b50)
#define ZIP_LF_SIGNATURE            (0x04034b50)

#define ZIP_OFFSETOF(TYPE, MEMBER)  ((uintptr_t)&((TYPE *)0)->MEMBER)

/* End of central directory record (EOCD) */
typedef struct {
    uint32_t signature;         /* End of central directory signature = 0x06054b50 */
    uint16_t num_of_disk;       /* Number of this disk (or 0xffff for ZIP64) */
    uint16_t cd_start_disk;     /* Disk where central directory starts (or 0xffff for ZIP64) */
    uint16_t num_of_cd;         /* Number of central directory records on this disk (or 0xffff for ZIP64) */
    uint16_t total_num_of_cd;   /* Total number of central directory records (or 0xffff for ZIP64) */
    uint32_t size_of_cd;        /* Size of central directory (bytes) (or 0xffffffff for ZIP64) */
    uint32_t cd_start_off;      /* Offset of start of central directory, relative to start of archive (or 0xffffffff for ZIP64) */
    uint16_t comment_len;       /* Comment length (n) */
    char *commnet;              /* Comment */
} __attribute__((packed)) zip_eocd_t;

/* Central directory file header (CDFH) */
typedef struct {
    uint32_t signature;         /* Central directory file header signature = 0x02014b50 */
    uint16_t zip_version;       /* Version made by */
    uint16_t unzip_version;     /* Version needed to extract (minimum) */
    uint16_t flags;             /* General purpose bit flag */
    uint16_t method;            /* Compression method */
    uint16_t time;              /* File last modification time */
    uint16_t date;              /* File last modification date */
    uint32_t crc32;             /* CRC-32 of uncompressed data */
    uint32_t comp_size;         /* Compressed size (or 0xffffffff for ZIP64) */
    uint32_t decomp_size;       /* Uncompressed size (or 0xffffffff for ZIP64) */
    uint16_t filename_len;      /* File name length (n) */
    uint16_t extra_field_len;   /* Extra field length (m) */
    uint16_t comment_len;       /* File comment length (k) */
    uint16_t disk_num;          /* Disk number where file starts (or 0xffff for ZIP64) */
    uint16_t internal_attr;     /* Internal file attributes */
    uint32_t external_attr;     /* External file attributes */
    uint32_t lf_off;            /* Relative offset of local file header (or 0xffffffff for ZIP64). 
                                   This is the number of bytes between the start of the first disk on which the file occurs, and the start of the local file header.
                                   This allows software reading the central directory to locate the position of the file inside the ZIP file. */
    char *name;                 /* File name */
    uintptr_t extra_field;      /* Extra field */
    char *comment;              /* File comment */
} __attribute__((packed)) zip_cdfh_t;

/* Local file header */
typedef struct {
    uint32_t signature;         /* Local file header signature = 0x04034b50 (PK♥♦ or "PK\3\4") */
    uint16_t unzip_version;     /* Version needed to extract (minimum) */
    uint16_t flags;             /* General purpose bit flag */
    uint16_t method;            /* Compression method; e.g. none = 0, DEFLATE = 8 (or "\0x08\0x00") */
    uint16_t time;              /* File last modification time */
    uint16_t date;              /* File last modification date */
    uint32_t crc32;             /* CRC-32 of uncompressed data */
    uint32_t comp_size;         /* Compressed size (or 0xffffffff for ZIP64) */
    uint32_t decomp_size;       /* Uncompressed size (or 0xffffffff for ZIP64) */
    uint16_t filename_len;      /* File name length (n) */
    uint16_t extra_field_len;   /* Extra field length (m) */
    char *name;                 /* File name */
    uintptr_t extra_field;      /* Extra field */
} __attribute__((packed)) zip_lf_t;

struct zip_structure {
    zip_ops_t ops;
    uintptr_t end_point;
    uintptr_t stream;
    zip_eocd_t eocd;
    zip_cdfh_t *cdfh;
    zip_lf_t *lf;
};

typedef zip_result_t(*zip_search_entry)(zip_t *handle);

#endif  //!__ZIP_INTERNAL__H__
