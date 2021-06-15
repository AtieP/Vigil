#include <stdint.h>
#include <cpu/gdt.h>
#include <cpu/idt.h>
#include <cpu/locals.h>
#include <misc/errno.h>
#include <mm/mm.h>
#include <proc/mutex.h>
#include <proc/sched.h>
#include <tools/nummap.h>
#include <tools/panic.h>

#define MODULE_NAME "sched"

static struct mutex sched_mutex = {0};
static struct nummap sched_processes;
static uint8_t sched_irq_vector;
static uint8_t sched_next_vector;

static void sched_next(struct interrupt_frame *gprs, uint8_t vector, uint64_t error_code) {
    (void) vector;
    (void) error_code;
    (void) gprs;
}

static void sched_irq(struct interrupt_frame *gprs, uint8_t vector, uint64_t error_code) {
    sched_next(gprs, vector, error_code);
}

__attribute__((__noreturn__)) void sched_init(void *(*start)(void *)) {
    nummap_create(&sched_processes);
    sched_new_process();
    sched_new_thread(SCHED_KERNEL_PID, start);
    asm volatile(
        "1:\n\t"
        "sti\n\t"
        "hlt\n\t"
        "jmp 1b\n\t"
    );
    __builtin_unreachable();
}

pid_t sched_get_pid() {
    return locals_cpu_get()->current_thread->process->pid;
}

tid_t sched_get_tid() {
    return locals_cpu_get()->current_thread->tid;
}

pid_t sched_new_process() {
    struct sched_process *new_process = kheap_calloc(sizeof(struct sched_process));
    if (!new_process) {
        errno = ENOMEM;
        return -1;
    }
    bool added;
    nummap_create(&new_process->fds);
    nummap_create(&new_process->threads);
    new_process->pid = nummap_add(&sched_processes, new_process, &added);
    if (!added) {
        kheap_free(new_process);
        errno = ENOMEM;
        return -1;
    }
    return new_process->pid;
}

tid_t sched_new_thread(pid_t pid, void *(*start)(void *)) {
    // is pid valid
    struct sched_process *process = nummap_get_by_number(&sched_processes, pid);
    if (!process) {
        errno = EINVAL;
        return -1;
    }
    // create thread struct
    struct sched_thread *new_thread = kheap_calloc(sizeof(struct sched_thread));
    if (!new_thread) {
        errno = EAGAIN;
        return -1;
    }
    // add into the process struct
    bool added;
    new_thread->tid = nummap_add(&process->threads, new_thread, &added);
    if (!added) {
        // no memory left for adding into nummap
        kheap_free(new_thread);
        errno = EAGAIN;
        return -1;
    }
    // create registers and stack
    if (pid == SCHED_KERNEL_PID) {
        new_thread->gprs.cs = GDT_KERNEL_CODE64_SEL;
        new_thread->gprs.ss = GDT_KERNEL_DATA_SEL;
        new_thread->gprs.rflags = 0x202;
        new_thread->gprs.rip = (uint64_t) start;
        new_thread->stack_bottom = (uintptr_t) pmm_calloc(SCHED_STACK_SIZE / MM_PAGE_SIZE);
        if (!new_thread->stack_bottom) {
            // could not allocate
            kheap_free(new_thread);
            errno = EAGAIN;
            return -1;
        }
        new_thread->stack_bottom += MM_HIGHER_BASE;
        new_thread->stack_top = new_thread->stack_bottom + SCHED_STACK_SIZE;
    } else {
        panic(MODULE_NAME, "User threads not supported yet, sorry");
    }
    return new_thread->tid;
}

struct sched_process *sched_get_process_info(pid_t pid) {
    return nummap_get_by_number(&sched_processes, pid);
}

struct sched_thread *sched_get_thread_info(pid_t pid, tid_t tid) {
    struct sched_process *process = nummap_get_by_number(&sched_processes, pid);
    if (!process) {
        return NULL;
    }
    return nummap_get_by_number(&process->threads, tid);
}
