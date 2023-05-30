#ifndef OS_CORE_H_
#define OS_CORE_H_

#include <stdint.h>
#include <stdbool.h>

#define MAX_NUMBER_TASK         8           // Define maximum task we could create
#define MAX_STACK_SIZE          256         // Define maximum stack size for a trask
#define SYSTICK_TIME            1000        // Tiempo del systick en milisegundos.

//#ifdef OS_USE_HARDWARE_FPU
#define SIZE_STACK_FRAME        17          // Size stack frame
//#else
//#define SIZE_STACK_FRAME        8         // Size stack frame
//#endif


#define XPSR_VALUE              1 << 24     // xPSR.T = 1
#define EXEC_RETURN_VALUE       0xFFFFFFF9  // EXEC_RETURN value. Return to thread mode with MSP, not use FPU
#define XPSR_REG_POSITION       1
#define PC_REG_POSTION          2
#define LR_REG_POSTION          3
#define R12_REG_POSTION         4
#define R3_REG_POSTION          5
#define R2_REG_POSTION          6
#define R1_REG_POSTION          7
#define R0_REG_POSTION          8
#define LR_PREV_VALUE_POSTION   9
#define R4_REG_POSTION          10
#define R5_REG_POSTION          11
#define R6_REG_POSTION          12
#define R7_REG_POSTION          13
#define R8_REG_POSTION          14
#define R9_REG_POSTION          15
#define R10_REG_POSTION         16
#define R11_REG_POSTION         17

/* Possible task state */
typedef enum
{
    OS_TASK_READY,      // Ready state
    OS_TASK_RUNNING,    // Running state
    OS_TASK_BLOCK,      // Blocked state
    OS_TASK_SUSPEND     // Suspended state
}osTaskStateType;

typedef struct
{
    uint32_t        memory[MAX_STACK_SIZE/4];   // Memory stack
    uint32_t        stackPointer;               // Stack pointer of task
    void            *entryPoint;                // Callback executed on task
    uint8_t         id;                         // Task ID, it is a number started in 0
    osTaskStateType state;                      // State task
}osTaskStruct;


/**
 * @brief Create task.
 *
 * @param[in,out]   handler     Data structure of task.
 * @param[in]       callback    Function executed on task
 *
 * @return Return true if task was success or false in otherwise.
 */
bool osTaskCreate(osTaskStruct *handler, void *callback);

/**
 * @brief Initialization pendSV exception with lowest priority possible.
 */
void osStart(void);

#endif // OS_CORE_H_
