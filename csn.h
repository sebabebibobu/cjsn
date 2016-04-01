#ifndef CSN_H
#define CSN_H

#define CSN_LAYER_COUNT 10 //MAX Layer

struct csn_ctx_s;

typedef bool (*csn_reader)(struct csn_ctx_s *ctx, void *data, size_t limit);
typedef size_t (*csn_writer)(struct csn_ctx_s *ctx, const void *data, size_t count);

enum {
    CSN_TYPE_NULL,
    CSN_TYPE_STRING,
    CSN_TYPE_NUMBER,
    CSN_TYPE_OBJECT,
    CSN_TYPE_ARRAY,
    CSN_TYPE_BOOL
};

union csn_object_data_u {
    bool boolean;
    //uint8_t u8;
    //uint16_t u16;
    uint32_t u32;
    //uint64_t u64;
    //int8_t s8;
    //int16_t s16;
    int32_t s32;
    //int64_t s64;
    float flt;
    //double dbl;
    uint32_t array_size;
    uint32_t map_size;
    uint32_t str_size;
    uint32_t bin_size;
    //csn_ext_t ext;
};

typedef struct csn_ctx_layer_s {
    uint8_t type;
    uint32_t size;
    uint32_t index;
} csn_cts_layer_t;

typedef struct csn_ctx_s {
  uint8_t error;
  void *buf;
  csn_reader read;
  csn_writer write;
  uint32_t layer_count;
  csn_cts_layer_t layer[CSN_LAYER_COUNT];
} csn_ctx_t;

typedef struct csn_object_s {
  uint8_t type;
  union csn_object_data_u as;
} csn_object_t;

#ifdef __cplusplus
extern "C" {
#endif

/*
 * ============================================================================
 * === Main API
 * ============================================================================
 */

/* Initializes a csn context */
void csn_init(csn_ctx_t *ctx, void *buf, csn_reader read, csn_writer write);

/* Returns csn's version */
uint32_t csn_version(void);

/* Returns the MessagePack version employed by csn */
uint32_t csn_mp_version(void);

/* Returns a string description of a csn context's error */
const char *csn_strerror(csn_ctx_t *ctx);

/* Writes an array to the backend. */
bool csn_write_array(csn_ctx_t *ctx, uint32_t size);
bool csn_write_object(csn_ctx_t *ctx, uint32_t size);

/* Writes a signed integer to the backend */
bool csn_write_integer(csn_ctx_t *ctx, int32_t i);

/* Writes an unsigned integer to the backend */
bool csn_write_uinteger(csn_ctx_t *ctx, uint32_t u);

/*
 * Writes a string to the backend; according to the MessagePack spec, this must
 * be encoded using UTF-8, but CMP leaves that job up to the programmer.
 */
bool csn_write_str(csn_ctx_t *ctx, const char *data, uint32_t size);

#ifdef __cplusplus
} /* extern "C" */
#endif

/*
 * ============================================================================
 * === Backwards compatibility defines
 * ============================================================================
 */

#define cmp_write_int      cmp_write_integer
#define cmp_write_sint     cmp_write_integer
#define cmp_write_sinteger cmp_write_integer
#define cmp_write_uint     cmp_write_uinteger
#define cmp_read_sinteger  cmp_read_integer

#endif // CSN_H
