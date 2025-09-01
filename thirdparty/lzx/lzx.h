// Modified version of the code from Wine:
// https://gitlab.winehq.org/wine/wine/-/blob/fcc40a07909dc7626b6d1e2ec73f823d828a47e8/dlls/itss/lzx.h

/***************************************************************************
*                        lzx.h - LZX decompression routines               *
*                           -------------------                           *
*                                                                         *
*  maintainer: Jed Wing <jedwin@ugcs.caltech.edu>                         *
*  source:     modified lzx.c from cabextract v0.5                        *
*  notes:      This file was taken from cabextract v0.5, which was,       *
*              itself, a modified version of the lzx decompression code   *
*              from unlzx.                                                *
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.  Note that an exemption to this   *
 *   license has been granted by Stuart Caie for the purposes of           *
 *   distribution with chmlib.  This does not, to the best of my           *
 *   knowledge, constitute a change in the license of this (the LZX) code  *
 *   in general.                                                           *
 *                                                                         *
 ***************************************************************************/

#ifndef INCLUDED_LZX_H
#define INCLUDED_LZX_H

#ifdef __cplusplus
extern "C" {
#endif

/* return codes */
#define DECR_OK (0)
#define DECR_DATAFORMAT (1)
#define DECR_ILLEGALDATA (2)
#define DECR_NOMEMORY (3)

/* opaque state structure */
struct lzx_state;

/* create an lzx state object */
struct lzx_state* lzx_init(int window);

/* destroy an lzx state object */
void lzx_teardown(struct lzx_state* pState);

/* reset an lzx stream */
void lzx_reset(struct lzx_state* pState);

/* decompress an LZX compressed block */
int lzx_decompress(struct lzx_state* pState, const unsigned char* inpos, unsigned char* outpos, int inlen,
                   int outlen);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDED_LZX_H */
