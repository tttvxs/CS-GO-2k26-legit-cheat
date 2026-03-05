#pragma once
#include <cstdint>
#include <vector>
namespace memory {
    template<typename T>
    inline T GetInterface(const char* dllname, const char* interfacename) {
        auto hModule = GetModuleHandleA(dllname);
        if (!hModule) return nullptr;

        auto CreateInterface = (void* (*)(const char*, DWORD*))(GetProcAddress(hModule, "CreateInterface"));
        if (!CreateInterface) return nullptr;

        auto ret = reinterpret_cast<T>(CreateInterface(interfacename, nullptr));

        printf("[%s] from [%s] is [0x%llx]\n", interfacename, dllname, ret);

        return ret;
    }

    inline void**& GetVtable(void* inst) {
        return *reinterpret_cast<void***>(inst);
    }

    inline uint8_t* PatternScan(const char* szModuleName, const char* signature) {
        HMODULE module = GetModuleHandleA(szModuleName);
        static auto pattern_to_byte = [](const char* pattern)
            {
                auto bytes = std::vector<int>{};
                auto start = const_cast<char*>(pattern);
                auto end = const_cast<char*>(pattern) + strlen(pattern);

                for (auto current = start; current < end; ++current) {
                    if (*current == '?') {
                        ++current;
                        if (*current == '?')
                            ++current;
                        bytes.push_back(-1);
                    }
                    else {
                        bytes.push_back(strtoul(current, &current, 16));
                    }
                }
                return bytes;
            };

        auto dosHeader = (PIMAGE_DOS_HEADER)module;
        auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)module + dosHeader->e_lfanew);

        auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
        auto patternBytes = pattern_to_byte(signature);
        auto scanBytes = reinterpret_cast<std::uint8_t*>(module);

        for (auto i = 0ul; i < sizeOfImage - patternBytes.size(); ++i) {
            bool found = true;
            for (auto j = 0ul; j < patternBytes.size(); ++j) {
                if (scanBytes[i + j] != patternBytes.data()[j] && patternBytes.data()[j] != -1) {
                    found = false;
                    break;
                }
            }
            if (found) {
                return &scanBytes[i];
            }
        }
        return nullptr;
    }
}
