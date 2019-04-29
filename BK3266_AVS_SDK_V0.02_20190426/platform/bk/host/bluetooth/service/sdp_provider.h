/* Modifications by Jungo Ltd. Copyright (c) 2012 Jungo Ltd. All Rights reserved */

/*
 * Copyright (c) 2006 Itronix Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of Itronix Inc. may not be used to endorse
 *    or promote products derived from this software without specific
 *    prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ITRONIX INC. ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL ITRONIX INC. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*
 * provider.h
 *
 * Copyright (c) 2004 Maksim Yevmenkin <m_evmenkin@yahoo.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _SDP_PROVIDER_H_
#define _SDP_PROVIDER_H_

#include <bt_sdp.h>
#include <uw_lists.h>

/*
 * Provider of service
 */

struct profile;

typedef struct provider {
    struct profile const    *profile;               /* profile */
    void                    *data;                  /* profile data */
    uint32_t                 handle;                /* record handle */
    btaddr_t                 btaddr;                /* provider's BDADDR */
    TAILQ_ENTRY(provider)    provider_next;         /* all providers */
} provider_t;

#define provider_match_btaddr(p, b) \
    (btaddr_any(b) || btaddr_any(&(p)->btaddr) || \
    btaddr_same(&(p)->btaddr, (b)))

result_t provider_register_sd(void);
provider_t *provider_register(struct profile const *profile,
    btaddr_t const *btaddr, uint8_t const *data, uint32_t datalen);

void provider_unregister_sd(void);
void provider_unregister(provider_t *provider);
result_t provider_update(provider_t *provider, uint8_t const *data,
    uint32_t datalen);
provider_t *provider_by_handle(uint32_t handle);
provider_t *provider_get_first(void);
provider_t *provider_get_next(provider_t *provider);
uint32_t provider_get_change_state(void);

bool_t provider_match_uuid(provider_t *provider, const uuid_t *ulist, int count);
int32_t server_prepare_attr_list(provider_t *const, uint8_t const *,
    uint8_t const *const, uint8_t *, uint8_t const *const);

void provider_populate_uuids(btaddr_t const *btaddr, uint16_t max_size,
    uint16_t *uuid16_list, uint16_t *size);

#ifdef J_DEBUG
void provider_dump(const provider_t *provider, const char *file, int_t line);
void provider_dump_all(const char *file, int_t line);
#endif

#endif /* ndef _PROVIDER_H_ */
