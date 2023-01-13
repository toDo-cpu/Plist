#include "plist.h"

BOOL get_process_snapshot(DWORD processId, struct ProcessInfo * res, SHORT flags) {

    // Take a snapshot of processes
    HANDLE hProcessSnapshots = INVALID_HANDLE_VALUE;
    hProcessSnapshots = CreateToolhelp32Snapshot(
        TH32CS_SNAPPROCESS,
        0
    )
    if (hProcessSnapshots == INVALID_HANDLE_VALUE) {
        return false;
    }

    //Find the process snapshot
    PROCESSENTRY32 pe32;
    pe32->dwSize = sizeof( PROCESSENTRY32 );

    if(!Process32First(hProcessSnapshots, &pe32)) {
        return False;
    }
    do {
        if( processId == pe32->th32ProcessPid) {

                // Fill the structure
                res->parentId = pe32.th32ParentProcessID;
                res->processId = pe32.th32ProcessID;
                res->cntThreads = pe32.cntThreads;
                strncpy(&res->exeFile, &pe32.szexeFile, MAX_PATH);
                res->exeFile[MAX_PATH-1] = '\x00';

                // If needed, take a snasphots of HEAP, MODULES and threads
                if (ISSET(flags, SNAPSHOT_THREADS)) {
                        if(!snapshot_process_threads(processId, res)) {
                        return False
                        }
                }
                if (ISSET(flags, SNAPSHOT_MODULES)) {
                        if(!snapshot_process_modules(processId, res)) {
                        return False
                        }
                }
                if (ISSET(flags, SNAPSHOT_HEAPS)) {
                        if(!snapshot_process_heaps(processId, res)) {
                        return False
                        }
                }

                CloseHandle(hProcessSnapshots);
                return TRUE;
        }
    } while(Process32Next( hProcessSnapshots, &pe32))

    return FALSE;
}

BOOL snapshot_process_threads(DWORD processId, PROCESSENTRY * pe32) {

    HANDLE hSnapshotsThreads = INVALID_HANDLE_VALUE;
    hSnapshotsThreads = CreateToolhelp32Snapshot(
        TH32CS_SNAPTHREADLIST,
        processID,
    )
    if (hSnapshotsThreads == INVALID_HANDLE_VALUE) {
        CloseHandle(hSnapshotsThreads)
        return False;
    }

    struct ThreadInfo th;
    THREADENTRY32 te32;
    te32.dwsize = sizeof( THREADENTRY32 );

    if (!Thread32First(hSnapshotsThreads, &te32)) {
        return FALSE;
    }

    do {
        if (te32.th32OwnerProcessID == processId) {
            memset(&th, 0x00, sizeof(th));
            th.threadId = te32.th32ThreadId;
            th.priority = te32.tpBasePri;
            res->threads.push_back(th);
        }
    } while(Thread32Next(hSnapshotsThreds, &te32))

    CloseHanlde(hSnapshotsThreads);
    return TRUE
}

BOOL snapshot_process_modules(DWORD processId, PROCESSENTRY * pe32) {

    HANDLE hSnapshotsModules = INVALID_HANDLE_VALUE;
    hSnapshotsModules = CreateToolhelp32Snapshot(
        TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32,
        processID,
    )
    if (hSnapshotsModules == INVALID_HANDLE_VALUE) {
        CloseHandle(hSnapshotsModules)
        return False;
    }

    struct ModuleInfo mi;
    MODULEENTRY32 me32;
    me32.dwsize = sizeof( MODULEENTRY32 );

    if (!Module32First(hSnapshotsModules, &me32)) {
        CloseHandle(hSnapshotsModule);
        return FALSE;
    }

    do {
        memset(&mi, 0x00, sizeof(mi));
        mi.modBaseAddr = me32.modBaseAddr;
        mi.modSize = me32.modBaseSize;
        strncpy(&mi.modName, &me32.szModule, MAX_MODULE_NAME32 + 1);
        mi.modName[MAX_MODULE_NAME] = '\x00';
        strncpy(&mi.modPath, &me32.szExePath, MAX_PATH);
        mi.modPath[MAX_PATH] = "\x00";
        res->modules.push_back(mi);
   } while(Heap32Next(hSnapshotsModules, &me32))

    CloseHanlde(hSnapshotsModules);
    return TRUE;

}
BOOL snapshot_process_heaps(DWORD processId, PROCESSENTRY * pe32) {
    HANDLE hSnapshotsHeaps = INVALID_HANDLE_VALUE;
    hSnapshotsHeaps = CreateToolhelp32Snapshot(
        TH32CS_SNAPHEAPSLIST,
        processID,
    )
    if (hSnapshotsHeaps == INVALID_HANDLE_VALUE) {
        CloseHandle(hSnapshotsHeaps)
        return False;
    }

    struct HeapInfo hi;
    HEAPENTRY32 he32;
    he32.dwsize = sizeof( HEAPENTRY32 );

    if (!Heap32First(hSnapshotsHeaps, &me32)) {
        CloseHandle(hSnapshotsHeap);
        return FALSE;
    }

    do {
        memset(&hi, 0x00, sizeof(hi));
        hi.address = he32.dwAddress;
        hi.blocksize = he32.dwBlockSize;
        hi.flags = he32.dwFlags;
        res->heaps.push_back(hi);
   } while(Heap32Next(hSnapshotsHeaps, &he32))

    CloseHanlde(hSnapshotsHeaps);
    return TRUE;
}
