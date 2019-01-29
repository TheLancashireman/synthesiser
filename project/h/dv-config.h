/* dv-config.h - configuration header file for a davroska system
 *
 * This file contains the configuration parameters for the davroska kernel used in $SYNTH
 *
 * (c) David Haworth
*/
#ifndef dv_config_h
#define dv_config_h	1

#define DV_TARGET			<arm64/h/dv-target-pi3-arm64.h>
#define DV_DEMO_TARGET		<demo-pi3-arm64.h>
#define DV_TARGET_CORE		0

/* DV_CFG_MAXEXE is the maximum number of executables (tasks + ISRs) that you can create.
*/
#define DV_CFG_MAXEXE		20

/* DV_CFG_MAXEXTENDED is the maximum number of extended executables that you can create.
 * If this is zero, the event API is omitted.
*/
#define DV_CFG_MAXEXTENDED	5

/* DV_CFG_MAXPRIO is the maximum number of different priorities that you can use
 * The number includes all task and ISR priorities
*/
#define DV_CFG_MAXPRIO		10

/* DV_CFG_NSLOT_EXTRA is the number of extra queue slots you need for tasks with maxact > 1
 * If this is zero the "multiple activations" feature is omitted,
*/
#define DV_CFG_NSLOT_EXTRA	10

/* DV_CFG_MAXMUTEX is the maximum number of mutexes that you can create
*/
#define DV_CFG_MAXMUTEX		10

/* DV_CFG_MAXCOUNTER is the maximum number of counters that you can create
*/
#define DV_CFG_MAXCOUNTER	5

/* DV_CFG_MAXALARM is the maximum number of alarms that you can create
*/
#define DV_CFG_MAXALARM		10

/* Inline versions of the empty callout functions
*/
static inline void callout_preexe(void)
{
}

static inline void callout_postexe(void)
{
}

static inline void callout_startup(void)
{
}

#endif
