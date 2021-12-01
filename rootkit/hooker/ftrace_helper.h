#ifndef FTRACE_HELPER_H
#define FTRACE_HELPER_H

#include <linux/ftrace.h>
#include <linux/linkage.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/version.h>

#define HOOK(_name, _hook, _orig)   \
{                                   \
    .name = (_name),                \
    .function = (_hook),            \
    .original = (_orig),            \
}

#define USE_FENTRY_OFFSET 0
#if !USE_FENTRY_OFFSET
#pragma GCC optimize("-fno-optimize-sibling-calls")
#endif

struct ftrace_hook {
    const char *name;
    void *function;
    void *original;

    unsigned long address;
    struct ftrace_ops ops;
};

int fh_resolve_hook_address(struct ftrace_hook *);
int fh_install_hook(struct ftrace_hook *);
int fh_install_hooks(struct ftrace_hook *, size_t);

void notrace fh_ftrace_thunk(unsigned long, unsigned long, struct ftrace_ops *, struct pt_regs *);
void fh_remove_hook(struct ftrace_hook *);
void fh_remove_hooks(struct ftrace_hook *, size_t);

#endif
