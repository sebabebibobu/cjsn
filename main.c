#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "csn.h"

static bool read_bytes(void *data, size_t sz, FILE *fh) {
    return fread(data, sizeof(uint8_t), sz, fh) == (sz * sizeof(uint8_t));
}

static bool file_reader(csn_ctx_t *ctx, void *data, size_t limit) {
    return read_bytes(data, limit, (FILE *)ctx->buf);
}

static size_t file_writer(csn_ctx_t *ctx, const void *data, size_t count) {
    return fwrite(data, sizeof(uint8_t), count, stdout);
    return fwrite(data, sizeof(uint8_t), count, (FILE *)ctx->buf);
}

void error_and_exit(const char *msg) {
    fprintf(stderr, "%s\n\n", msg);
    exit(EXIT_FAILURE);
}

int main(void)
{
    FILE *fh = NULL;
    csn_ctx_t csn;
    int li = 0;
    int li_array = 0;
    int li_object = 0;

    fh = fopen("csn_data.dat", "w+b");

    if (fh == NULL) {
        error_and_exit("Error opening data.dat");
    }

    csn_init(&csn, fh, file_reader, file_writer);

    li_array = 2;
    if (!csn_write_array(&csn, li_array)) {
        error_and_exit(csn_strerror(&csn));
    }

    for (li = 0; li < li_array; li++) {
        if (!csn_write_integer(&csn, li)) {
            error_and_exit(csn_strerror(&csn));
        }
    }

    li_object = 2;
    if (!csn_write_object(&csn, li_object)) {
        error_and_exit(csn_strerror(&csn));
    }
    for (li = 0; li < li_object; li++) {
        if (!csn_write_str(&csn, "hallo", strlen("hallo"))) {
            error_and_exit(csn_strerror(&csn));
        }
        if (!csn_write_integer(&csn, li)) {
            error_and_exit(csn_strerror(&csn));
        }
    }
    printf("\n");

    fclose(fh);
    return 0;
}
