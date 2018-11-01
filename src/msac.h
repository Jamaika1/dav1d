/*
 * Copyright © 2018, VideoLAN and dav1d authors
 * Copyright © 2018, Two Orioles, LLC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __DAV1D_SRC_MSAC_H__
#define __DAV1D_SRC_MSAC_H__

#include <stdint.h>
#include <stdlib.h>

/* Using uint32_t should be faster on 32 bit systems, in theory, maybe */
typedef uint64_t ec_win;

typedef struct MsacContext {
    const uint8_t *buf_pos;
    const uint8_t *buf_end;
    ec_win dif;
    uint16_t rng;
    int cnt;
} MsacContext;

#define EC_PROB_SHIFT 6
#define EC_BOOL_EPROB 256

void msac_init(MsacContext *c, const uint8_t *data, size_t sz);
unsigned msac_decode_symbol(MsacContext *s, const uint16_t *cdf,
                            const unsigned n_symbols);
unsigned msac_decode_bool(MsacContext *s, unsigned f);
unsigned msac_decode_bools(MsacContext *c, unsigned l);
int msac_decode_subexp(MsacContext *c, int ref, int n, unsigned k);
int msac_decode_uniform(MsacContext *c, unsigned n);
void msac_update_cdf(uint16_t *cdf, unsigned val, unsigned n_symbols);

static inline unsigned msac_decode_symbol_adapt(MsacContext *const c,
                                                uint16_t *const cdf,
                                                const unsigned n_symbols)
{
    const unsigned val = msac_decode_symbol(c, cdf, n_symbols);
    msac_update_cdf(cdf, val, n_symbols);
    return val;
}

static inline unsigned msac_decode_bool_adapt(MsacContext *const c,
                                              uint16_t *const cdf)
{
    const unsigned bit = msac_decode_bool(c, *cdf >> EC_PROB_SHIFT);
    uint16_t bak_cdf[3] = { cdf[0], 0, cdf[1] };
    msac_update_cdf(bak_cdf, bit, 2);
    cdf[0] = bak_cdf[0];
    cdf[1] = bak_cdf[2];
    return bit;
}

#endif /* __DAV1D_SRC_MSAC_H__ */
