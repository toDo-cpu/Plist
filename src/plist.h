#include <vector>
#include <cstring>
#include <windows.h>
#include <tlhelp32.h>


struct ThreadInfo {
    DWORD threadID;
    LONG priority;
};

struct ModuleInfo {

    BYTE * modBaseAddr;
    DWORD modSize;
    char * modName[MAX_MODULE_NAME32 + 1];
    char * modPath[MAX_PATH];
};

struct HeapInfo {

    ULONG_PTR address;
    SIZE_T blockSize;
    DWORD flags;

};

struct ProcessInfo
{
    /* Basic informations about the process */
    DWORD parentId;
    DWORD processId;
    DWORD cntThreads;
    char exeFile[MAX_PATH];

    /* Informations about the current process threads */
    std::vector<ThreadInfo> threads;

    /* Informations about the current process modules */
    std::vector<ModuleInfo> modules;

    /* Informations about the current process heap */
    std::vector<HeapInfo> heaps;
};

#define False 0x00
#define True 0x1

#define ISSET(a,b) ((a) & (b) == (b))
#define SNAPSHOT_THREADS 0x1
#define SNAPSHOT_MODULES 0x2
#define SNAPSHOT_HEAPS   0x4

/*
 * Get a snapshot of a process
 * arg1: the process id of the process
 * arg2: A pointer to the structure to fill with snapshots
 * arg3: Flags
 * return: TRUE if succeed and the struct res is filled with info, FALSE if it failed
 */
BOOL get_process_snapshot(DWORD processId, struct ProcessInfo * res, int flags);

// Interal functions
BOOL snapshot_process_threads(DWORD processId, PROCESSENTRY * pe32);
BOOL snapshot_process_modules(DWORD processId, PROCESSENTRY * pe32);
BOOL snapshot_process_heaps(DWORD processId, PROCESSENTRY * pe32);

