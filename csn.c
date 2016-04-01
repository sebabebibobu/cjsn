#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

//FIXME remove limis.h only for tests!
#include <limits.h>

#include "csn.h"

enum {
    ERROR_NONE,
    STR_DATA_LENGTH_TOO_LONG_ERROR,
    BIN_DATA_LENGTH_TOO_LONG_ERROR,
    ARRAY_LENGTH_TOO_LONG_ERROR,
    MAP_LENGTH_TOO_LONG_ERROR,
    INPUT_VALUE_TOO_LARGE_ERROR,
    FIXED_VALUE_WRITING_ERROR,
    TYPE_MARKER_READING_ERROR,
    TYPE_MARKER_WRITING_ERROR,
    DATA_READING_ERROR,
    DATA_WRITING_ERROR,
    EXT_TYPE_READING_ERROR,
    EXT_TYPE_WRITING_ERROR,
    INVALID_TYPE_ERROR,
    LENGTH_READING_ERROR,
    LENGTH_WRITING_ERROR,
    LAYER_COUNT_ERROR,
    ERROR_MAX
};

const char *cmp_error_messages[ERROR_MAX + 1] = {
    "No Error",
    "Specified string data length is too long (> 0xFFFFFFFF)",
    "Specified binary data length is too long (> 0xFFFFFFFF)",
    "Specified array length is too long (> 0xFFFFFFFF)",
    "Specified map length is too long (> 0xFFFFFFFF)",
    "Input value is too large",
    "Error writing fixed value",
    "Error reading type marker",
    "Error writing type marker",
    "Error reading packed data",
    "Error writing packed data",
    "Error reading ext type",
    "Error writing ext type",
    "Invalid type",
    "Error reading size",
    "Error writing size",
    "Can't write value (Neither Object nor Array is opened)",
    "Max Error"
};

void csn_init(csn_ctx_t *ctx, void *buf, csn_reader read, csn_writer write)
{
    memset(ctx, 0, sizeof(csn_ctx_t));

    ctx->error = ERROR_NONE;
    ctx->buf = buf;
    ctx->read = read;
    ctx->write = write;
}

uint32_t csn_version()
{

}

uint32_t csn_mp_version()
{

}

const char *csn_strerror(csn_ctx_t *ctx)
{
    if (ctx->error > ERROR_NONE && ctx->error < ERROR_MAX)
        return cmp_error_messages[ctx->error];

    return "";
}

static bool csn_write_list(csn_ctx_t *ctx, uint32_t size, uint8_t type)
{
    csn_cts_layer_t *layer = NULL;
    if (type == CSN_TYPE_ARRAY) {
        if (!ctx->write(ctx, "[", 1)) {
            ctx->error = DATA_WRITING_ERROR;
            return false;
        }
    } else { // CSN_TYPE_OBJECT
        if (!ctx->write(ctx, "{", 1)) {
            ctx->error = DATA_WRITING_ERROR;
            return false;
        }
    }

    layer = &ctx->layer[ctx->layer_count];
    layer->index = 0;
    layer->type = type;
    if (type == CSN_TYPE_ARRAY) {
        layer->size = size;
    } else { // CSN_TYPE_OBJECT
        layer->size = 2 * size;
    }

    ctx->layer_count++;
    return true;
}

static bool csn_write_marker(csn_ctx_t *ctx)
{
    csn_cts_layer_t *layer = NULL;
    if (ctx->layer_count > 0) {
        layer = &ctx->layer[ctx->layer_count - 1];
        if (layer->index < layer->size - 1) {
            layer->index++;

            if (layer->type == CSN_TYPE_OBJECT && ((layer->index % 2) != 0)) {
                return ctx->write(ctx, ":", 1);
            }

            return ctx->write(ctx, ",", 1);
        } else {
            layer->index = 0;
            layer->size = 0;
            ctx->layer_count--;
            if (layer->type == CSN_TYPE_ARRAY) {
                return ctx->write(ctx, "]", 1);
            } else { // CSN_TYPE_OBJECT
                return ctx->write(ctx, "}", 1);
            }
        }
    }

    return true;
}

bool csn_write_array(csn_ctx_t *ctx, uint32_t size)
{
    csn_write_list(ctx, size, CSN_TYPE_ARRAY);
}

bool csn_write_object(csn_ctx_t *ctx, uint32_t size)
{
    csn_write_list(ctx, size, CSN_TYPE_OBJECT);
}

bool csn_write_integer(csn_ctx_t *ctx, int32_t i)
{
    char buf[12]; //INT_MIN == -2147483648 => 11 + \0 chars!!!
    sprintf(buf, "%d", i);
    if (ctx->layer_count == 0) {
        ctx->error = LAYER_COUNT_ERROR;
        return false;
    }

    if (!ctx->write(ctx, &buf, strlen(buf))) {
        ctx->error = DATA_WRITING_ERROR;
        return false;
    }

    return csn_write_marker(ctx);
}

bool csn_write_uinteger(csn_ctx_t *ctx, uint32_t u)
{

}

bool csn_write_str(csn_ctx_t *ctx, const char *data, uint32_t size)
{
    if (!ctx->write(ctx, "\"", 1)) {
        ctx->error = DATA_WRITING_ERROR;
        return false;
    }
    if (!ctx->write(ctx, data, size)) {
        ctx->error = DATA_WRITING_ERROR;
        return false;
    }
    if (!ctx->write(ctx, "\"", 1)) {
        ctx->error = DATA_WRITING_ERROR;
        return false;
    }

    return csn_write_marker(ctx);
}
