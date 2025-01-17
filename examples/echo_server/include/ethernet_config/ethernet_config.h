/*
 * Copyright 2024, UNSW
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <microkit.h>
#include <sddf/util/string.h>
#include <sddf/network/queue.h>
#include <sddf/util/util.h>

#define NUM_NETWORK_CLIENTS 2

#define NET_CLI0_NAME "client0"
#define NET_CLI1_NAME "client1"
#define NET_COPY0_NAME "copy0"
#define NET_COPY1_NAME "copy1"
#define NET_VIRT_RX_NAME "net_virt_rx"
#define NET_VIRT_TX_NAME "net_virt_tx"
#define NET_DRIVER_NAME "eth"
#define NET_TIMER_NAME "timer"

#define NET_DATA_REGION_SIZE                    0x200000
#define NET_HW_REGION_SIZE                      0x10000

#if defined(CONFIG_PLAT_IMX8MM_EVK)
#define MAC_ADDR_CLI0                       0x525401000001
#define MAC_ADDR_CLI1                       0x525401000002
#elif defined(CONFIG_PLAT_ODROIDC4)
#define MAC_ADDR_CLI0                       0x525401000003
#define MAC_ADDR_CLI1                       0x525401000004
#elif defined(CONFIG_PLAT_MAAXBOARD)
#define MAC_ADDR_CLI0                       0x525401000005
#define MAC_ADDR_CLI1                       0x525401000006
#elif defined(CONFIG_PLAT_QEMU_ARM_VIRT)
#define MAC_ADDR_CLI0                       0x525401000007
#define MAC_ADDR_CLI1                       0x525401000008
#elif defined(CONFIG_PLAT_IMX8MP_EVK)
#define MAC_ADDR_CLI0                       0x525401000009
#define MAC_ADDR_CLI1                       0x52540100000A
#else
#error "Must define MAC addresses for clients in ethernet config"
#endif

#define NET_TX_QUEUE_SIZE_CLI0                   512
#define NET_TX_QUEUE_SIZE_CLI1                   512
#define NET_TX_QUEUE_SIZE_DRIV                   (NET_TX_QUEUE_SIZE_CLI0 + NET_TX_QUEUE_SIZE_CLI1)

#define NET_TX_DATA_REGION_SIZE_CLI0            NET_DATA_REGION_SIZE
#define NET_TX_DATA_REGION_SIZE_CLI1            NET_DATA_REGION_SIZE

_Static_assert(NET_TX_DATA_REGION_SIZE_CLI0 >= NET_TX_QUEUE_SIZE_CLI0 *NET_BUFFER_SIZE,
               "Client0 TX data region size must fit Client0 TX buffers");
_Static_assert(NET_TX_DATA_REGION_SIZE_CLI1 >= NET_TX_QUEUE_SIZE_CLI1 *NET_BUFFER_SIZE,
               "Client1 TX data region size must fit Client1 TX buffers");

#define NET_RX_QUEUE_SIZE_DRIV                   512
#define NET_RX_QUEUE_SIZE_CLI0                   512
#define NET_RX_QUEUE_SIZE_CLI1                   512
#define NET_MAX_CLIENT_QUEUE_SIZE                MAX(NET_RX_QUEUE_SIZE_CLI0, NET_RX_QUEUE_SIZE_CLI1)
#define NET_RX_QUEUE_SIZE_COPY0                  NET_RX_QUEUE_SIZE_DRIV
#define NET_RX_QUEUE_SIZE_COPY1                  NET_RX_QUEUE_SIZE_DRIV

#define NET_RX_DATA_REGION_SIZE_DRIV            NET_DATA_REGION_SIZE
#define NET_RX_DATA_REGION_SIZE_CLI0            NET_DATA_REGION_SIZE
#define NET_RX_DATA_REGION_SIZE_CLI1            NET_DATA_REGION_SIZE

_Static_assert(NET_RX_DATA_REGION_SIZE_DRIV >= NET_RX_QUEUE_SIZE_DRIV *NET_BUFFER_SIZE,
               "Driver RX data region size must fit Driver RX buffers");
_Static_assert(NET_RX_DATA_REGION_SIZE_CLI0 >= NET_RX_QUEUE_SIZE_CLI0 *NET_BUFFER_SIZE,
               "Client0 RX data region size must fit Client0 RX buffers");
_Static_assert(NET_RX_DATA_REGION_SIZE_CLI1 >= NET_RX_QUEUE_SIZE_CLI1 *NET_BUFFER_SIZE,
               "Client1 RX data region size must fit Client1 RX buffers");

#define NET_MAX_QUEUE_SIZE MAX(NET_TX_QUEUE_SIZE_DRIV, MAX(NET_RX_QUEUE_SIZE_DRIV, MAX(NET_RX_QUEUE_SIZE_CLI0, NET_RX_QUEUE_SIZE_CLI1)))
_Static_assert(NET_TX_QUEUE_SIZE_DRIV >= NET_TX_QUEUE_SIZE_CLI0 + NET_TX_QUEUE_SIZE_CLI1,
               "Driver TX queue must have capacity to fit all of client's TX buffers.");
_Static_assert(NET_RX_QUEUE_SIZE_COPY0 >= NET_RX_QUEUE_SIZE_DRIV,
               "Copy0 queues must have capacity to fit all RX buffers.");
_Static_assert(NET_RX_QUEUE_SIZE_COPY1 >= NET_RX_QUEUE_SIZE_DRIV,
               "Copy1 queues must have capacity to fit all RX buffers.");
_Static_assert(sizeof(net_queue_t) + NET_MAX_QUEUE_SIZE *sizeof(net_buff_desc_t) <= NET_DATA_REGION_SIZE,
               "net_queue_t must fit into a single data region.");

static inline uint64_t net_cli_mac_addr(char *pd_name)
{
    if (!sddf_strcmp(pd_name, NET_CLI0_NAME)) {
        return MAC_ADDR_CLI0;
    } else if (!sddf_strcmp(pd_name, NET_CLI1_NAME)) {
        return MAC_ADDR_CLI1;
    }

    return 0;
}

static inline void net_virt_mac_addrs(char *pd_name, uint64_t macs[NUM_NETWORK_CLIENTS])
{
    if (!sddf_strcmp(pd_name, NET_VIRT_RX_NAME)) {
        macs[0] = MAC_ADDR_CLI0;
        macs[1] = MAC_ADDR_CLI1;
    }
}

static inline void net_cli_queue_size(char *pd_name, size_t *rx_queue_size, size_t *tx_queue_size)
{
    if (!sddf_strcmp(pd_name, NET_CLI0_NAME)) {
        *rx_queue_size = NET_RX_QUEUE_SIZE_CLI0;
        *tx_queue_size = NET_TX_QUEUE_SIZE_CLI0;
    } else if (!sddf_strcmp(pd_name, NET_CLI1_NAME)) {
        *rx_queue_size = NET_RX_QUEUE_SIZE_CLI1;
        *tx_queue_size = NET_TX_QUEUE_SIZE_CLI1;
    }
}

static inline void net_copy_queue_size(char *pd_name, size_t *cli_queue_size, size_t *virt_queue_size)
{
    if (!sddf_strcmp(pd_name, NET_COPY0_NAME)) {
        *cli_queue_size = NET_RX_QUEUE_SIZE_CLI0;
        *virt_queue_size = NET_RX_QUEUE_SIZE_COPY0;
    } else if (!sddf_strcmp(pd_name, NET_COPY1_NAME)) {
        *cli_queue_size = NET_RX_QUEUE_SIZE_CLI1;
        *virt_queue_size = NET_RX_QUEUE_SIZE_COPY1;
    }
}

typedef struct net_queue_info {
    net_queue_t *free;
    net_queue_t *active;
    size_t size;
} net_queue_info_t;

static inline void net_virt_queue_info(char *pd_name, net_queue_t *cli0_free, net_queue_t *cli0_active,
                                       net_queue_info_t ret[NUM_NETWORK_CLIENTS])
{
    if (!sddf_strcmp(pd_name, NET_VIRT_RX_NAME)) {
        ret[0] = (net_queue_info_t) {
            .free = cli0_free, .active = cli0_active, .size = NET_RX_QUEUE_SIZE_COPY0
        };
        ret[1] = (net_queue_info_t) {
            .free = (net_queue_t *)((uintptr_t)cli0_free + 2 * NET_DATA_REGION_SIZE),
            .active = (net_queue_t *)((uintptr_t)cli0_active + 2 * NET_DATA_REGION_SIZE),
            .size = NET_RX_QUEUE_SIZE_COPY1
        };
    } else if (!sddf_strcmp(pd_name, NET_VIRT_TX_NAME)) {
        ret[0] = (net_queue_info_t) {
            .free = cli0_free, .active = cli0_active, .size = NET_TX_QUEUE_SIZE_CLI0
        };
        ret[1] = (net_queue_info_t) {
            .free = (net_queue_t *)((uintptr_t)cli0_free + 2 * NET_DATA_REGION_SIZE),
            .active = (net_queue_t *)((uintptr_t)cli0_active + 2 * NET_DATA_REGION_SIZE),
            .size = NET_TX_QUEUE_SIZE_CLI1
        };
    }
}

static inline void net_mem_region_vaddr(char *pd_name, uintptr_t mem_regions[NUM_NETWORK_CLIENTS],
                                        uintptr_t start_region)
{
    if (!sddf_strcmp(pd_name, NET_VIRT_TX_NAME)) {
        mem_regions[0] = start_region;
        mem_regions[1] = start_region + NET_DATA_REGION_SIZE;
    }
}
