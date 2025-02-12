/* Copyright (c) 2012-2018, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef __MSM_VIDC_DEBUG__
#define __MSM_VIDC_DEBUG__
#include <linux/debugfs.h>
#include <linux/delay.h>
#include "msm_vidc_internal.h"
#include "trace/events/msm_vidc_events.h"

#ifndef VIDC_DBG_LABEL
#define VIDC_DBG_LABEL "msm_vidc"
#endif

/*
 * This enforces a rate limit: not more than 6 messages
 * in every 1s.
 */

#define VIDC_DBG_SESSION_RATELIMIT_INTERVAL (1 * HZ)
#define VIDC_DBG_SESSION_RATELIMIT_BURST 6

#define VIDC_DBG_TAG VIDC_DBG_LABEL ": %4s: "

/* To enable messages OR these values and
 * echo the result to debugfs file.
 *
 * To enable all messages set debug_level = 0x101F
 */

enum vidc_msg_prio {
	VIDC_ERR  = 0x0001,
	VIDC_WARN = 0x0002,
	VIDC_INFO = 0x0004,
	VIDC_DBG  = 0x0008,
	VIDC_PROF = 0x0010,
	VIDC_PKT  = 0x0020,
	VIDC_FW   = 0x1000,
};

enum vidc_msg_out {
	VIDC_OUT_PRINTK = 0,
};

enum msm_vidc_debugfs_event {
	MSM_VIDC_DEBUGFS_EVENT_ETB,
	MSM_VIDC_DEBUGFS_EVENT_EBD,
	MSM_VIDC_DEBUGFS_EVENT_FTB,
	MSM_VIDC_DEBUGFS_EVENT_FBD,
};

#ifdef CONFIG_DEBUG_FS
extern int msm_vidc_debug;
extern int msm_vidc_debug_out;
extern int msm_vidc_fw_debug;
extern int msm_vidc_fw_debug_mode;
extern int msm_vidc_fw_low_power_mode;
extern bool msm_vidc_fw_coverage;
extern bool msm_vidc_thermal_mitigation_disabled;
extern int msm_vidc_clock_voting;
extern bool msm_vidc_syscache_disable;

#define dprintk(__level, __fmt, arg...)	\
	do { \
		if (msm_vidc_debug & __level) { \
			if (msm_vidc_debug_out == VIDC_OUT_PRINTK) { \
				pr_info(VIDC_DBG_TAG __fmt, \
					get_debug_level_str(__level),	\
					## arg); \
			} \
		} \
	} while (0)

#define dprintk_ratelimit(__level, __fmt, arg...) \
	do { \
		if (msm_vidc_debug & __level) { \
			if (msm_vidc_debug_out == VIDC_OUT_PRINTK && \
					msm_vidc_check_ratelimit()) { \
				pr_info(VIDC_DBG_TAG __fmt, \
					get_debug_level_str(__level),	\
					## arg); \
			} \
		} \
	} while (0)

#define MSM_VIDC_ERROR(value)					\
	do {	if (value)					\
			dprintk(VIDC_DBG, "BugOn");		\
		BUG_ON(value);					\
	} while (0)


struct dentry *msm_vidc_debugfs_init_drv(void);
struct dentry *msm_vidc_debugfs_init_core(struct msm_vidc_core *core,
		struct dentry *parent);
struct dentry *msm_vidc_debugfs_init_inst(struct msm_vidc_inst *inst,
		struct dentry *parent);
void msm_vidc_debugfs_deinit_inst(struct msm_vidc_inst *inst);
void msm_vidc_debugfs_update(struct msm_vidc_inst *inst,
		enum msm_vidc_debugfs_event e);
int msm_vidc_check_ratelimit(void);
#else
static __maybe_unused int msm_vidc_debug = 0;
EXPORT_SYMBOL(msm_vidc_debug);

static __maybe_unused int msm_vidc_debug_out = 0;
EXPORT_SYMBOL(msm_vidc_debug_out);

static __maybe_unused int msm_vidc_fw_debug = 0x00;
static __maybe_unused int msm_vidc_fw_debug_mode = 0;
static __maybe_unused int msm_vidc_fw_low_power_mode = 1;
static __maybe_unused bool msm_vidc_fw_coverage = false;
static __maybe_unused bool msm_vidc_thermal_mitigation_disabled = false;
static __maybe_unused int msm_vidc_clock_voting = 0;
static __maybe_unused bool msm_vidc_syscache_disable = false;

#define dprintk(__level, __fmt, arg...)	\
	do { \
	} while (0)

#define dprintk_ratelimit(__level, __fmt, arg...) \
	do { \
	} while (0)

#define MSM_VIDC_ERROR(value)					\
	do { \
	} while (0)

inline static struct dentry *msm_vidc_debugfs_init_drv(void) { return NULL; }
inline static struct dentry *msm_vidc_debugfs_init_core(struct msm_vidc_core *core,
		struct dentry *parent) { return NULL; }
inline static struct dentry *msm_vidc_debugfs_init_inst(struct msm_vidc_inst *inst,
		struct dentry *parent) { return NULL; }
inline static void msm_vidc_debugfs_deinit_inst(struct msm_vidc_inst *inst) { }
inline static void msm_vidc_debugfs_update(struct msm_vidc_inst *inst,
		enum msm_vidc_debugfs_event e) {}
#endif

static inline char *get_debug_level_str(int level)
{
	switch (level) {
	case VIDC_ERR:
		return "err";
	case VIDC_WARN:
		return "warn";
	case VIDC_INFO:
		return "info";
	case VIDC_DBG:
		return "dbg";
	case VIDC_PROF:
		return "prof";
	case VIDC_PKT:
		return "pkt";
	case VIDC_FW:
		return "fw";
	default:
		return "???";
	}
}

static inline void tic(struct msm_vidc_inst *i, enum profiling_points p,
				 char *b)
{
	struct timeval __ddl_tv;

	if (!i->debug.pdata[p].name[0])
		memcpy(i->debug.pdata[p].name, b, 64);
	if ((msm_vidc_debug & VIDC_PROF) &&
		i->debug.pdata[p].sampling) {
		do_gettimeofday(&__ddl_tv);
		i->debug.pdata[p].start =
			(__ddl_tv.tv_sec * 1000) + (__ddl_tv.tv_usec / 1000);
			i->debug.pdata[p].sampling = false;
	}
}

static inline void toc(struct msm_vidc_inst *i, enum profiling_points p)
{
	struct timeval __ddl_tv;

	if ((msm_vidc_debug & VIDC_PROF) &&
		!i->debug.pdata[p].sampling) {
		do_gettimeofday(&__ddl_tv);
		i->debug.pdata[p].stop = (__ddl_tv.tv_sec * 1000)
			+ (__ddl_tv.tv_usec / 1000);
		i->debug.pdata[p].cumulative += i->debug.pdata[p].stop -
			i->debug.pdata[p].start;
		i->debug.pdata[p].sampling = true;
	}
}

static inline void show_stats(struct msm_vidc_inst *i)
{
	int x;

	for (x = 0; x < MAX_PROFILING_POINTS; x++) {
		if (i->debug.pdata[x].name[0] &&
				(msm_vidc_debug & VIDC_PROF)) {
			if (i->debug.samples) {
				dprintk(VIDC_PROF, "%s averaged %d ms/sample\n",
						i->debug.pdata[x].name,
						i->debug.pdata[x].cumulative /
						i->debug.samples);
			}

			dprintk(VIDC_PROF, "%s Samples: %d\n",
					i->debug.pdata[x].name,
					i->debug.samples);
		}
	}
}

static inline void msm_vidc_res_handle_fatal_hw_error(
	struct msm_vidc_platform_resources *resources,
	bool enable_fatal)
{
	enable_fatal &= resources->debug_timeout;
	MSM_VIDC_ERROR(enable_fatal);
}

static inline void msm_vidc_handle_hw_error(struct msm_vidc_core *core)
{
	bool enable_fatal = true;

	/*
	 * In current implementation user-initiated SSR triggers
	 * a fatal error from hardware. However, there is no way
	 * to know if fatal error is due to SSR or not. Handle
	 * user SSR as non-fatal.
	 */
	if (core->trigger_ssr) {
		core->trigger_ssr = false;
		enable_fatal = false;
	}

	/* Video driver can decide FATAL handling of HW errors
	 * based on multiple factors. This condition check will
	 * be enhanced later.
	 */
	msm_vidc_res_handle_fatal_hw_error(&core->resources, enable_fatal);
}

#endif
