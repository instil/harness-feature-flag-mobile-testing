#ifndef STACKTRACEPRINTER_H
#define STACKTRACEPRINTER_H

#if defined(Q_OS_LINUX)

#include <execinfo.h>
#include <signal.h>
#include <unistd.h>

void printStacktraceHandler(int sig) {
    void *array[10];
    size_t size;

    size = backtrace(array, 10);
    fprintf(stderr, "Error: signal %d:\n", sig);
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    exit(1);
}

void registerStacktracePrinterOnSegFault() {
    signal(SIGSEGV, printStacktraceHandler);
}

#else

void printStacktraceHandler(int sig) { }

void registerStacktracePrinterOnSegFault() { }

#endif

#endif // STACKTRACEPRINTER_H
