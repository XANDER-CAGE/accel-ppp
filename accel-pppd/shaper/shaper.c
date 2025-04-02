#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <linux/if_ether.h>
#include <linux/pkt_cls.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "ppp.h"
#include "shaper.h"
#include "tc_core.h"
#include "log.h"
#include "libnetlink.h"

#ifndef TCA_FW_CLASSID
#define TCA_FW_CLASSID 1
#endif

#ifndef TCA_FW_MARK
#define TCA_FW_MARK 2
#endif

// Заглушка для struct list_head, если нужно
struct list_head {
    struct list_head *next, *prev;
};

// Заглушка для qdisc
static int dummy_qdisc(struct qdisc_opt *opt, struct nlmsghdr *n) {
    return 0;
}

// install_htb_with_fwmark: демо без настоящего tc
int install_htb_with_fwmark(struct ap_session *ses, struct shaper_rule *rule, int base_classid)
{
    log_ppp_info("[shaper] install_htb_with_fwmark(): ifindex=%d fwmark=%d rate=%dKbps\n",
                 ses->ifindex, rule->fwmark, rule->down_speed);

    // В реальной версии здесь были бы вызовы tc_qdisc_modify и addattr_l
    return 0;
}

int remove_htb_with_fwmark(struct ap_session *ses, struct shaper_rule *rule)
{
    log_ppp_info("[shaper] remove_htb_with_fwmark(): ifindex=%d fwmark=%d\n",
                 ses->ifindex, rule->fwmark);
    return 0;
}

int install_limiter(struct ap_session *ses, int down_speed, int down_burst, int up_speed, int up_burst, int idx)
{
    log_ppp_info("[shaper] install_limiter(): ifindex=%d down=%dKbps up=%dKbps idx=%d\n",
                 ses->ifindex, down_speed, up_speed, idx);
    return 0;
}

int remove_limiter(struct ap_session *ses, int idx)
{
    log_ppp_info("[shaper] remove_limiter(): ifindex=%d idx=%d\n",
                 ses->ifindex, idx);
    return 0;
}

int install_leaf_qdisc(struct rtnl_handle *rth, int ifindex, int parent, int handle)
{
    log_ppp_info("[shaper] install_leaf_qdisc(): ifindex=%d parent=0x%x handle=0x%x\n",
                 ifindex, parent, handle);
    return 0;
}

void leaf_qdisc_parse(const char *opt)
{
    log_ppp_info("[shaper] leaf_qdisc_parse(): opt=%s\n", opt);
}

int tc_qdisc_modify(struct rtnl_handle *rth, int ifindex, int cmd, unsigned flags, struct qdisc_opt *opt)
{
    log_ppp_info("[shaper] tc_qdisc_modify(): ifindex=%d kind=%s cmd=%d\n",
                 ifindex, opt->kind ? opt->kind : "(null)", cmd);
    return 0;
}