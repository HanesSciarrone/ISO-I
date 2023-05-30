#include <stddef.h>

#include "os_core.h"
#include "stm32f429xx.h"
#include "system_stm32f4xx.h"
#include "core_cm4.h"
#include "cmsis_gcc.h"

/* ==================== Define private variables ==================== */

typedef struct
{
    osTaskStruct *listTask[MAX_NUMBER_TASK];    // Task list.
    osTaskStruct *currentTask;                  // Current handler task running
    osTaskStruct *nextTask;                     // Next handler task will be run
    uint8_t      countTask;                     // Number of task created;
    bool         running;                       // Status task, if it is running true in otherwise false
}osControlStruct;

/* ================== Private variables declaration ================= */
static osControlStruct osControl = {0};

/* ================== Private functions declaration ================= */

/**
 * @brief Get next context task.
 *
 * @bparam[in] currentStaskPointer Stack pointer of current task.
 *
 * @return Return stack pointer of new task to execute.
 */
static uint32_t getNextContext(uint32_t currentStaskPointer);
static void scheduler(void);

/* ================= Public functions implementation ================ */
bool osTaskCreate(osTaskStruct *handler, void *callback)
{
    static uint8_t taskNumber = 0;

    if (taskNumber >= MAX_NUMBER_TASK)
    {
        return false;
    }

    handler->memory[MAX_STACK_SIZE/4 - XPSR_REG_POSITION] = XPSR_VALUE;         // xPSR value with 24 bit on one (Thumb mode).
    handler->memory[MAX_STACK_SIZE/4 - PC_REG_POSTION]    = (uint32_t)callback; // Program pointer (PC) points to function used by the task.

    /*
     * Previous Link register (LR) value because handler pendSV call function inside exception
     * and LR is overwrite with the return value of getNextContext.
     */
    handler->memory[MAX_STACK_SIZE/4 - LR_PREV_VALUE_POSTION] = EXEC_RETURN_VALUE;
    handler->entryPoint     = callback; // Pointer fuction of task.
    handler->id             = taskNumber;
    handler->stackPointer   = (uint32_t)(handler->memory + MAX_STACK_SIZE/4 - SIZE_STACK_FRAME);

    // Fill controls OS structure
    osControl.listTask[taskNumber] = handler;
    ++taskNumber;
    osControl.countTask = taskNumber;

    // Ask to avoid overflow memory when fill element vector
    if (taskNumber < MAX_NUMBER_TASK)
    {
        osControl.listTask[taskNumber] = NULL;
    }

    return true;
}

void osStart(void)
{
    NVIC_DisableIRQ(SysTick_IRQn);
    NVIC_DisableIRQ(PendSV_IRQn);

    osControl.running = false;
    osControl.currentTask = NULL;
    osControl.nextTask = NULL;

    /*
     * All interrupts has priority 0 (maximum) at start execution. For that don't happen fault
     * condition, we have to less priotity of NVIC. This math calculation showing take lowest
     * priority possible.
     */
    NVIC_SetPriority(PendSV_IRQn, (1 << __NVIC_PRIO_BITS)-1);

    /* Activate and configure the time of Systick exception */
    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / SYSTICK_TIME);

    NVIC_EnableIRQ(PendSV_IRQn);
    NVIC_EnableIRQ(SysTick_IRQn);
}


static uint32_t getNextContext(uint32_t currentStaskPointer)
{
     // Is the first time execute operating system? Yes, so will do task charged on next task.
    if (!osControl.running)
    {
        osControl.currentTask->state = OS_TASK_RUNNING;
        osControl.running = true;
        return osControl.currentTask->stackPointer;
    }

    // Storage last stack pointer used on current task and change state to ready.
    osControl.currentTask->stackPointer = currentStaskPointer;
    osControl.currentTask->state = OS_TASK_READY;

    // Switch address memory points on current task for next task and change state of task
    osControl.currentTask = osControl.nextTask;
    osControl.currentTask->state = OS_TASK_RUNNING;

    return osControl.currentTask->stackPointer;
}


/* ================ Private functions implementation ================ */
static void scheduler(void)
{
    uint8_t index = 0;

    // Is the first time that operating system execute? Yes, so I start with Task1
    if (!osControl.running) {
        osControl.currentTask = osControl.listTask[0];
        return;
    }

    // Calculate next task will be execute
    index = osControl.currentTask->id + 1;

    // If is the last task so I start againt with first.
    if (index >= MAX_NUMBER_TASK || osControl.listTask[index] == NULL) {
        osControl.nextTask = osControl.listTask[0];
        return;
    }

    // Setup next task to execute
    osControl.nextTask = osControl.listTask[index];
}

/* ========== Processor Interruption and Exception Handlers ========= */

void SysTick_Handler(void)
{
        scheduler();

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

void PendSV_Handler(void)
{
    /*
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
    __ASM volatile ("bx lr");               //Se hace un branch indirect con el valor de LR que es nuevamente EXEC_RETURN
}
