#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "rex-header.h"
#include "util.h"

struct rex_header* rex_header_create ()
{
    struct rex_header *header = malloc(sizeof (struct rex_header));
    header->version = REX_FILE_VERSION;
    header->crc = 0;
    header->nr_datablocks = 0;
    header->start_addr = 0;
    header->sz_all_datablocks = 0;

    memcpy (header->magic, REX_FILE_MAGIC, sizeof (REX_FILE_MAGIC));
    memset (header->reserved, 0, 42);
    return header;
}

uint8_t* rex_header_read (uint8_t *buf, struct rex_header *header)
{
    MEM_CHECK (buf);
    uint8_t *start = buf;

    rexcpy(header->magic, buf, 4);
    rexcpy(&header->version, buf, sizeof(uint16_t));
    rexcpy(&header->crc, buf, sizeof(uint32_t));
    rexcpy(&header->nr_datablocks, buf, sizeof(uint16_t));
    rexcpy(&header->start_addr, buf,sizeof(uint16_t));
    rexcpy(&header->sz_all_datablocks,buf, sizeof(uint64_t));
    rexcpy(header->reserved, buf, 42);

    if (strncmp (header->magic, "REX1", 4) != 0)
        die("This is not a valid REX file");

    // NOTE: we skip the coordinate system block because it is not used
    return start + header->start_addr;
}

int rex_header_write (FILE *fp, struct rex_header *header)
{
    FP_CHECK (fp)
    rewind (fp);

    header->start_addr = 86; // 86 because of fixed CSB below!

    rex_write(header->magic, 4, 1, fp);
    rex_write(&header->version, sizeof(uint16_t), 1, fp);
    rex_write(&header->crc, sizeof(uint32_t), 1, fp);
    rex_write(&header->nr_datablocks, sizeof(uint16_t), 1, fp);
    rex_write(&header->start_addr, sizeof(uint16_t), 1, fp);
    rex_write(&header->sz_all_datablocks, sizeof(uint64_t), 1, fp);
    rex_write(header->reserved, 42, 1, fp);

    // dummy CSB
    uint32_t srid = 0;
    uint16_t sz = 4;
    char *name = "EPSG";
    float xofs = 0.0f;
    float yofs = 0.0f;
    float zofs = 0.0f;
    rex_write(&srid, sizeof(uint32_t), 1, fp);
    rex_write(&sz, sizeof(uint16_t), 1, fp);
    rex_write(name, sz, 1, fp);
    rex_write(&xofs, sizeof(float), 1, fp);
    rex_write(&yofs, sizeof(float), 1, fp);
    rex_write(&zofs, sizeof(float), 1, fp);

    return REX_OK;
}
