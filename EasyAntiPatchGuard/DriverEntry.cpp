#include <ntifs.h>
#include <ntimage.h>
#include <intrin.h>
#define uint64_t unsigned long long
#define uint32_t unsigned long
#define uint16_t unsigned short


extern "C" uintptr_t GetKernelBase();

typedef struct _RUNTIME_FUNCTION
{
	ULONG BeginAddress;
	ULONG EndAddress;
	ULONG UnwindData;
} RUNTIME_FUNCTION, *PRUNTIME_FUNCTION;

extern "C"
PRUNTIME_FUNCTION
RtlLookupFunctionEntry(
	IN ULONG64 ControlPc,
	OUT PULONG64 ImageBase,
	IN OUT PVOID HistoryTable OPTIONAL
);


bool easy_anti_patchguard(uintptr_t search_base) {
	const auto dos_header = reinterpret_cast<PIMAGE_DOS_HEADER>(search_base);
	const auto pNtHeader = reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<PUCHAR>(search_base) + dos_header->
		e_lfanew);
	const auto section_size = pNtHeader->FileHeader.NumberOfSections;
	const auto sections_array = reinterpret_cast<PIMAGE_SECTION_HEADER>(reinterpret_cast<PUCHAR>(pNtHeader) + sizeof(
		IMAGE_NT_HEADERS));

	uint32_t KiTimerDispatch[3]{0};

	for (auto i = 0; i < section_size; i++) {
		//INITKDBG
		if (*reinterpret_cast<uint64_t*>(sections_array[i].Name) == 0x4742444B54494E49) {
			const auto scan_base = (PUCHAR)(search_base) + sections_array[i].VirtualAddress;
			const auto scan_size = max(sections_array[i].SizeOfRawData, sections_array[i].Misc.VirtualSize);
			for (int i = 0; i < scan_size - 2; i++) {
				const auto start_address = (ULONG64)scan_base + i;
				if (*(uint16_t*)start_address == 0x9C48) {
					//pushfq
					ULONG64 image_base = 0;
					if (const auto runtime_table = RtlLookupFunctionEntry(start_address, &image_base, 0)) {
						const auto va = image_base + runtime_table->BeginAddress;
						if (start_address - va <= 0x20) {

							KiTimerDispatch[0] = *(uint32_t*)(va);
							KiTimerDispatch[1] = *(uint32_t*)(va + sizeof(uint32_t));
							KiTimerDispatch[2] = *(uint32_t*)(va + sizeof(uint32_t) * 2);
							DbgPrint("KiTimerDispatch :%p \n", va);
							break;
						}
					}
				}

			}
			break;
		}
	}

	if (KiTimerDispatch[0] == 0) {
		return false;
	}

	for (auto i = 0; i < section_size; i++) {
		//.text
		if (*reinterpret_cast<uint32_t*>(sections_array[i].Name) == 0x7865742E) {
			const auto scan_base = (PUCHAR)(search_base) + sections_array[i].VirtualAddress;
			const auto scan_size = max(sections_array[i].SizeOfRawData, sections_array[i].Misc.VirtualSize);
			for (int i = 0; i < scan_size - 10; i++) {
				const auto p1 = *reinterpret_cast<uint32_t*>(scan_base + i) & 0x00ffffff;
				const auto p2 = *reinterpret_cast<uint32_t*>(scan_base + i + 6) & 0xffffff00;
				if (p1 == 0x1D8B4C && p2 == 0xC0854800) {
					const auto _guard_icall_handler = scan_base + i;
					DbgPrint("_guard_icall_handler:%p\n", _guard_icall_handler);

					/*
					cmp     dword ptr ds:[rax], 0x1131482E   //     xor     qword ptr cs:[rcx],rdx
					je      ret
					cmp     dword ptr ds:[rax], 0x48513148   //     KiDpcDispatch
					je      ret
					cmp     dword ptr ds:[rax], 0x11111111   //		KiTimerDispatch[0]
					jne     ok
					cmp     qword ptr ds:[rax+0x4], 0x22222222  //	KiTimerDispatch[1]
					jne     ok
					cmp     qword ptr ds:[rax+0x8], 0x33333333  //	KiTimerDispatch[2]
					jne     ok
					ret
					jmp     rax
					 */

					unsigned char patch_code[] = {
						0x81, 0x38, 0x2E, 0x48, 0x31, 0x11, 0x74, 0x19, 0x81, 0x38, 0x48, 0x31, 0x51, 0x48, 0x74,
						0x11,
						0x81, 0x38, 0x11, 0x11, 0x11, 0x11, 0x75, 0x0A, 0x81, 0x78, 0x04, 0x22, 0x22, 0x22, 0x22,
						0x75,
						0x01, 0x81, 0x78, 0x08, 0x22, 0x22, 0x22, 0x22, 0x75, 0xF8, 0xC3, 0xFF, 0xE0
					};
					*(uint32_t*)(&patch_code[0x12]) = KiTimerDispatch[0];
					*(uint32_t*)(&patch_code[0x1B]) = KiTimerDispatch[1];
					*(uint32_t*)(&patch_code[0x24]) = KiTimerDispatch[2];
					const auto irql = __readcr8();
					KeRaiseIrqlToDpcLevel();
					__writecr0(__readcr0() & 0xfffffffffffeffff);

					memcpy(_guard_icall_handler, patch_code, sizeof(patch_code));

					__writecr0(__readcr0() | 0x10000);
					__writecr8(irql);
					break;
				}
			}
			break;
		}
	}
	return true;
}

EXTERN_C
NTSTATUS DriverEntry(
	IN PDRIVER_OBJECT DriverObject,
	IN PUNICODE_STRING RegistryPath
) {
	easy_anti_patchguard(GetKernelBase());
	return STATUS_UNSUCCESSFUL;
}
