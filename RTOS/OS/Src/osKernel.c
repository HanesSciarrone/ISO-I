#include <stddef.h>

#include "osKernel.h"
#include "system_stm32f4xx.h"
#include "stm32f429xx.h"
#include "core_cm4.h"
#include "cmsis_gcc.h"

/* ==================== Define private variables ==================== */
#define FIRST_INDEX_TASK_PRIORITY     0U

typedef struct
{
    osTaskObject*   listTask[MAX_NUMBER_TASK];                              // Task list.
    osTaskObject*   currentTask;                                            // Current handler task running.
    osTaskObject*   nextTask;                                               // Next handler task will be run.
    uint8_t         countTask;                                              // Number of task created.
    bool            running;                                                // Status task, if it is running true in otherwise false.
}osKernelObject;

/* ================== Private variables declaration ================= */
static osKernelObject osKernel = { 0 };

/* ================== Private functions declaration ================= */

static uint32_t getNextContext(uint32_t currentStaskPointer);
static void scheduler(void);

/* ================= Public functions implementation ================ */

bool osTaskCreate(osTaskObject* handler, osPriorityType priority, void* callback)
{
    if (osKernel.countTask >= MAX_NUMBER_TASK)
    {
        return false;
    }

    // xPSR value with 24 bit on one (Thumb mode).
    handler->memory[MAX_STACK_SIZE/4 - XPSR_REG_POSITION]   = XPSR_VALUE;
    // Program pointer (PC) points to function used by the task.
    handler->memory[MAX_STACK_SIZE/4 - PC_REG_POSTION]      = (uint32_t)callback;
    // If occur some problem and task executed return so after that execute this function.
    handler->memory[MAX_STACK_SIZE/4 - LR_REG_POSTION]      = (uint32_t)osReturnTaskHook;

    /*
     * Previous Link register (LR) value because handler pendSV call function inside exception
     * and LR is overwrite with the return value of getNextContext.
     */
    handler->memory[MAX_STACK_SIZE/4 - LR_PREV_VALUE_POSTION] = EXEC_RETURN_VALUE;

    // Pointer function of task.
    handler->entryPoint     = callback;
    handler->id             = osKernel.countTask;
    handler->state          = OS_TASK_READY;
    handler->priority       = priority;
    handler->stackPointer   = (uint32_t)(handler->memory + MAX_STACK_SIZE/4 - SIZE_STACK_FRAME);

    // Fill controls OS structure
    osKernel.listTask[osKernel.countTask] = handler;
    osKernel.countTask++;

    // Ask to avoid overflow memory when fill element vector
    if (osKernel.countTask < MAX_NUMBER_TASK)
    {
        osKernel.listTask[osKernel.countTask] = NULL;
    }

    return true;
}

void osStart(void)
{
    NVIC_DisableIRQ(SysTick_IRQn);
    NVIC_DisableIRQ(PendSV_IRQn);

    osKernel.running = false;
    osKernel.currentTask = NULL;
    osKernel.nextTask = NULL;

    /*
     * All interrupts has priority 0 (maximum) at start execution. For that don't happen fault
     * condition, we have to less priotity of NVIC. This math calculation showing take lowest
     * priority possible.
     */
    NVIC_SetPriority(PendSV_IRQn, (1 << __NVIC_PRIO_BITS)-1);

    /* Activate and configure the time of Systick exception */
    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / (1000U * SYSTICK_PERIOD_MS));

    NVIC_EnableIRQ(PendSV_IRQn);
    NVIC_EnableIRQ(SysTick_IRQn);
}

void osDelay(const uint32_t tick)
{
    (void)tick;
}

__attribute__((weak)) void osReturnTaskHook(void)
{
    while(1)
    {
        __WFI();
    }
}

__attribute__((weak)) void osSysTickHook(void)
{
    __ASM volatile ("nop");
}

__attribute__((weak)) void osErrorHook(void* caller)
{
    while(1)
    {
    }
}

__attribute__((weak)) void osIdleTask(void)
{
    while(1)
    {
    }
}

/* ================ Private functions implementation ================ */

/**
 * @brief Get next context task.
 *
 * @param[in] currentStaskPointer Stack pointer of current task.
 *
 * @return Return stack pointer of new task to execute.
 */
static uint32_t getNextContext(uint32_t currentStaskPointer)
{
     // Is the first time execute operating system? Yes, so will do task charged on next task.
    if (!osKernel.running)
    {
        osKernel.currentTask->state = OS_TASK_RUNNING;
        osKernel.running            = true;
    }
    else
    {
        // Storage last stack pointer used on current task and change state to ready.
        osKernel.currentTask->stackPointer  = currentStaskPointer;
        osKernel.currentTask->state         = OS_TASK_READY;

        // Switch address memory points on current task for next task and change state of task
        osKernel.currentTask            = osKernel.nextTask;
        osKernel.currentTask->state     = OS_TASK_RUNNING;
    }

    return osKernel.currentTask->stackPointer;
}

/**
 * @brief Get the task that must be run.
 *
 * @return Returns true if a new task to be executed.
 */
static void scheduler(void)
{
    uint8_t index = 0;

    // Is the first time that operating system execute? Yes, so I start with Task1
    if (!osKernel.running) {
        osKernel.currentTask = osKernel.listTask[0];
    }
    else
    {
        index = osKernel.currentTask->id + 1;

        // If is the last task so I start againt with first.
        if (index >= MAX_NUMBER_TASK || NULL == osKernel.listTask[index])
        {
            index = 0;
        }

        osKernel.nextTask = osKernel.listTask[index];
    }
}

/* ========== Processor Interruption and Exception Handlers ========= */

void SysTick_Handler(void)
{
    scheduler();
    osSysTickHook();

    /*
     * Set up bit corresponding exception PendSV
     */
    SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;

    /*
     * Instruction Synchronization Barrier; flushes the pipeline and ensures that
     * all previous instructions are completed before executing new instructions
     */
    __ISB();

    /*
     * Data Synchronization Barrier; ensures that all memory accesses are
     * completed before next instruction is executed
     */
    __DSB();
}

__attribute__ ((naked)) void PendSV_Handler(void)
{
    // Se entra a la seccion critica y se deshabilita las interrupciones.
	__ASM volatile ("cpsid i");

    /**
     * Implementación de stacking para FPU:
     *
     * Las tres primeras corresponden a un testeo del bit EXEC_RETURN[4]. La instruccion TST hace un
     * AND estilo bitwise (bit a bit) entre el registro LR y el literal inmediato. El resultado de esta
     * operacion no se guarda y los bits N y Z son actualizados. En este caso, si el bit EXEC_RETURN[4] = 0
     * el resultado de la operacion sera cero, y la bandera Z = 1, por lo que se da la condicion EQ y
     * se hace el push de los registros de FPU restantes
     */
    __ASM volatile ("tst lr, 0x10");
    __ASM volatile ("it eq");
    __ASM volatile ("vpusheq {s16-s31}");

    /**
     * Cuando se ingresa al handler de PendSV lo primero que se ejecuta es un push para
	 * guardar los registros R4-R11 y el valor de LR, que en este punto es EXEC_RETURN
	 * El push se hace al reves de como se escribe en la instruccion, por lo que LR
	 * se guarda en la posicion 9 (luego del stack frame). Como la funcion getNextContext
	 * se llama con un branch con link, el valor del LR es modificado guardando la direccion
	 * de retorno una vez se complete la ejecucion de la funcion
	 * El pasaje de argumentos a getContextoSiguiente se hace como especifica el AAPCS siendo
	 * el unico argumento pasado por RO, y el valor de retorno tambien se almacena en R0
	 *
	 * NOTA: El primer ingreso a este handler (luego del reset) implica que el push se hace sobre el
	 * stack inicial, ese stack se pierde porque no hay seguimiento del MSP en el primer ingreso
     */
    __ASM volatile ("push {r4-r11, lr}");
    __ASM volatile ("mrs r0, msp");
    __ASM volatile ("bl %0" :: "i"(getNextContext));
    __ASM volatile ("msr msp, r0");
    __ASM volatile ("pop {r4-r11, lr}");    //Recuperados todos los valores de registros

    /**
     * Implementación de unstacking para FPU:
     *
     * Habiendo hecho el cambio de contexto y recuperado los valores de los registros, es necesario
     * determinar si el contexto tiene guardados registros correspondientes a la FPU. si este es el caso
     * se hace el unstacking de los que se hizo PUSH manualmente.
     */
    __ASM volatile ("tst lr,0x10");
    __ASM volatile ("it eq");
    __ASM volatile ("vpopeq {s16-s31}");

    // Se sale de la seccion critica y se habilita las interrupciones.
	__ASM volatile ("cpsie i");

    /* Se hace un branch indirect con el valor de LR que es nuevamente EXEC_RETURN */
    __ASM volatile ("bx lr");
}
