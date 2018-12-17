/*
*  (C) Copyright 2014-2016 Shenzhen South Silicon Valley microelectronics co.,limited
*
*  All Rights Reserved
*/
#ifndef _NETSTACK_H_
#define _NETSTACK_H_
#include <ssv_types.h>
#include <netstack_def.h>

struct netdev
{
    char name[26];
    u16 mtu;
    u32 ipaddr;
    u32 netmask;
    u32 gw;
    u32 flags;    
    unsigned char hwmac[6];        
};


typedef struct st_dhcps_info{
	/* start,end are in host order: we need to compare start <= ip <= end */
	u32 start_ip;              /* start address of leases, in host order */
	u32 end_ip;                /* end of leases, in host order */
	u32 max_leases;            /* maximum number of leases (including reserved addresses) */

    u32 subnet;
    u32 gw;
    u32 dns;

	u32 auto_time;             /* how long should udhcpd wait before writing a config file.
			                         * if this is zero, it will only write one on SIGUSR1 */
	u32 decline_time;          /* how long an address is reserved if a client returns a
			                         * decline message */
	u32 conflict_time;         /* how long an arp conflict offender is leased for */
	u32 offer_time;            /* how long an offered address is reserved */
	u32 max_lease_sec;         /* maximum lease time (host order) */
	u32 min_lease_sec;         /* minimum lease time a client can request */
}dhcps_info;

typedef struct st_dhcpdipmac
{
    u32 ip;
    u8 mac[6];
    u8 reserved[2];
}dhcpdipmac;

struct netstack_ip_addr {
  u32 addr;
};

typedef struct netstack_ip_addr netstack_ip_addr_t;

#define NS_OK           0   //Everything is fine
#define NS_NG           -1
#define NS_ERR_MEM      -2  //Out of memory
#define NS_ERR_ARG      -3  //Invalid arguement
#define NS_ERR_IMP      -4  //Not implement yet
#define NS_ERR_CALLER   -5  //Not define error, need to check

typedef int (*eth_input_fn)(void *dat, u32 len);
typedef int (*reg_fn)(eth_input_fn);
typedef void (*netdev_link_callback_t)(void *dat);
#define netstack_inet_addr(cp)         netstack_ipaddr_addr(cp)

// Transfer L2 packet to netstack
int netstack_input(void *data, u32 len);

// Transfer netstack packet to L2
int netstack_output(void* net_interface, void *data, u32 len);

//init netstack
int netstack_init(void *config);

//Add device with specific setting
int netdev_init(struct netdev * pdev, bool dft_dev, bool init_up);

//get hw mac
int netdev_getmacaddr(const char *ifname, u8 *macaddr);
//get ipinfo
int netdev_getipv4info(const char *ifname, u32 *ip, u32 *gw, u32 *netmask);
//set ipinfo
int netdev_setipv4info(const char *ifname, u32 ip, u32 gw, u32 netmask);

//get dns server
//int netdev_get_ipv4dnsaddr(const char *ifname, u32 *dnsserver);
//set dns server
//int netdev_set_ipv4dnsaddr(const char *ifname, const u32 *dnsserver);

//get interface status
int netdev_check_ifup(const char *ifname);

//set interface up
int netdev_l3_if_up(const char *ifname);
//set interface down
int netdev_l3_if_down(const char *ifname);
//interface link up cb
void netdev_link_up_cb(void *ifname);
//interface link down cb
void netdev_link_down_cb(void *ifname);
//get all netdev
u32 netdev_getallnetdev(struct netdev * pdev, u32 num);


//set dhcp client on dev
int dhcpc_wrapper_set(const char *ifname, const bool enable);

// UDP operation

int netstack_udp_send(void* data, u32 len, u32 srcip, u16 srcport, u32 dstip, u16 dstport, s16 rptsndtimes);
int netstack_tcp_send(void* data, u32 len, u32 srcip, u16 srcport, u32 dstip, u16 dstport);
int netstack_dhcps_info_set(dhcps_info *if_dhcps, dhcps_info *des_if_dhcps);
int netstack_udhcpd_start(void);
int netstack_udhcpd_stop(void);
int netstack_dhcp_ipmac_get(dhcpdipmac *ipmac, int *size_count);
int netstack_find_ip_in_arp_table(u8 * mac,netstack_ip_addr_t *ipaddr);
int netstack_etharp_unicast (u8 *dst_mac, netstack_ip_addr_t *ipaddr);

u32 netstack_ipaddr_addr(const char *cp);
char *netstack_inet_ntoa(netstack_ip_addr_t addr);

u16 netstack_ip4_addr1_16(u32* ipaddr);
u16 netstack_ip4_addr2_16(u32* ipaddr);
u16 netstack_ip4_addr3_16(u32* ipaddr);
u16 netstack_ip4_addr4_16(u32* ipaddr);

#endif //#ifndef _NETSTACK_H_
