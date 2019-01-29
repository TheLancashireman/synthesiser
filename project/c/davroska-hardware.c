/* davroska-hardware.c - davroska- and hardware-specific functions to glue the thing together
 *
 * (c) David Haworth
*/
#define DV_ASM	0
#include <dv-config.h>
#include <davroska.h>
#include <dv-stdio.h>
#include <dv-string.h>

#include <dv-arm-bcm2835-uart.h>
#include <dv-arm-bcm2835-aux.h>
#include <dv-arm-bcm2835-interruptcontroller.h>
#include <dv-armv8-mmu.h>
#include <dv-arm-bcm2835-systimer.h>
#include <dv-arm-bcm2835-armtimer.h>

extern dv_u64_t dv_c1_stack_top, dv_c2_stack_top, dv_c3_stack_top;

const dv_u64_t dv_c1_initialsp = (dv_address_t)&dv_c1_stack_top - DV_CANARY;
const dv_u64_t dv_c2_initialsp = (dv_address_t)&dv_c2_stack_top - DV_CANARY;
const dv_u64_t dv_c3_initialsp = (dv_address_t)&dv_c3_stack_top - DV_CANARY;

extern int main(int argc, char **argv);

static void dv_init_core(void);

/* Startup and exception handling
*/
extern dv_u32_t dv_start_bss, dv_end_bss, dv_vectortable;

void dv_board_start(void)
{
	/* Initialise bss
	*/
	dv_memset32(&dv_start_bss, 0,
		((dv_address_t)&dv_end_bss - (dv_address_t)&dv_start_bss + sizeof(dv_u32_t) - 1) / sizeof(dv_u32_t));

	/* Initialise uart and connect it to the stdio functions
	*/
	dv_arm_bcm2835_uart_init(115200, 8, 0);
	dv_arm_bcm2835_uart_console();

	dv_printf("pi-3-arm64 starting ...\n");
	dv_printf("stack pointer = 0x%016lx\n", dv_get_SP());

	dv_init_core();

	dv_arm64_msr(VBAR_EL1, (dv_u64_t)&dv_vectortable);

	/* Set up the MMU
	*/
	dv_armv8_mmu_setup(1);

	main(0, 0);
}

/* dv_init_core() - initialise the important control registers then drop to EL1/NS
 *
 * Most of the registers are "safe" values. Some of the EL1 registers get overwritten later.
*/
static void dv_init_core(void)
{
	dv_u32_t el = dv_get_el();

	if ( el >= 3 )
	{
		dv_arm64_msr(ELR_EL3, 0);
		dv_arm64_msr(VBAR_EL3, (dv_u64_t)&dv_vectortable);
		dv_arm64_msr(SCR_EL3, 0xcb1);						/* ToDo: explain "magic number" */
		dv_arm64_msr(SCTLR_EL3, 0);
		dv_arm64_msr(CPTR_EL3, 0);
		dv_arm64_msr(MDCR_EL3, 0);
	}

	if ( el >= 2 )
	{
		dv_arm64_msr(ELR_EL2, 0);
		dv_arm64_msr(VBAR_EL2, (dv_u64_t)&dv_vectortable);
		dv_arm64_msr(HCR_EL2, 0x80000002);					/* RW = 1 ==> execution state for EL1 is aarch64 */
		dv_arm64_msr(SCTLR_EL2, 0);
		dv_arm64_msr(VTTBR_EL2, 0);
		dv_arm64_msr(CPTR_EL2, 0);
		dv_arm64_msr(MDCR_EL2, 0);
		dv_arm64_msr(CNTHCTL_EL2, dv_arm64_mrs(CNTHCTL_EL2)|0x03uL);
		dv_arm64_msr(CNTVOFF_EL2, 0);
	}

	if ( el >= 1 )
	{
		dv_arm64_msr(ELR_EL1, 0);
		dv_arm64_msr(SPSR_EL1, 0);
		dv_arm64_msr(SCTLR_EL1, 0);
		__asm__ volatile("tlbi ALLE1");
		dv_u64_t cpacr = dv_arm64_mrs(CPACR_EL1)|0x300000;		/* FPEN = b11 ==> disable FPU instruction traps */
		dv_arm64_msr(CPACR_EL1, cpacr);
	}

	if ( el == 3 )
	{
		dv_printf("Current EL = %d\n", el);
		dv_printf("Dropping to EL2\n");
		dv_switch_el3el2();
		el = dv_get_el();
	}

	if ( el == 2 )
	{
		dv_printf("Current EL = %d\n", el);
		dv_printf("Dropping to EL1\n");
		dv_switch_el2el1();
		el = dv_get_el();
	}

	dv_printf("Current EL = %d\n", el);

	if ( el != 1 )
	{
		dv_panic(dv_panic_UnexpectedHardwareResponse, dv_sid_startup, "Oops! Started in unsupported exception level");
	}
}

/* Assorted panic trampolines for use in assembly language code.
*/
void dv_panic_return_from_switchcall_function(void)
{
	dv_panic(dv_panic_ReturnFromLongjmp, dv_sid_scheduler, "Oops! The task wrapper returned");
}

void dv_catch_fiq_wrong_state(dv_stackword_t *sp)
{
	dv_panic(dv_panic_Exception, dv_sid_exceptionhandler, "Oops! An fiq interrupt occurred from the wrong state");
}

void dv_catch_irq_wrong_state(dv_stackword_t *sp)
{
	dv_panic(dv_panic_Exception, dv_sid_exceptionhandler, "Oops! An irq interrupt occurred from the wrong state");
}

void dv_catch_synchronous_exception_wrong_state(dv_stackword_t *sp)
{
	dv_panic(dv_panic_Exception, dv_sid_exceptionhandler, "Oops! A sync exception occurred from the wrong state");
}

void dv_catch_syserror_wrong_state(dv_stackword_t *sp)
{
	dv_panic(dv_panic_Exception, dv_sid_exceptionhandler, "Oops! A SeError exception occurred from the wrong state");
}

void dv_catch_fiq(dv_stackword_t *sp)
{
	dv_panic(dv_panic_Exception, dv_sid_exceptionhandler, "Oops! An fiq interrupt occurred");
}

void dv_catch_synchronous_exception(dv_stackword_t *sp)
{
	dv_panic(dv_panic_Exception, dv_sid_exceptionhandler, "Oops! A sync exception occurred");
}

void dv_catch_syserror(dv_stackword_t *sp)
{
	dv_panic(dv_panic_Exception, dv_sid_exceptionhandler, "Oops! A SeError exception occurred");
}
