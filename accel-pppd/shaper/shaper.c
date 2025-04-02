#include <stdio.h>
#include <linux/pkt_sched.h>
#include <linux/pkt_cls.h>
#include <netinet/in.h>
#include <string.h>

#include "ppp.h"
#include "log.h"
#include "shaper.h"
#include "tc_core.h"
#include "libnetlink.h"

int install_htb_with_fwmark(struct ap_session *ses, struct shaper_rule *rule, int base_classid) {
    log_ppp_info("[shaper] install_htb_with_fwmark(): ifindex=%d fwmark=%d rate=%dKbps\n",
        ses->ifindex, rule->fwmark, rule->down_speed);
    return 0; // всегда успешно
}

int remove_htb_with_fwmark(struct ap_session *ses, struct shaper_rule *rule) {
    log_ppp_info("[shaper] remove_htb_with_fwmark(): ifindex=%d fwmark=%d\n",
        ses->ifindex, rule->fwmark);
    return 0;
}

int install_limiter(struct ap_session *ses, int down_speed, int down_burst, int up_speed, int up_burst, int idx) {
    log_ppp_info("[shaper] install_limiter(): ifindex=%d ds=%d us=%d\n",
        ses->ifindex, down_speed, up_speed);
    return 0;
}

int remove_limiter(struct ap_session *ses, int idx) {
    log_ppp_info("[shaper] remove_limiter(): ifindex=%d\n", ses->ifindex);
    return 0;
}

int install_leaf_qdisc(struct rtnl_handle *rth, int ifindex, int parent, int handle) {
    log_ppp_info("[shaper] install_leaf_qdisc(): ifindex=%d parent=0x%x handle=0x%x\n", ifindex, parent, handle);
    return 0;
}

int init_ifb(const char *name) {
    log_ppp_info("[shaper] init_ifb(): ifb=%s\n", name);
    return 0;
}

void leaf_qdisc_parse(const char *opt) {
    log_ppp_info("[shaper] leaf_qdisc_parse(): opt='%s'\n", opt);
}

int tc_qdisc_modify(struct rtnl_handle *rth, int ifindex, int cmd, unsigned flags, struct qdisc_opt *opt) {
    log_ppp_info("[shaper] tc_qdisc_modify(): ifindex=%d cmd=%d kind='%s'\n",
        ifindex, cmd, opt ? opt->kind : "null");
    return 0;
}
