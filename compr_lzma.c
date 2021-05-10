/*
 * JFFS2 -- Journalling Flash File System, Version 2.
 *
 * For licensing information, see the file 'LICENCE' in this directory.
 *
 * JFFS2 wrapper to the LZMA C SDK
 *
 */
#include <stdint.h>
#include <stdlib.h>
#include "LzmaDec.h"

#ifdef __KERNEL__
	static DEFINE_MUTEX(deflate_mutex);
#endif

#define LZMA_MALLOC malloc
#define LZMA_FREE free
static void *p_lzma_malloc(void *p, size_t size)
{
        if (size == 0)
                return NULL;

        return LZMA_MALLOC(size);
}

static void p_lzma_free(void *p, void *address)
{
        if (address != NULL)
                LZMA_FREE(address);
}

static ISzAlloc lzma_alloc = {p_lzma_malloc, p_lzma_free};
static Byte propsEncoded[LZMA_PROPS_SIZE];
static SizeT propsSize = sizeof(propsEncoded);


int jffs2_lzma_decompress(unsigned char *data_in, unsigned char *cpage_out,
				 uint32_t srclen, uint32_t* destlen)
{
	int ret;
	SizeT dl = (SizeT)destlen;
	SizeT sl = (SizeT)srclen;
	ELzmaStatus status;
	
	ret = LzmaDecode(cpage_out, &dl, data_in, &sl, propsEncoded,
		propsSize, LZMA_FINISH_ANY, &status, &lzma_alloc);

	if (ret != SZ_OK || status == LZMA_STATUS_NOT_FINISHED || dl != (SizeT)destlen)
		return -1;

	return 0;
}

