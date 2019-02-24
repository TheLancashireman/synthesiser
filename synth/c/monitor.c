/*	monitor.c - health monitor task
 *
 *	Copyright 2019 David Haworth
 *
 *	This file is part of SynthEffect.
 *
 *	SynthEffect is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	SynthEffect is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with SynthEffect.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <dv-config.h>
#include <synth-stdio.h>
#include <monitor.h>

struct monitor_elapsed_s core1_loop;
struct monitor_elapsed_s core1_idle;

struct monitor_elapsed_s core2_loop;
struct monitor_elapsed_s core2_idle;

struct monitor_elapsed_s core3_loop;
struct monitor_elapsed_s core3_idle;

int monitor_pace;

/* Monitor_main() - monitor task main function
 *
 * Runs once per second
*/
void Monitor_main(void)
{
#if 0
	sy_printf("Alive!\n");
#endif

	monitor_pace = (monitor_pace >= 9) ? 0 : (monitor_pace + 1);

	switch (monitor_pace)
	{
	case 0:
		print_elapsed(&core1_loop, "Loop 1");
		core1_loop.init = 0;
		break;
	case 1:
		print_elapsed(&core1_idle, "Idle 1");
		core1_idle.init = 0;
		break;
	case 2:
#if 0
		print_elapsed(&core2_loop, "Loop 2");
		core2_loop.init = 0;
		break;
#endif
	case 3:
#if 0
		print_elapsed(&core2_idle, "Idle 2");
		core2_idle.init = 0;
		break;
#endif
	case 4:
#if 0
		print_elapsed(&core3_loop, "Loop 3");
		core3_loop.init = 0;
		break;
#endif
	case 5:
#if 0
		print_elapsed(&core3_idle, "Idle 3");
		core3_idle.init = 0;
		break;
#endif
	case 6:
	case 7:
	case 8:
	case 9:
	default:
		break;
	}
}
