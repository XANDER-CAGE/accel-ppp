/* minimal shaper.c */

#include "shaper.h"
#include "log.h"
#include "ppp.h"
#include "tc_core.h"
#include "libnetlink.h"

#include <stdlib.h>
#include <string.h>

#define CLASSID(idx) TC_H_MAKE(1, idx)
#define LEAF_QDISC_HANDLE(idx) TC_H_MAKE(2, idx)
#define PARENT_CLASS TC_H_MAKE(1, 0)

static int idx_counter = 1;

static int next_idx() {
    return idx_counter++;
}

int install_htb_with_fwmark(struct ap_session *ses, struct shaper_rule *rule, int base_classid) {
    struct rtnl_handle *rth = &g_rth;
    int ifindex = ses->ifindex;
    uint32_t classid = CLASSID(rule->fwmark);

    struct qdisc_opt root = {
        .kind = "htb",
        .handle = TC_H_MAKE(1, 0),
        .parent = TC_H_ROOT,
        .quantum = conf_r2q,
        .defcls = 0,
        .qdisc = qdisc_htb_root,
    };

    tc_qdisc_modify(rth, ifindex, RTM_NEWQDISC, NLM_F_EXCL | NLM_F_CREATE, &root);

    struct qdisc_opt cls = {
        .kind = "htb",
        .handle = classid,
        .parent = PARENT_CLASS,
        .rate = rule->down_speed,
        .buffer = rule->down_burst ? rule->down_burst : rule->down_speed * 0.1,
        .quantum = conf_quantum,
        .qdisc = qdisc_htb_class,
    };

    tc_qdisc_modify(rth, ifindex, RTM_NEWTCLASS, NLM_F_CREATE, &cls);
    install_leaf_qdisc(rth, ifindex, classid, LEAF_QDISC_HANDLE(rule->fwmark));

    struct {
        struct nlmsghdr n;
        struct tcmsg t;
        char buf[256];
    } req;

    memset(&req, 0, sizeof(req));
    req.n.nlmsg_len = NLMSG_LENGTH(sizeof(struct tcmsg));
    req.n.nlmsg_flags = NLM_F_REQUEST | NLM_F_CREATE | NLM_F_EXCL;
    req.n.nlmsg_type = RTM_NEWTFILTER;
    req.t.tcm_family = AF_UNSPEC;
    req.t.tcm_ifindex = ifindex;
    req.t.tcm_parent = PARENT_CLASS;
    req.t.tcm_info = TC_H_MAKE(1, htons(ETH_P_ALL));

    addattr_l(&req.n, sizeof(req), TCA_KIND, "fw", 3);
    struct rtattr *tail = NLMSG_TAIL(&req.n);
    addattr_l(&req.n, sizeof(req), TCA_OPTIONS, NULL, 0);
    addattr32(&req.n, sizeof(req), TCA_FW_CLASSID, classid);
    addattr32(&req.n, sizeof(req), TCA_FW_MARK, rule->fwmark);
    tail->rta_len = (void *)NLMSG_TAIL(&req.n) - (void *)tail;

    rtnl_talk(rth, &req.n, 0, 0, NULL, NULL, NULL, 0);

    return 0;
}

int remove_htb_with_fwmark(struct ap_session *ses, struct shaper_rule *rule) {
    // пустышка для обратной совместимости
    return 0;
}

int install_limiter(struct ap_session *ses, int down_speed, int down_burst, int up_speed, int up_burst, int idx) {
    struct shaper_rule rule = {
        .fwmark = 0,
        .down_speed = down_speed,
        .down_burst = down_burst,
    };
    return install_htb_with_fwmark(ses, &rule, idx);
}

int remove_limiter(struct ap_session *ses, int idx) {
    return 0;
}

int install_leaf_qdisc(struct rtnl_handle *rth, int ifindex, int parent, int handle) {
    struct qdisc_opt opt = {
        .kind = "sfq",
        .handle = handle,
        .parent = parent,
        .qdisc = qdisc_sfq,
    };
    return tc_qdisc_modify(rth, ifindex, RTM_NEWQDISC, NLM_F_EXCL | NLM_F_CREATE, &opt);
}

void leaf_qdisc_parse(const char *opt) {
    // не используется
}

int init_ifb(const char *name) {
    // не используется
    return 0;
}

int tc_qdisc_modify(struct rtnl_handle *rth, int ifindex, int cmd, unsigned flags, struct qdisc_opt *opt) {
    struct {
        struct nlmsghdr n;
        struct tcmsg t;
        char buf[1024];
    } req;

    memset(&req, 0, sizeof(req));
    req.n.nlmsg_len = NLMSG_LENGTH(sizeof(struct tcmsg));
    req.n.nlmsg_flags = NLM_F_REQUEST | flags;
    req.n.nlmsg_type = cmd;
    req.t.tcm_family = AF_UNSPEC;
    req.t.tcm_ifindex = ifindex;
    req.t.tcm_handle = opt->handle;
    req.t.tcm_parent = opt->parent;

    addattr_l(&req.n, sizeof(req), TCA_KIND, opt->kind, strlen(opt->kind) + 1);
    if (opt->qdisc)
        opt->qdisc(opt, &req.n);

    return rtnl_talk(rth, &req.n, 0, 0, NULL, NULL, NULL, cmd == RTM_DELQDISC);
}