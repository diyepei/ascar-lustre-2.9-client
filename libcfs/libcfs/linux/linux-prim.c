/*
 * GPL HEADER START
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 only,
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License version 2 for more details (a copy is included
 * in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU General Public License
 * version 2 along with this program; If not, see
 * http://www.gnu.org/licenses/gpl-2.0.html
 *
 * GPL HEADER END
 */
/*
 * Copyright (c) 2008, 2010, Oracle and/or its affiliates. All rights reserved.
 * Use is subject to license terms.
 *
 * Copyright (c) 2011, 2013, Intel Corporation.
 */
/*
 * This file is part of Lustre, http://www.lustre.org/
 * Lustre is a trademark of Sun Microsystems, Inc.
 */

#define DEBUG_SUBSYSTEM S_LNET
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs_struct.h>
#include <linux/sched.h>

#include <libcfs/libcfs.h>

#if defined(CONFIG_KGDB)
#include <asm/kgdb.h>
#endif

void cfs_init_timer(struct timer_list *t)
{
	init_timer(t);
}
EXPORT_SYMBOL(cfs_init_timer);

void cfs_timer_init(struct timer_list *t, cfs_timer_func_t *func, void *arg)
{
	init_timer(t);
	t->function = func;
	t->data = (unsigned long)arg;
}
EXPORT_SYMBOL(cfs_timer_init);

void cfs_timer_arm(struct timer_list *t, cfs_time_t deadline)
{
	mod_timer(t, deadline);
}
EXPORT_SYMBOL(cfs_timer_arm);

void cfs_timer_disarm(struct timer_list *t)
{
	del_timer(t);
}
EXPORT_SYMBOL(cfs_timer_disarm);

int  cfs_timer_is_armed(struct timer_list *t)
{
	return timer_pending(t);
}
EXPORT_SYMBOL(cfs_timer_is_armed);

cfs_time_t cfs_timer_deadline(struct timer_list *t)
{
	return t->expires;
}
EXPORT_SYMBOL(cfs_timer_deadline);

#ifndef HAVE_KTIME_GET_REAL_TS64
void ktime_get_real_ts64(struct timespec64 *ts)
{
	*ts = timespec_to_timespec64(CURRENT_TIME);
}
EXPORT_SYMBOL(ktime_get_real_ts64);
#endif /* HAVE_KTIME_GET_REAL_TS64 */

sigset_t
cfs_block_allsigs(void)
{
	unsigned long	flags;
	sigset_t	old;

	spin_lock_irqsave(&current->sighand->siglock, flags);
	old = current->blocked;
	sigfillset(&current->blocked);
	recalc_sigpending();
	spin_unlock_irqrestore(&current->sighand->siglock, flags);
	return old;
}
EXPORT_SYMBOL(cfs_block_allsigs);

sigset_t cfs_block_sigs(unsigned long sigs)
{
	unsigned long  flags;
	sigset_t	old;

	spin_lock_irqsave(&current->sighand->siglock, flags);
	old = current->blocked;
	sigaddsetmask(&current->blocked, sigs);
	recalc_sigpending();
	spin_unlock_irqrestore(&current->sighand->siglock, flags);
	return old;
}
EXPORT_SYMBOL(cfs_block_sigs);

/* Block all signals except for the @sigs */
sigset_t cfs_block_sigsinv(unsigned long sigs)
{
	unsigned long flags;
	sigset_t old;

	spin_lock_irqsave(&current->sighand->siglock, flags);
	old = current->blocked;
	sigaddsetmask(&current->blocked, ~sigs);
	recalc_sigpending();
	spin_unlock_irqrestore(&current->sighand->siglock, flags);
	return old;
}
EXPORT_SYMBOL(cfs_block_sigsinv);

void
cfs_restore_sigs(sigset_t old)
{
	unsigned long  flags;

	spin_lock_irqsave(&current->sighand->siglock, flags);
	current->blocked = old;
	recalc_sigpending();
	spin_unlock_irqrestore(&current->sighand->siglock, flags);
}
EXPORT_SYMBOL(cfs_restore_sigs);

void
cfs_clear_sigpending(void)
{
	unsigned long flags;

	spin_lock_irqsave(&current->sighand->siglock, flags);
	clear_tsk_thread_flag(current, TIF_SIGPENDING);
	spin_unlock_irqrestore(&current->sighand->siglock, flags);
}
EXPORT_SYMBOL(cfs_clear_sigpending);
