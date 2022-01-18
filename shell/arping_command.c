/*
 * arping2 - internal stack version
 *         It is obsolete.
 */
#include "lib.h"
#include "arp.h"
#include "ether.h"

extern unsigned int net_debug;

static unsigned int ipaddr;

static void usage(void)
{
    printf(
        "Usage: arping ipaddr"
    );
}

static int parse_args(int argc, char **argv)
{
    argc -= 1;
    argv += 1;
    arpingdbg("%d %s", argc, *argv);
    if (str2ip(*argv, &ipaddr) < 0) {
        printf("bad ip address %s\n", *argv);
        return -2;
    }
    arpingdbg("ip: %d", ipaddr);
    return 0;
}

static void send_packet(void)
{
    struct arpentry *ae;
    ae = arp_lookup(ETH_P_IP, ipaddr);
    if (!ae) {
        arpdbg("not found arp cache");
        ae = arp_alloc();
        ae->ae_ipaddr = ipaddr;
        ae->ae_dev = veth;
        arp_request(ae);
    } else {
        arpingdbg("state:%d " MACFMT, ae->ae_state, macfmt(ae->ae_hwaddr));
    }
}

void arping2(int argc, char **argv)
{
    int err;

    net_debug |= NET_DEBUG_ARPING|NET_DEBUG_ARP|NET_DEBUG_ALL;

    /* parse args */
    if ((err = parse_args(argc, argv)) < 0) {
        if (err == -1)
            usage();
        return;
    }

    send_packet();

}

