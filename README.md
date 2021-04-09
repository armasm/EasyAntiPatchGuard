# EasyAntiPatchGuard

------


##Support System

> *  >= Win8   (Win8 - Win10 21H4)


## How to use

#### 1.Build EasyAntiPatchGuard.sln
#### 2.Load EasyAntiPatchGuard.sys

## Detail
as we know, patchguard execution chain:

On pgentry -> 
CmpAppendDllSection(decrypt context)-> 
ExQueueWorkItem -> 
FsRtlMdlReadCompleteDevEx -> 
MmAllocateIndependentPages/ExAllocatePoolWithTag ->
re encrypt context -> 
Insert New context ->
FreePool/Page current context.

some pgentry(apc,dpc,..):
```c++
00 fffffd0c`0d40d868 fffff802`0a52410d     0xffffc405`84a020d8
01 fffffd0c`0d40d870 fffff802`0a467de5     nt!KiDispatchCallout+0x1cd
02 fffffd0c`0d40d8e0 fffff802`0a465b77     nt!KiDeliverApc+0x2b5
03 fffffd0c`0d40d990 fffff802`0a464d7f     nt!KiSwapThread+0x827
04 fffffd0c`0d40da40 fffff802`0a464623     nt!KiCommitThreadWait+0x14f
05 fffffd0c`0d40dae0 fffff802`0a5c7b12     nt!KeWaitForSingleObject+0x233
06 fffffd0c`0d40dbd0 fffff802`0a517e25     nt!PopIrpWorkerControl+0x22
07 fffffd0c`0d40dc10 fffff802`0a5fd0d8     nt!PspSystemThreadStartup+0x55
08 fffffd0c`0d40dc60 00000000`00000000     nt!KiStartSystemThread+0x28

06 ffffc381`53d460f0 fffff803`137cc052     nt!ExpTimerDpcRoutine$filt$1+0x493
07 ffffc381`53d46130 fffff803`137fe942     nt!_C_specific_handler+0xa2
08 ffffc381`53d461a0 fffff803`1372bf97     nt!RtlpExecuteHandlerForException+0x12
09 ffffc381`53d461d0 fffff803`1372ab86     nt!RtlDispatchException+0x297
0a ffffc381`53d468f0 fffff803`137f6912     nt!KiDispatchException+0x186
0b ffffc381`53d46fb0 fffff803`137f68e0     nt!KxExceptionDispatchOnExceptionStack+0x12
0c ffffd88a`c62452c8 fffff803`13807ba5     nt!KiExceptionDispatchOnExceptionStackContinue
0d ffffd88a`c62452d0 fffff803`138038e0     nt!KiExceptionDispatch+0x125
0e ffffd88a`c62454b0 fffff803`1380014d     nt!KiGeneralProtectionFault+0x320
0f ffffd88a`c6245640 fffff803`13800182     nt!KiCustomRecurseRoutine0+0xd
10 ffffd88a`c6245670 fffff803`13873ce9     nt!KiCustomAccessRoutine0+0x22
11 ffffd88a`c62456a0 fffff803`1360781e     nt!ExpTimerDpcRoutine+0x155d29
12 ffffd88a`c6245860 fffff803`13606b04     nt!KiExecuteAllDpcs+0x30e
13 ffffd88a`c62459d0 fffff803`137f95ee     nt!KiRetireDpcList+0x1f4
14 ffffd88a`c6245c60 00000000`00000000     nt!KiIdleLoop+0x9e


0d fffff802`29b82b80 fffff802`247cc154     nt!CmpEnableLazyFlushDpcRoutine$fin$1+0x16a
0e fffff802`29b82be0 fffff802`247fe9c2     nt!_C_specific_handler+0x1a4
0f fffff802`29b82c50 fffff802`2472c484     nt!RtlpExecuteHandlerForUnwind+0x12
10 fffff802`29b82c80 fffff802`24720574     nt!RtlUnwindEx+0x2c4
11 fffff802`29b833a0 fffff802`247cd6d3     nt!RtlUnwind+0xa4
12 fffff802`29b83900 fffff802`24810701     nt!local_unwind+0x23
13 fffff802`29b83930 fffff802`247cc154     nt!CmpEnableLazyFlushDpcRoutine$fin$0+0x34
14 fffff802`29b83970 fffff802`247fe9c2     nt!_C_specific_handler+0x1a4
15 fffff802`29b839e0 fffff802`2472c484     nt!RtlpExecuteHandlerForUnwind+0x12
16 fffff802`29b83a10 fffff802`247cc095     nt!RtlUnwindEx+0x2c4
17 fffff802`29b84130 fffff802`247fe942     nt!_C_specific_handler+0xe5
18 fffff802`29b841a0 fffff802`2472bf97     nt!RtlpExecuteHandlerForException+0x12
19 fffff802`29b841d0 fffff802`2472ab86     nt!RtlDispatchException+0x297
1a fffff802`29b848f0 fffff802`247f6912     nt!KiDispatchException+0x186
1b fffff802`29b84fb0 fffff802`247f68e0     nt!KxExceptionDispatchOnExceptionStack+0x12
1c fffff802`29b6f348 fffff802`24807ba5     nt!KiExceptionDispatchOnExceptionStackContinue
1d fffff802`29b6f350 fffff802`248038e0     nt!KiExceptionDispatch+0x125
1e fffff802`29b6f530 fffff802`247fff0d     nt!KiGeneralProtectionFault+0x320
1f fffff802`29b6f6c0 fffff802`247ffd8d     nt!KiCustomRecurseRoutine7+0xd
20 fffff802`29b6f6f0 fffff802`247ffa8d     nt!KiCustomRecurseRoutine6+0xd
21 fffff802`29b6f720 fffff802`247ffb4d     nt!KiCustomRecurseRoutine5+0xd
22 fffff802`29b6f750 fffff802`247ffb82     nt!KiCustomRecurseRoutine4+0xd
23 fffff802`29b6f780 fffff802`24723877     nt!KiCustomAccessRoutine4+0x22
24 fffff802`29b6f7b0 fffff802`24644f12     nt!CmpEnableLazyFlushDpcRoutine+0x97
25 fffff802`29b6f8e0 fffff802`24606eed     nt!KiProcessExpiredTimerList+0x172
26 fffff802`29b6f9d0 fffff802`247f95ee     nt!KiRetireDpcList+0x5dd
27 fffff802`29b6fc60 00000000`00000000     nt!KiIdleLoop+0x9e
```

execute pgentry:
```c++
 {
  v32 = *(_QWORD *)pg_entry ^ 0x85131481131482Ei64;
  *(_DWORD *)pg_entry = 0xAD1B6FF5;
  *(_DWORD *)pg_entry ^= 0xBC2A27DB;  //decrypt to xor     qword ptr cs:[rcx],rdx
  ((void (__fastcall *)(unsigned __int64, __int64, _QWORD, _QWORD))pg_entry)(pg_entry, v32, 0i64, 0i64);
 }
```
asm:
```asm
.text:00000001403240F9   mov     rdx, [rbp+arg_0]
.text:00000001403240FD   xor     eax, 0BC2A27DBh
.text:0000000140324102   mov     [r10], eax
.text:0000000140324105   mov     rax, r10
.text:0000000140324108   call    _guard_dispatch_icall
```
  
CmpAppendDllSection and FsRtlMdlReadCompleteDevEx executes very fast. Most of the time is the encrypted context state. so we hook _guard_dispatch_icall and check target is 
2e483111        xor     qword ptr cs:[rcx],rdx
```asm
cmp     dword ptr ds:[rax], 0x1131482E 
je      ret:
jmp     rax
ret
```

## Result
```asm
0: kd> u guard_dispatch_icall
nt!guard_dispatch_icall:
fffff803`3c7fe620 81382e483111    cmp     dword ptr [rax],1131482Eh
fffff803`3c7fe626 7402            je      nt!guard_dispatch_icall+0xa (fffff803`3c7fe62a)
fffff803`3c7fe628 ffe0            jmp     rax
fffff803`3c7fe62a c3              ret
fffff803`3c7fe62b 8d7a00          lea     edi,[rdx]
fffff803`3c7fe62e 0000            add     byte ptr [rax],al
fffff803`3c7fe630 4d85db          test    r11,r11
fffff803`3c7fe633 741c            je      nt!guard_dispatch_icall+0x31 (fffff803`3c7fe651)
0: kd> bp fffff803`3c7fe62a
0: kd> g
nt!DbgBreakPointWithStatus:
fffff803`3c7fd920 cc              int     3
0: kd> g
Breakpoint 2 hit
nt!guard_dispatch_icall+0xa:
fffff803`3c7fe62a c3              ret
0: kd> kn
 # Child-SP          RetAddr               Call Site
00 fffff803`42b838a8 fffff803`3c810bcf     nt!guard_dispatch_icall+0xa
01 fffff803`42b838b0 fffff803`3c7cc154     nt!IopIrpStackProfilerDpcRoutine$fin$1+0x153
02 fffff803`42b83910 fffff803`3c7f16f2     nt!_C_specific_handler+0x1a4
03 fffff803`42b83980 fffff803`3c7fe9c2     nt!_GSHandlerCheck_SEH+0x6a
04 fffff803`42b839b0 fffff803`3c72c484     nt!RtlpExecuteHandlerForUnwind+0x12
05 fffff803`42b839e0 fffff803`3c7cc095     nt!RtlUnwindEx+0x2c4
06 fffff803`42b84100 fffff803`3c7f16f2     nt!_C_specific_handler+0xe5
07 fffff803`42b84170 fffff803`3c7fe942     nt!_GSHandlerCheck_SEH+0x6a
08 fffff803`42b841a0 fffff803`3c72bf97     nt!RtlpExecuteHandlerForException+0x12
09 fffff803`42b841d0 fffff803`3c72ab86     nt!RtlDispatchException+0x297
0a fffff803`42b848f0 fffff803`3c7f6912     nt!KiDispatchException+0x186
0b fffff803`42b84fb0 fffff803`3c7f68e0     nt!KxExceptionDispatchOnExceptionStack+0x12
0c fffff803`42b6f1c8 fffff803`3c807ba5     nt!KiExceptionDispatchOnExceptionStackContinue
0d fffff803`42b6f1d0 fffff803`3c8038e0     nt!KiExceptionDispatch+0x125
0e fffff803`42b6f3b0 fffff803`3c7ffa8d     nt!KiGeneralProtectionFault+0x320
0f fffff803`42b6f540 fffff803`3c7ffb4d     nt!KiCustomRecurseRoutine5+0xd
10 fffff803`42b6f570 fffff803`3c7ffe4d     nt!KiCustomRecurseRoutine4+0xd
11 fffff803`42b6f5a0 fffff803`3c7ffccd     nt!KiCustomRecurseRoutine3+0xd
12 fffff803`42b6f5d0 fffff803`3c7ffd02     nt!KiCustomRecurseRoutine2+0xd
13 fffff803`42b6f600 fffff803`3c723b1a     nt!KiCustomAccessRoutine2+0x22
14 fffff803`42b6f630 fffff803`3c60781e     nt!IopIrpStackProfilerDpcRoutine+0x23a
15 fffff803`42b6f860 fffff803`3c606b04     nt!KiExecuteAllDpcs+0x30e
16 fffff803`42b6f9d0 fffff803`3c7f95ee     nt!KiRetireDpcList+0x1f4
17 fffff803`42b6fc60 00000000`00000000     nt!KiIdleLoop+0x9e
0: kd> g
Breakpoint 2 hit
nt!guard_dispatch_icall+0xa:
fffff803`3c7fe62a c3              ret
0: kd> kn
 # Child-SP          RetAddr               Call Site
00 fffff803`42b840e8 fffff803`3c80fa6f     nt!guard_dispatch_icall+0xa
01 fffff803`42b840f0 fffff803`3c7cc052     nt!CmpLazyFlushDpcRoutine$filt$1+0x517
02 fffff803`42b84130 fffff803`3c7fe942     nt!_C_specific_handler+0xa2
03 fffff803`42b841a0 fffff803`3c72bf97     nt!RtlpExecuteHandlerForException+0x12
04 fffff803`42b841d0 fffff803`3c72ab86     nt!RtlDispatchException+0x297
05 fffff803`42b848f0 fffff803`3c7f6912     nt!KiDispatchException+0x186
06 fffff803`42b84fb0 fffff803`3c7f68e0     nt!KxExceptionDispatchOnExceptionStack+0x12
07 fffff803`42b6f368 fffff803`3c807ba5     nt!KiExceptionDispatchOnExceptionStackContinue
08 fffff803`42b6f370 fffff803`3c8038e0     nt!KiExceptionDispatch+0x125
09 fffff803`42b6f550 fffff803`3c7ffa8d     nt!KiGeneralProtectionFault+0x320
0a fffff803`42b6f6e0 fffff803`3c7ffac2     nt!KiCustomRecurseRoutine5+0xd
0b fffff803`42b6f710 fffff803`3c8729fb     nt!KiCustomAccessRoutine5+0x22
0c fffff803`42b6f740 fffff803`3c644f12     nt!CmpLazyFlushDpcRoutine+0x1574ab
0d fffff803`42b6f8e0 fffff803`3c6072e9     nt!KiProcessExpiredTimerList+0x172
0e fffff803`42b6f9d0 fffff803`3c7f95ee     nt!KiRetireDpcList+0x9d9
0f fffff803`42b6fc60 00000000`00000000     nt!KiIdleLoop+0x9e
```
## Todo

- [ ] Wait in a loop until there is no pg workitem running
```c++
for(;;){
    if(find_decrypt_context() == 0)
       break;
    KeSleep(100);
}
```
- [x] fix KiTimerDispatch entry check
- [x] fix KiDpcDispatch entry check
