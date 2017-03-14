/*
	here we can include some standard header file
*/
#include <stdio.h>
#include <string.h>
/*
	here include some special hearder file you need
*/
#include "inc/arc/arc_exception.h"
#include "inc/arc/arc_timer.h"
#include "board/board.h"

/* while use bsp code to start system tick, don't use LOS header */
#define INCLUDE_LOS_HEADER
#ifdef INCLUDE_LOS_HEADER
#include "los_tick.ph"
#include "los_base.h"
#include "los_task.ph"
#include "los_swtmr.h"
#include "los_hwi.h"
#endif

/*****************************************************************************
	global var
 *****************************************************************************/
/* current system Freq , should be set according to the microchip */

const unsigned int sys_clk_freq = OS_SYS_CLOCK;
/* tick count per second , don't change this */
const unsigned int tick_per_second = LOSCFG_BASE_CORE_TICK_PER_SECOND;
static unsigned int g_ucycle_per_tick = 0;


const unsigned char g_use_ram_vect = 1;
/*****************************************************************************
	LOS function extern 
 *****************************************************************************/
extern void LOS_SetTickSycle(unsigned int);
extern void LOS_TickHandler(void);
extern unsigned int osGetVectorAddr(void);



/*****************************************************************************
 Function    : osTickStart
 Description : Set system tick reqister, and start system tick exception
              Note that This function is called by LOS_Start() In LiteOS kernel.
              so, please don't init system tick before LiteOS kernel Inited
              in your own system.
 Input       : None
 Output      : None
 Return      : LOS_OK
 *****************************************************************************/
unsigned int osTickStart(void)
{

    g_ucycle_per_tick = sys_clk_freq / tick_per_second;
    LOS_SetTickSycle(g_ucycle_per_tick);

    int_disable(BOARD_OS_TIMER_INTNO); /* disable os timer interrupt */
    timer_stop(BOARD_OS_TIMER_ID);
    timer_start(BOARD_OS_TIMER_ID, TIMER_CTRL_IE | TIMER_CTRL_NH, g_ucycle_per_tick);

    int_handler_install(BOARD_OS_TIMER_INTNO, (INT_HANDLER)LOS_TickHandler);
    int_enable(BOARD_OS_TIMER_INTNO);

    return 0;
}


/*****************************************************************************
 Function    : LosAdapIntInit
 Description : config vector table in ram.
 Input       : None
 Output      : None
 Return      : LOS_OK
 *****************************************************************************/
void LosAdapIntInit(void)
{
  
}

/*****************************************************************************
 Function    : LosAdapIrpEnable
 Description : external interrupt enable, and set priority 
               this function is called by LOS_HwiCreate(), 
               so here can use bsp func to inplemente it 
               Note : if don't use LOS_HwiCreate(), leave it empty
 Input       : irqnum: external interrupt number
               prior: priority of this interrupt
 Output      : None
 Return      : None
 *****************************************************************************/
void LosAdapIrpEnable(unsigned int irqnum, unsigned short prior)
{
    int_pri_set(irqnum, prior + INT_PRI_MIN);
    int_enable(irqnum);
}

/*****************************************************************************
 Function    : LosAdapIrqDisable
 Description : external interrupt disable
               this function is called by LOS_HwiDelete(), so use bsp func 
               to inplemente it
               Note : if don't use LOS_HwiDelete(), leave it empty
 Input       : irqnum: external interrupt number
 Output      : None
 Return      : None
 *****************************************************************************/
void LosAdapIrqDisable(unsigned int irqnum)
{
    int_disable(irqnum);
}
