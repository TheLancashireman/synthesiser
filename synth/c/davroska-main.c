/* davroska-main.c - davroska startup, configuration and error reporting
 *
 * (c) David Haworth
*/
#define DV_ASM	0
#include <dv-config.h>
#include <davroska.h>
#include <dv-stdio.h>
#include <dv-string.h>
#include <synth-config.h>

char *project_name = "SynthEffect";

/* main() - start up the OS
*/
int main(int argc, char **argv)
{
	dv_printf("%s starting...\n", project_name);

	/* Initialise all the variables and hardware
	*/
	syntheffect_init();

	dv_statustype_t e = dv_startos(0);

	dv_printf("dv_startos() returned %d  :-(\n", e);

	dv_panic(dv_panic_UnknownRequest, dv_sid_startup, "Oops! dv_startos() returned!");
}

/* callout_reporterror() - report an error detected in a davroska API
*/
dv_statustype_t callout_reporterror(dv_sid_t sid, dv_statustype_t e, dv_qty_t nParam, dv_param_t *p)
{
	dv_printf("Error %d in API %d.", e, sid);
	if ( nParam > 0 )
	{
		dv_printf(" Parameters:");
		for ( int i = 0; i < nParam; i++ )
		{
			dv_printf(" 0x%lx", p[i]);
		}
		dv_printf("\n");
	}

	return e;
}

/* callout_shutdown() - report a shutdown
*/
void callout_shutdown(dv_statustype_t e)
{
	dv_printf("%s shutting down - status %d\n", e);
}

/* callout_panic() - report a panic
*/
void callout_panic(dv_panic_t p, dv_sid_t sid, char *fault)
{
	dv_printf("Panic %d (API %d): %s\n", p, sid, fault);
}

/* callout_idle() - the idle loop
 *
 * This function gets called in a loop. It might be more power-efficient to put the loop here.
*/
void callout_idle(void)
{
	dv_printf("callout_idle() reached\n");
	for (;;) {}
}

/* callout_autostart() - davroska automatic startup
*/
void callout_autostart(dv_id_t unused_mode)
{
}

/* callout_addtasks() - davroska object creation
*/
void callout_addtasks(dv_id_t unused_mode)
{
}

/* callout_addisrs() - davroska object creation
*/
void callout_addisrs(dv_id_t unused_mode)
{
}

/* callout_addgroups() - davroska object creation
*/
void callout_addgroups(dv_id_t unused_mode)
{
}

/* callout_addmutexes() - davroska object creation
*/
void callout_addmutexes(dv_id_t unused_mode)
{
}

/* callout_addcounters() - davroska object creation
*/
void callout_addcounters(dv_id_t unused_mode)
{
}

/* callout_addalarms() - davroska object creation
*/
void callout_addalarms(dv_id_t unused_mode)
{
}
