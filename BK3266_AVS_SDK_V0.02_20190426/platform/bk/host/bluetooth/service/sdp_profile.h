/* Modifications by Jungo Ltd. Copyright (c) 2012 Jungo Ltd. All Rights reserved */
/*
 * profile.h
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

#ifndef _SDP_PROFILE_H_
#define _SDP_PROFILE_H_

/*
 * Attribute descriptor
 */

typedef int32_t (profile_attr_create_t)(uint8_t *buf,
    uint8_t const * const eob, uint8_t const *data, uint32_t datalen);

typedef bool_t (profile_data_valid_t)(uint8_t const *data, uint32_t datalen);

typedef struct attr {
    uint16_t                    attr;    /* attribute id */
    profile_attr_create_t       *create; /* create attr value */
} attr_t;


/*
 * Profile descriptor
 */

typedef struct {
    uuid_t                      service_class;
    uint16_t                    version;
} profile_descriptor_t;

typedef struct profile {
#ifdef J_DEBUG
    const char                  *name;
#endif
    bt_svc_type_t               type;
    uuid_t                      *match_uuid;  /* profile uuid list */
    uint32_t                    usize;  /* profile uuid list size */
    uint16_t                    dsize;  /* profile data size */
    profile_data_valid_t        *valid; /* profile data validator */
    attr_t const * const        attrs;  /* supported attributes */
} profile_t;

bool_t profile_match_uuid(const profile_t *profile, const uuid_t *uuid);
const profile_t *profile_get_descriptor(bt_svc_type_t type);
profile_attr_create_t *profile_get_attr(const profile_t *profile,
    uint16_t attr);

profile_attr_create_t common_profile_create_service_record_handle;
profile_attr_create_t common_profile_create_service_class_id_list;
profile_attr_create_t common_profile_create_bluetooth_profile_descriptor_list;
profile_attr_create_t common_profile_create_language_base_attribute_id_list;
profile_attr_create_t common_profile_create_service_provider_name;
profile_attr_create_t common_profile_create_string8;
profile_attr_create_t common_profile_create_uint16;
profile_attr_create_t rfcomm_profile_create_protocol_descriptor_list;
profile_attr_create_t avdtp_profile_create_protocol_descriptor_list;
profile_attr_create_t avctp_profile_create_protocol_descriptor_list;
profile_attr_create_t avctp_profile_create_additional_protocol_descriptor_list;
profile_attr_create_t obex_profile_create_protocol_descriptor_list;
profile_attr_create_t obex_profile_create_supported_formats_list;
profile_attr_create_t bnep_profile_create_protocol_descriptor_list;

profile_attr_create_t hid_profile_create_protocol_descriptor_list;
profile_attr_create_t hid_additional_profile_create_protocol_descriptor_list;
profile_attr_create_t pnp_profile_create_protocol_descriptor_list;

profile_data_valid_t common_profile_server_channel_valid;
profile_data_valid_t obex_profile_data_valid;

#ifdef J_DEBUG
void profile_dump(const profile_t *profile, const char *file, int_t line);
#endif

#endif /* ndef _PROFILE_H_ */
