#include "sdk.h"
#include "deps/imgui/imgui.h"
#include "deps/imgui/imgui_impl_win32.h"
#include "deps/imgui/imgui_impl_dx9.h"
#include <d3d9.h>
#include "deps/minhook/include/MinHook.h"
//#include <d3dx9.h>
#pragma comment(lib, "d3d9.lib")
//#pragma comment(lib, "d3dx9.lib")

CHLClient* Client;
IClientEntityList* ClientEntityList;
IVDebugOverlay* DebugOverlay;

//std::string TypeToString(const RecvProp* prop) {
//	switch (prop->GetType()) {
//	case DPT_Int: return "int";
//	case DPT_Float: return "float";
//	case DPT_Vector: return "vec3";
//	case DPT_VectorXY: return "vec2";
//	case DPT_String: return "char*";
//	case DPT_Array: return "array";
//	case DPT_DataTable: return "RecvTable*";
//	default: return "unknown";
//	}
//}
//
//struct netvar {
//	SendPropType type;
//	std::string cl_name;
//	std::string var_name;
//	const RecvProp* prop;
//	int offset;
//};
//
//std::vector<netvar> dumped_netvars;
//std::vector<std::string> dumped_classes;
//
//std::ofstream dump;
//void dump_client_class(const char* network_name, RecvTable* table, uint32_t offset) {
//	for (int i = 0; i < table->m_nProps; i++) {
//		const RecvProp* prop = &table->m_pProps[i];
//		if (!prop || !prop->m_pVarName || isdigit(prop->m_pVarName[0])) continue;
//		if (!strcmp(prop->m_pVarName, "baseclass")) continue;
//		if (prop->m_RecvType == DPT_DataTable && prop->GetDataTable() && ((RecvTable*)prop->GetDataTable())->GetName()[0] == 'D') {
//			dump_client_class(network_name, (RecvTable*)prop->GetDataTable(), offset + prop->GetOffset());
//		}
//		dumped_classes.push_back(network_name);
//		dumped_netvars.push_back({ prop->GetType(), network_name, prop->m_pVarName, prop,  prop->GetOffset() });
//		//dump << type2text(prop->GetType()) << " | " << network_name << " -> " << prop->m_pVarName << " : 0x" << std::hex << offset + prop->GetOffset() << std::dec << std::endl;
//	}
//}
//
//void netvars_dump() {
//	dump.open("C:\\Users\\gfhhfghjfgh\\Desktop\\csgo_2k26\\Release\\dump.txt");
//
//	for (ClientClass* client_class = Client->GetAllClasses(); client_class; client_class = client_class->m_pNext) {
//		if (client_class) dump_client_class(client_class->m_pNetworkName, client_class->m_pRecvTable, 0);
//	}
//
//	std::unordered_set<std::string> processed_classes;
//
//	for (int j = 0; j < dumped_classes.size(); j++) {
//		auto clas = dumped_classes[j];
//		if (processed_classes.count(clas)) continue;
//
//		processed_classes.insert(clas);
//
//		dump << "class " << clas << " { \n public: \n";
//		for (int i = 0; i < dumped_netvars.size(); i++) {
//			auto& netvar = dumped_netvars[i];
//
//			if (netvar.cl_name.empty() || netvar.var_name.empty()) continue;
//			if (netvar.cl_name != clas) continue;
//
//			auto type_name = TypeToString(netvar.prop);
//			if (type_name.empty()) continue;
//
//			dump << "   VAROFF(" << netvar.var_name << ", " << type_name << ", 0x" << std::hex << netvar.offset << std::dec << ");\n";
//			//dump << "   NETVAR(" << netvar.var_name << ", " << type_name << ", \"" << netvar.cl_name << "\", \"" << netvar.var_name << "\");\n";
//		}
//
//		dump << "};\n\n";
//	}
//
//	dump.close();
//}

bool opened = true;
bool unhook = false;
bool init = false;
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
typedef long(__stdcall* EndScene)(LPDIRECT3DDEVICE9);
typedef LRESULT(CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);
EndScene oEndScene = NULL;
WNDPROC oWndProc;
static HWND window = NULL;

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (true && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam)) return true;
	if (GetAsyncKeyState(VK_DELETE) & 1) opened = !opened;	
	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

long __stdcall hkEndScene(LPDIRECT3DDEVICE9 pDevice) {
	if (!init) {
		window = FindWindowA("Valve001", "Counter-Strike: Global Offensive - Direct3D 9");
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
		ImGui_ImplWin32_Init(window);
		ImGui_ImplDX9_Init(pDevice);
		oWndProc = (WNDPROC)SetWindowLongPtr(window, GWL_WNDPROC, (LONG_PTR)WndProc);
		init = true;
	}

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	if (opened) {
		ImGui::Begin("ImGui Window");

		unhook = ImGui::Button("Unhook");

		ImGui::End();
	}

	for (int i = 0; i < ClientEntityList->GetHighestEntityIndex(); i++) {
		auto entity = (CBaseEntity*)ClientEntityList->GetClientEntity(i);
		if (!entity) continue;
		if (entity->GetClientClass()->m_ClassID == 40) {//player - 40 | c4 - 34 | world - 275
			auto pos = entity->GetAbsOrigin();
			Vector screen;
			if (DebugOverlay->ScreenPosition(pos, screen)) {
				ImGui::GetBackgroundDrawList()->AddLine({ 0,0 }, { screen.x, screen.y }, ImColor(255, 255, 255));
			}
			//printf("%s | [%.2f, %.2f, %.2f]\n", entity->GetClientClass()->GetName(), pos.x, pos.y, pos.z);
		}
	}

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
	return oEndScene(pDevice);
}

void main(HINSTANCE hinst) {
	AllocConsole();
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);

    Client = memory::GetInterface<CHLClient*>("client.dll", "VClient018");
    ClientEntityList = memory::GetInterface<IClientEntityList*>("client.dll", "VClientEntityList003");
    DebugOverlay = memory::GetInterface<IVDebugOverlay*>("engine.dll", "VDebugOverlay004");
	auto dx9device = **reinterpret_cast<IDirect3DDevice9***>(memory::PatternScan("shaderapidx9.dll", "A1 ? ? ? ? 50 8B 08 FF 51 0C") + 0x1);

	MH_Initialize();

	MH_CreateHook(memory::GetVtable(dx9device)[42], &hkEndScene, (void**)&oEndScene);

	MH_EnableHook(MH_ALL_HOOKS);

	DebugOverlay->AddCapsuleOverlay

    while (!unhook) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
	printf("Unhooked!\n");
	MH_DisableHook(MH_ALL_HOOKS);
    FreeLibraryAndExitThread(hinst, 0);
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    switch (fdwReason) {
    case DLL_PROCESS_ATTACH:
        std::thread(main, hinstDLL).detach();
        break;
    case DLL_PROCESS_DETACH:
        if (lpvReserved != nullptr) break;
        break;
    }
    return TRUE;
}