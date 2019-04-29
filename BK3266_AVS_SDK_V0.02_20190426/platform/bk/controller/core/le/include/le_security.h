#ifndef _CEVA_LE_SECURITY__
#define _CEVA_LE_SECURITY__


/*********************************************************************
 * MODULE NAME:     le_security.h
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:		The header file for security.
 * MAINTAINER:      Nicola Lenihan
 * DATE:            05-01-12        
 * SOURCE CONTROL:  
 *
 * LICENSE:
 *     This source code is copyright (c) 1999-2012 Ceva Inc.
 *     All rights reserved.
 *
 *********************************************************************/
#define LE_LINK_KEY_REQUEST_TIMEOUT  0xFA00   /*20.00 Seconds (BT_CLOCK_TICKS_PER_SECOND * 20) */
#define LE_SECURITY_OPTIMIZATION 1

void LEsecurity_Encrypt(t_p_pdu p_pdu, u_int8 *encrypted_data);
void LEsecurity_Rand(u_int8 *random_number_for_host);
t_error LEsecurity_Start_Encryption(u_int16 handle, u_int8* p_rand,u_int16 encry_div,u_int8* p_ltk );
t_error LEsecurity_Long_Term_Key_Request_Reply(t_LE_Connection* p_connection,t_p_pdu p_pdu);
t_error LEsecurity_Long_Term_Key_Request_Negative_Reply(t_LE_Connection* p_connection);
void LEsecurity_HW_aes_encrypt_128(const u_int8* plaintext_data, u_int8* inv_encrypted_data,
								   const u_int8* key,            u_int8* output_key);		
void LEsecurity_Generate_LTK_Request(t_deviceIndex dev_index);

void LEsecurity_Long_Term_Key_Request_Timeout(void);
#if (PRH_BS_CFG_SYS_LE_GAP_INCLUDED==1)
t_error LEsecurity_GAP_Start_Encryption(t_LE_Connection* p_connection,u_int8* random,u_int16 encry_div,u_int8* ltk);
#endif

#if (PRH_BS_CFG_SYS_LE_BT41_AUTHENTICATED_PAYLOAD_TIMEOUT == 1)
void LEsecurity_Authenticated_Payload_Timeout_Expired(t_LE_Connection* p_connection);
t_error LEsecurity_Write_Authenticated_Payload_Timeout(t_LE_Connection* p_connection,u_int16 timeout);
#endif

#if (PRH_BS_CFG_SYS_INLINE_AES_SUPPORTED==1)
void LEsecurity_Write_IV_and_Key_To_HW(t_LE_Connection* p_connection);
#endif

#define	LE_DATA_NOT_ENCRYPTED      0x01
#define	LE_DATA_NOT_DECRYPTED      0x02
#define	LE_DATA_ENCRYPT_PENDING    0x04
#define	LE_DATA_ENCRYPTED          0x08
#define	LE_DATA_DECRYPT_PENDING    0x10
#define	LE_DATA_DECRYPTED          0x20

#endif

