Credits: Roman Kulev

-   Two modes:

1.  Thread Mode (Normal software operation mode)
2.  Handler Mode (Interrupt Mode)

-   Two Stack Pointers:

    1.  MSP (main stack pointer)
    2.  PSP (process stack pointer)

-   Reading r13 always reads or writes to MSP or PSP *Handler mode
    always uses the MSP* *Thread mode often uses the PSP*

    1.  MRS (move Register from Special) instruction

    2.  MSR (move Special from Register) instruction *MRS and MSR mostly
        used in priviliged mode*

    3.  PSR (processor status register) look-up documentation *remember
        the thumb bit*

*CONTROL register (3 active bits)* 
1. nPRIV bit: if 1, thread-mode code will run without privilege 
2. SPSEL bit: if 1, the PSP is in use 
3. FPCA bit: enables the FPU

*Nested Vectored Interrupt Controller* 
On entry to an interrupt: 
1. r0-r3, r12, lr, pc and PSR are pushed to active stack
2. special return code is loaded into lr 
3. CPU enters privileged handler more and selects the MSP  
4. Execution of the ISR begins

ISRs can be interrupted by other higher priority interrupts *Highest
priority handler is the hard fault*

At the end of an ISR, a BX lr instruction is executed The special return
code in lr causes the CPU to: 
1. select the correct stack 
2. pop r0-r3, r12, lr, pc and PSR *Popping PSR restores the privilege level and mode*

The SVC instruction is used to generate a software interrupt 
1. takes a single 8 bit constant as argument (doesn't do anything). 
*A constant argument allows one SVC handler to perform multiple tasks by using
**delegates*** 
*In other words, the SVC handler's job is to work out
which delegate to call*

**Nowadays, it's more common to use a register to hold the index of the
delegate on entry; e.g. r7** 
Standard approach is: 
1. Put parameters
into r0-r3 as required (if you want to call the SVC delegate with
arguments, which are stored in r0-r3) 
2. Put desired SVC delegate into
r7 
3. Issue an SVC instruction

Multi-tasking can be provided via **time-sharing** 
*One CPU = one program at any given time*

**A time sharing system uses context switching to stop the CPU running
the task, and to start it running another**

Context switching is split into: 
- Cooperative 
- Pre-emptive

**Cooperative**: 
- The running task indicates that it has done its job,
the next task can have a go
 
**Pre-emptive**: 
- The OS chooses when to switch context - usually linked to a timer; every 1ms is common 
- **Tasks are unaware that context switching is taking place**

**The scheduler decides which task to run next whenever a context switch
takes place** 
- many different schedulers (round robin etc.)

The '**context**' is the entire state of the CPU. A context switch is
performed in an ISR.

**Context Switching** - Usually each task will have its own stack for
temp. storage 
- Each has a task control block (TCB), which contains
information about the task

When a context switch occurs between task A and task B: 
1. All of user-mode registers are pushed to active stack 
2. current stack pointer is stored in task A's TCB 
**We have essentially saved the running
state of task A** 
3. The stack pointer is loaded from task B's TCB 
4. The user-mode registers are popped from the active stack 
**Because the whole of context has been replaced with task B's context, including the
ISR return address, when the ISR finished the CPU will resume task, not
task A**

**Task Initialisation** 
We must essentially fake up a state for the task for it to be started - When a task is first created, its stack must be initialised with a suitable **stack frame**

**PendSV** 
- Another kind of software interrupt, PendSV 
- Triggered by setting the PendSV bit in the ICSR (interrupt control and status
register) 
- Writing the ICSR is only possible from privileged code
**PendSV interrupt has the lowest priority of any interrupt** 
1. PendSV handler will run last after all other interrupts have been processed 2.
PendSV bit is cleared automatically at the start of the PendSV handler

**PendSV is the perfect location for a context switch** 
- Context switch can be triggered from any ISR by setting the PendSV bit 
**The contextswitch can never end up being 'nested'**
