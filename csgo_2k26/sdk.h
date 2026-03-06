#pragma once
#include<Windows.h>
#include <thread>
#include "utils/memory.h"
#include "utils/vector.h"
#include <unordered_set>
#include "dt/dt_recv.h"
#include <string>
#include <fstream>
#include <array>
#include <sstream>

#define VAROFF(_name, type, off) inline type &_name() { return *reinterpret_cast<type *>(reinterpret_cast<uintptr_t>(this) + off); }

enum ShouldTransmitState_t {
	SHOULDTRANSMIT_START = 0,	// The entity is starting to be transmitted (maybe it entered the PVS).
	SHOULDTRANSMIT_END		// Called when the entity isn't being transmitted by the server.
};

enum DataUpdateType_t {
	DATA_UPDATE_CREATED = 0,	// indicates it was created +and+ entered the pvs
	DATA_UPDATE_DATATABLE_CHANGED,
	DATA_UPDATE_POST_UPDATE,
};

typedef unsigned short ClientShadowHandle_t;

enum {
	CLIENTSHADOW_INVALID_HANDLE = (ClientShadowHandle_t)~0
};

enum ShadowType_t {
	SHADOWS_NONE = 0,
	SHADOWS_SIMPLE,
	SHADOWS_RENDER_TO_TEXTURE,
	SHADOWS_RENDER_TO_TEXTURE_DYNAMIC,	// the shadow is always changing state
	SHADOWS_RENDER_TO_DEPTH_TEXTURE,
	SHADOWS_RENDER_TO_TEXTURE_DYNAMIC_CUSTOM,	// changing, and entity uses custom rendering code for shadow
};

struct AppSystemInfo_t {
	const char* m_pModuleName;
	const char* m_pInterfaceName;
};

enum InitReturnVal_t {
	INIT_FAILED = 0,
	INIT_OK,
	INIT_LAST_VAL,
};

enum AppSystemTier_t {
	APP_SYSTEM_TIER0 = 0,
	APP_SYSTEM_TIER1,
	APP_SYSTEM_TIER2,
	APP_SYSTEM_TIER3,
	APP_SYSTEM_TIER_OTHER,
};

class CBaseHandle;
struct model_t;

class IBaseInterface {
public:
	virtual	~IBaseInterface() {}
};

class CClientThinkHandlePtr;
typedef CClientThinkHandlePtr* ClientThinkHandle_t;
typedef void* (*CreateInterfaceFn)(const char* pName, int* pReturnCode);
typedef void* (*InstantiateInterfaceFn)();
typedef class IClientNetworkable* (*CreateClientClassFn)(int entnum, int serialNum);
typedef class IClientNetworkable* (*CreateEventFn)();

class CUtlAbstractDelegate;

class IAppSystem {
public:
	virtual bool Connect(CreateInterfaceFn factory) = 0;
	virtual void Disconnect() = 0;
	virtual void* QueryInterface(const char* pInterfaceName) = 0;
	virtual InitReturnVal_t Init() = 0;
	virtual void Shutdown() = 0;
	virtual const AppSystemInfo_t* GetDependencies() = 0;
	virtual AppSystemTier_t GetTier() = 0;
	virtual void Reconnect(CreateInterfaceFn factory, const char* pInterfaceName) = 0;
	virtual bool IsSingleton() = 0;
};

class IZip {
public:
	virtual void Reset() = 0;
	virtual void AddFileToZip(const char* relativename, const char* fullpath) = 0;
	virtual bool FileExistsInZip(const char* pRelativeName) = 0;
	virtual bool ReadFileFromZip(const char* pRelativeName, bool bTextMode, void* buf) = 0;
	virtual bool ReadFileFromZip(HANDLE hFile, const char* pRelativeName, bool bTextMode, void* buf) = 0;
	virtual void RemoveFileFromZip(const char* relativename) = 0;
	virtual int GetNextFilename(int id, char* pBuffer, int bufferSize, int& fileSize) = 0;
	virtual void PrintDirectory(void) = 0;
	virtual unsigned int EstimateSize(void) = 0;
	virtual void AddBufferToZip(const char* relativename, void* data, int length, bool bTextMode) = 0;
	virtual void SaveToBuffer(void* outbuf) = 0;
	virtual void SaveToDisk(FILE* fout) = 0;
	virtual void SaveToDisk(HANDLE hFileOut) = 0;
	virtual void ParseFromBuffer(void* buffer, int bufferlength) = 0;
	virtual HANDLE ParseFromDisk(const char* pFilename) = 0;
	virtual void ForceAlignment(bool aligned, bool bCompatibleFormat, unsigned int alignmentSize = 0) = 0;
	virtual unsigned int GetAlignment() = 0;
	virtual void SetBigEndian(bool bigEndian) = 0;
	virtual void ActivateByteSwapping(bool bActivate) = 0;
};


namespace panorama {
	class IUIPanel;
	class IUIEvent;
	class IUIEngineFrameListener;
	class IUIPanelStyle;
	class IUIJSObject;
	class IUIWindow;
	class IUISettings;
	class IUITextLayout;
	class IUIInputEngine;
	class IUILocalize;
	class IUISoundSystem;
	class IUISettings;
	class IUILayoutManager;
	class IUIFileSystem;
	class IUIPanelClient;

	// E Prefix for enums?
	class EFontStyle;
	class EFontWeight;
	class ETextAlign;
	class EPanelRepaint;
	class EStyleRepaint;
	class EStyleFlags;
	class EFocusMoveDirection;
	class EMouseCanActivate;
	class EAnimationTimingFunction;
	class EAnimationDirection;

	class CUILength;
	class CLayoutFile;
	typedef unsigned short CPanoramaSymbol;
	class PanelHandle_t;
	class CJSONWebAPIParams;
	class UIEventFactory;
	class EPanelEventSource_t;
	class CPanel2DFactory;
	class RegisterJSType_t;
	class RegisterJSScopeInfo_t;
	class RegisterJSEntryInfo_t;


	// Returned by the CreatePanel functions. TODO: Look at panelRefs this might be that.
	class PanelWrapper {
	public:
		void* vtable;
		IUIPanel* panel;
	};

	struct ScrollBehavior_t;

	struct PanelArraySlot
	{
		IUIPanel* panel;
		void* unk1;
		void* unk2;
		uint32_t someSymbol; // Might be a CPanoramaSymbol
		uint32_t boolean; // Looks to normally be 1, maybe IsVisible or IsFinishedSettingUp
	};

	struct PanelArray
	{
		void* _pad; // all F's
		uint32_t size; // I don't think this is right.
		uint32_t unk;
		//TODO: This array is not fixed-size! Something like: CUtlVector
		//Make sure to check panel pointer when using this!
		std::array<PanelArraySlot, 1024> slots;
	};

	class IUIPanel
	{
	public:
		virtual void DESTROY1(void) = 0;
#ifndef WIN32
		virtual void DESTROY2(void) = 0;
#endif
		virtual void Initialize(IUIWindow*, IUIPanel*, const char*, unsigned int) = 0;
		virtual void InitClonedPanel(IUIPanel* other) = 0;
		virtual void RegisterEventHandlersOnPanel2DType(CPanoramaSymbol) = 0;
		virtual void Shutdown(void) = 0;
		virtual void FirePanelLoadedEvent(void) = 0;
		virtual void SetClientPtr(IUIPanelClient*) = 0;
		virtual IUIPanelClient* ClientPtr() = 0;
		virtual void SetID(const char* id) = 0;
		virtual const char* GetID(void) = 0; // ID in the XML, (EX: <Panel id="ShopCourierControls">)
		virtual void GetPanelType(void) = 0;
		virtual bool HasID() = 0;
		virtual void unk() = 0;
		virtual void unk2() = 0;
		virtual void unk3() = 0;
		virtual void unk4() = 0;
		virtual void unk5() = 0;
		virtual bool LoadLayout(const char* pathToXML, bool, bool) = 0;
		virtual bool LoadLayoutFromString(const char* layoutXML, bool, bool) = 0;
		virtual void LoadLayoutAsync(const char* pathToXML, bool, bool) = 0;
		virtual void LoadLayoutFromStringAsync(const char* layoutXML, bool, bool) = 0;
		virtual bool CreateChildren(const char*) = 0;
		virtual void UnloadLayout(void) = 0;
		virtual bool IsLoaded(void) = 0;
		virtual void SetParent(IUIPanel* parent) = 0;
		virtual IUIPanel* GetParent(void) = 0;
		virtual IUIWindow* GetParentWindow(void) = 0;
		virtual void SetVisible(bool state) = 0;
		virtual bool IsVisible(void) = 0;
		virtual bool IsTransparent(void) = 0;
		virtual void SetLayoutLoadedFromParent(IUIPanel*) = 0;
		virtual void SetPanelIntoContext(IUIPanel*) = 0;
		virtual void* GetLayoutFile(void) = 0;
		virtual void GetLayoutFileLoadedFrom(void) = 0;
		virtual int GetLayoutFileReloadCount(void) = 0;
		virtual void unk6() = 0;
		virtual void unk7() = 0;
		virtual void unk8() = 0;
		virtual IUIPanel* FindChild(const char*) = 0;
		virtual void unk9() = 0;
		virtual IUIPanel* FindChildTraverse(const char*) = 0;
		virtual void unk10() = 0;
		virtual IUIPanel* FindChildInLayoutFile(const char*) = 0;
		virtual void unk11() = 0;
		virtual IUIPanel* FindPanelInLayoutFile(const char*) = 0;
		virtual bool IsDecendantOf(IUIPanel const* const) = 0;
		virtual void RemoveAndDeleteChildren(void) = 0;
		virtual void RemoveAndDeleteChildrenOfType(CPanoramaSymbol) = 0;
		virtual int GetChildCount(void) = 0;
		virtual IUIPanel* GetChild(int) = 0;
		virtual IUIPanel* GetFirstChild(void) = 0;
		virtual IUIPanel* GetLastChild(void) = 0;
		virtual int GetChildIndex(IUIPanel const* const) = 0;
		virtual int GetChildCountOfType(CPanoramaSymbol) = 0;
		virtual int GetHiddenChildCount(void) = 0;
		virtual IUIPanel* GetHiddenChild(int) = 0;
		virtual IUIPanel* FindAncestor(const char*) = 0;
		virtual void sub_131D10() = 0; // new function, added around august 2018
		virtual void SetRepaint(EPanelRepaint) = 0;
		virtual void SetRepaintUpParentChain(void* unk) = 0;
		virtual bool ShouldDrawChildren(IUIPanel*) = 0;
		virtual void EnableBackgroundMovies(bool state) = 0;
		virtual void* AccessIUIStyle() = 0;
		virtual IUIPanelStyle* AccessIUIStyleDirty() = 0;
		virtual void ApplyStyles(bool apply) = 0;
		virtual void AfterStylesApplied(bool, EStyleRepaint, bool, bool) = 0;
		virtual void SetOnStylesChangedNeeded(void) = 0;
		virtual void* AccessChildren(void) = 0;
		virtual void DesiredLayoutSizeTraverse(float, float) = 0;
		virtual void DesiredLayoutSizeTraverse(float*, float*, float, float, bool) = 0;
		virtual void OnContentSizeTraverse(float*, float*, float, float, bool) = 0;
		virtual void LayoutTraverse(float, float, float, float) = 0;
		virtual void OnLayoutTraverse(float, float) = 0;
		virtual void SetPositionFromLayoutTraverse(CUILength, CUILength, CUILength) = 0;
		virtual void InvalidateSizeAndPosition(void) = 0;
		virtual void InvalidatePosition(void) = 0;
		virtual void SetActiveSizeAndPositionTransition(void) = 0;
		virtual void SetActivePositionTransition(void) = 0;
		virtual bool IsSizeValid(void) = 0;
		virtual bool IsPositionValid(void) = 0;
		virtual bool IsChildSizeValid(void) = 0;
		virtual bool IsChildPositionValid(void) = 0;
		virtual bool IsSizeTransitioning(void) = 0;
		virtual bool IsPositionTransitioning(void) = 0;
		virtual bool IsChildPositionTransitioning(void) = 0;
		virtual bool IsChildSizeTransitioning(void) = 0;
		virtual void TransitionPositionApplied(bool) = 0;
		virtual void sub_140EE0();
		virtual void sub_140F00();
		virtual float GetDesiredLayoutWidth(void) = 0; // these might be doubles, need to "double"-check
		virtual float GetDesiredLayoutHeight(void) = 0;
		virtual float GetContentWidth(void) = 0;
		virtual float GetContentHeight(void) = 0;
		virtual float GetActualLayoutWidth(void) = 0;
		virtual float GetActualLayoutHeight(void) = 0;
		virtual float GetActualRenderWidth(void) = 0;
		virtual float GetActualRenderHeight(void) = 0;
		virtual float GetActualXOffset(void) = 0;
		virtual float GetActualYOffset(void) = 0;
		virtual float GetRawActualXOffset(void) = 0;
		virtual float GetRawActualYOffset(void) = 0;
		virtual void unk12() = 0;
		virtual void sub_140FC0() = 0;
		virtual float GetContentsYScrollOffset(void) = 0;
		virtual float GetContentsXScrollOffset(void) = 0;
		virtual void unk_131A10() = 0;
		virtual void unk_152250() = 0;
		virtual void unk_152230() = 0;
		virtual void unk_1522A0() = 0;
		virtual void unk_152270() = 0;
		virtual void unk_1522D0() = 0;
		virtual void unk_1522F0() = 0;
		virtual void unk_152310() = 0;
		virtual void unk_152330() = 0;
		virtual void unk_152350() = 0;
		virtual void unk_152380() = 0;
		virtual void unk_131080() = 0;
		virtual void unk_1310F0() = 0;
		virtual void unk_134E50() = 0;
		virtual void unk_1350C0() = 0;
		virtual void unk_134EF0() = 0;
		virtual void unk_135450() = 0;
		virtual void unk_135470() = 0;
		virtual void unk_131710() = 0;
		virtual void unk_131760() = 0;
		virtual bool CanScrollUp(void) = 0;
		virtual bool CanScrollDown(void) = 0;
		virtual bool CanScrollLeft(void) = 0;
		virtual bool CanScrollRight(void) = 0;
		virtual void AddClass(const char*) = 0;
		virtual void AddClass(CPanoramaSymbol) = 0;
		virtual void AddClasses(CPanoramaSymbol*, unsigned int count) = 0;
		virtual void RemoveClass(const char*) = 0;
		virtual void RemoveClasses(CPanoramaSymbol const* const, unsigned int) = 0;
		virtual void RemoveClasses(const char*) = 0;
		virtual void RemoveAllClasses(void) = 0;
		virtual void sub_137540() = 0;
		virtual void* GetClasses(void) = 0;
		virtual bool HasClass(const char*) = 0;
		virtual bool HasClass(CPanoramaSymbol) = 0;
		virtual bool AscendantHasClass(const char*) = 0;
		virtual bool AscendantHasClass(CPanoramaSymbol) = 0;
		virtual void ToggleClass(const char*) = 0;
		virtual void ToggleClass(CPanoramaSymbol) = 0;
		virtual void SetHasClass(const char*, bool) = 0;
		virtual void SetHasClass(CPanoramaSymbol, bool) = 0;
		virtual void SwitchClass(const char*, const char*) = 0;
		virtual void SwitchClass(const char*, CPanoramaSymbol) = 0;
		virtual void unk_149DF0() = 0;
		virtual void unk_149D50() = 0;
		virtual void unk_149EB0() = 0;
		virtual void unk_149E50() = 0;
		virtual bool AcceptsInput() = 0;
		virtual void SetAcceptsInput(bool state) = 0;
		virtual bool AcceptsFocus() = 0;
		virtual void SetAcceptsFocus(bool state) = 0;
		virtual bool CanAcceptInput() = 0;
		virtual void SetDefaultFocus(const char*) = 0;
		virtual const char* GetDefaultFocus() = 0;
		virtual void SetDisableFocusOnMouseDown(bool state) = 0;
		virtual bool FocusOnMouseDown() = 0;
		virtual void unk_152460() = 0;
		virtual void unk_1523E0() = 0;
		virtual void unk_130E80() = 0;
		virtual void unk_130EB0() = 0;
		virtual void unk_152400() = 0;
		virtual void unk_152420() = 0;
		virtual void unk_152470() = 0;
		virtual void unk_152490() = 0;
		virtual void unk_1318C0() = 0;
		virtual void GetDefaultInputFocus(void) = 0;
		virtual void SetFocus(void) = 0;
		virtual void UpdateFocusInContext(void) = 0;
		virtual void SetFocusDueToHover(void) = 0;
		virtual void SetInputContextFocus(void) = 0;
		virtual unsigned long long GetStyleFlags(void) = 0;
		virtual void AddStyleFlag(EStyleFlags) = 0;
		virtual void RemoveStyleFlag(EStyleFlags) = 0;
		virtual bool IsInspected(void) = 0;
		virtual bool HasHoverStyle(bool) = 0;
		virtual void SetSelected(bool state) = 0;
		virtual bool IsSelected(void) = 0;
		virtual bool HasKeyFocus(void) = 0;
		virtual bool HasDescendantKeyFocus(void) = 0;
		virtual bool IsLayoutLoading(void) = 0;
		virtual void SetEnabled(bool state) = 0;
		virtual bool IsEnabled(void) = 0;
		virtual void unk_146920() = 0;
		virtual void unk_1525B0() = 0;
		virtual void unk13() = 0;
		virtual void sub_125010() = 0;
		virtual void SetAllChildrenActivationEnabled(bool state) = 0;
		virtual void SetHitTestEnabled(bool state) = 0;
		virtual bool HitTestEnabled(void) = 0;
		virtual void SetHitTestEnabledTraverse(bool state) = 0;
		virtual void unk_152630() = 0;
		virtual void unk_152660() = 0;
		virtual void SetDraggable(bool state) = 0;
		virtual bool IsDraggable(void) = 0;
		virtual void SetRememberChildFocus(bool state) = 0;
		virtual bool GetRememberChildFocus(void) = 0;
		virtual void ClearLastChildFocus(void) = 0;
		virtual void SetNeedsIntermediateTexture(bool state) = 0;
		virtual bool GetNeedsIntermediateTexture(void) = 0;
		virtual void unk_152DE0() = 0;
		virtual void unk_152E40() = 0;
		virtual void SetClipAfterTransform(bool state) = 0;
		virtual bool GetClipAfterTransform(void) = 0;
		virtual const char* GetInputNamespace(void) = 0;
		virtual void SetInputNamespace(const char*) = 0;
		virtual void MarkStylesDirty(bool state) = 0;
		virtual bool StylesDirty(void) = 0;
		virtual void sub_12A000() = 0;
		virtual bool ChildStylesDirty() = 0;
		virtual bool ParsePanelEvent(CPanoramaSymbol, const char*) = 0;
		virtual bool IsPanelEventSet(CPanoramaSymbol) = 0;
		virtual bool IsPanelEvent(CPanoramaSymbol) = 0;
		virtual void DispatchPanelEvent(CPanoramaSymbol) = 0;
		virtual void* GetJavaScriptContextParent(void) = 0;
		virtual void* UIImageManager(void) = 0;
		virtual void* UIRenderEngine(void) = 0;
		virtual void* unk_130DF0() = 0; // returns another class like the 2 above
		virtual void PaintTraverse(void) = 0;
		virtual void SetTabIndex(float) = 0;
		virtual float GetTabIndex(void) = 0;
		virtual void SetSelectionPosition(float, float) = 0;
		virtual void SetSelectionPositionX(float) = 0;
		virtual void SetSelectionPositionY(float) = 0;
		virtual float GetSelectionPositionX(void) = 0;
		virtual float GetSelectionPositionY(void) = 0;
		virtual float GetTabIndex_Raw(void) = 0;
		virtual float GetSelectionPositionX_Raw(void) = 0;
		virtual float GetSelectionPositionY_Raw(void) = 0;
		virtual void SetFocusToNextPanel(int, EFocusMoveDirection, bool, float, float, float, float, float) = 0;
		virtual void SetInputFocusToFirstOrLastChildInFocusOrder(EFocusMoveDirection, float, float) = 0;
		virtual void SelectionPosVerticalBoundary(void) = 0;
		virtual void SelectionPosHorizontalBoundary(void) = 0;
		virtual void SetChildFocusOnHover(bool state) = 0;
		virtual bool GetChildFocusOnHover(void) = 0;
		virtual void SetFocusOnHover(bool state) = 0;
		virtual bool GetFocusOnHover(void) = 0;
		virtual void ScrollToTop(void) = 0;
		virtual void ScrollToBottom(void) = 0;
		virtual void ScrollToLeftEdge(void) = 0;
		virtual void ScrollToRightEdge(void) = 0;
		virtual void ScrollParentToMakePanelFit(ScrollBehavior_t, bool) = 0;
		virtual void ScrollToFitRegion(float, float, float, float, ScrollBehavior_t, bool, bool) = 0;
		virtual bool CanSeeInParentScroll(void) = 0;
		virtual void OnScrollPositionChanged(void) = 0;
		virtual void SetSendChildScrolledIntoViewEvents(bool) = 0;
		virtual void OnCheckChildrenScrolledIntoView(void) = 0;
		virtual void FireScrolledIntoViewEvent(void) = 0;
		virtual void FireScrolledOutOfViewEvent(void) = 0;
		virtual bool IsScrolledIntoView(void) = 0;
		virtual void SortChildren() = 0; // (int (*)(panorama::IUIPanelClient * const*,panorama::IUIPanelClient * const*))
		virtual void AddChild(IUIPanel*) = 0;
		virtual void AddChildSorted() = 0; // (bool (*)(panorama::IUIPanelClient * const&,panorama::IUIPanelClient * const&),panorama::IUIPanel *)
		virtual void ResortChild() = 0; // (bool (*)(panorama::IUIPanelClient * const&,panorama::IUIPanelClient * const&),panorama::IUIPanel *)
		virtual void RemoveChild(IUIPanel*) = 0;
		virtual void unk_139310() = 0;
		virtual void unk_1393C0() = 0;
		virtual void unk_131260() = 0;
		virtual void unk_131270() = 0;
		virtual void SetMouseCanActivate(EMouseCanActivate, const char*) = 0;
		virtual EMouseCanActivate GetMouseCanActivate(void) = 0;
		virtual IUIPanel* FindParentForMouseCanActivate(void) = 0;
		virtual bool ReloadLayout(CPanoramaSymbol) = 0;
		virtual void ReloadStyleFileTraverse(CPanoramaSymbol) = 0;
		virtual bool HasOnActivateEvent(void) = 0;
		virtual bool HasOnMouseActivateEvent(void) = 0;
		virtual void SetupJavascriptObjectTemplate(void) = 0;
		virtual void SetLayoutFile(CPanoramaSymbol) = 0;
		virtual void BuildMatchingStyleList() = 0; // (CUtlVector<panorama::CascadeStyleFileInfo_t,CUtlMemory<panorama::CascadeStyleFileInfo_t,int>> *)
		virtual void GetAttribute(const char*, int) = 0;
		virtual void GetAttribute(const char*, const char*) = 0;
		virtual void unk_132680() = 0;
		virtual void GetAttribute(const char*, unsigned int) = 0;
		virtual void GetAttribute(const char*, unsigned long long) = 0;
		virtual void SetAttribute(const char*, int) = 0;
		virtual void SetAttribute(const char*, const char*) = 0;
		virtual void SetAttribute(const char*, unsigned int) = 0;
		virtual void SetAttribute(const char*, unsigned long long) = 0;
		virtual void SetAttribute() = 0;
		virtual void unk_132770() = 0;
		virtual void unk_132830() = 0;
		virtual void unk_132810() = 0;
		virtual void unk_1327C0() = 0;
		virtual void unk_132880() = 0;
		virtual void unk_13D2D0() = 0;
		virtual void unk_13D110() = 0;
		virtual void unk_13D350() = 0;
		virtual void unk_13D3D0() = 0;
		virtual void unk_13D450() = 0;
		virtual void unk_13D950() = 0;
		virtual void unk_13D870() = 0;
		virtual void unk_13D690() = 0;
		virtual void unk_13D420() = 0;
		virtual void SetAnimation(const char*, float, float, EAnimationTimingFunction, EAnimationDirection, float) = 0;
		virtual void UpdateVisibility(bool) = 0;
		virtual void unk14() = 0;
		virtual void SetProperty(CPanoramaSymbol, const char*) = 0;
		virtual void FindChildrenWithClass() = 0; // (char const*,CUtlVector<panorama::IUIPanel *,CUtlMemory<panorama::IUIPanel *,int>> &)
		virtual void FindChildrenWithClassTraverse() = 0; // (char const*,CUtlVector<panorama::IUIPanel *,CUtlMemory<panorama::IUIPanel *,int>> &)
		virtual void PlayFocusChangeSound(int, float) = 0;
		virtual void unk15() = 0;
		virtual void ClearPanelEvents(CPanoramaSymbol) = 0;
		virtual void SetPanelEvent(CPanoramaSymbol, IUIEvent*) = 0;
		virtual void SetPanelEvent2() = 0; // (panorama::CPanoramaSymbol,CUtlVector<panorama::IUIEvent *,CUtlMemory<panorama::IUIEvent *,int>> *)
		virtual bool GetAnalogStickScrollingEnabled(void) = 0;
		virtual void EnableAnalogStickScrolling(bool state) = 0;
		virtual void SetMouseTracking(bool state) = 0;
		virtual void SetInScrollbarConstruction(bool state) = 0;
		virtual void* GetVerticalScrollBar(void) = 0;
		virtual void* GetHorizontalScrollBar(void) = 0;
		virtual void GetPanelEvents(CPanoramaSymbol) = 0;
		virtual bool HasBeenLayedOut(void) = 0;
		virtual void OnStyleTransitionsCleanup(void) = 0;
		virtual void unk_1529D0() = 0;
		virtual void unk_1529F0() = 0;
		virtual void unk_152A00() = 0;
		virtual void unk_152A20() = 0;
		virtual void unk_136B20() = 0;
		virtual void unk_152A30() = 0;
		virtual void unk_139260() = 0;
		virtual void unk_1331A0() = 0;
		virtual void unk_1529A0() = 0;
		virtual void unk_1529B0() = 0;
		virtual void unk_1529C0() = 0;
	};

	inline PanelArray* panelArray;

	class IUIEvent
	{
	public:
		/*
		virtual void DESTROY() = 0;
		virtual void DESTROY2() = 0;
		virtual IUIPanel* GetTargetPanel(void);
		virtual void SetTargetPanel(IUIPanel const *);
		virtual CPanoramaSymbol* GetEventType(void);
		virtual bool CanBubble(void);
		virtual void Dispatch(CUtlAbstractDelegate);
		virtual void Copy(void);
		virtual void GetJavaScriptArgs(int *,v8::Handle<v8::Value> value);
		*/
		panorama::IUIPanel* pos1; //0x0000
		char pad_0008[8]; //0x0008
		panorama::IUIPanel* pos2; //0x0010
		panorama::IUIPanel* pos3; //0x0018
		char pad_0020[24]; //0x0020
		panorama::IUIPanel* pos4; //0x0038
		char pad_0040[8]; //0x0040
		panorama::IUIPanel* pos5; //0x0048
		panorama::IUIPanel* pos6; //0x0050
		char pad_0058[8]; //0x0058
		panorama::IUIPanel* pos7; //0x0060
		panorama::IUIPanel* pos8; //0x0068
		char pad_0070[16]; //0x0070
		panorama::IUIPanel* pos9; //0x0080
		panorama::IUIPanel* pos10; //0x0088
		panorama::IUIPanel* pos11; //0x0090
		char pad_0098[24]; //0x0098
		panorama::IUIPanel* pos12; //0x00B0
		panorama::IUIPanel* pos13; //0x00B8
		char pad_00C0[8]; //0x00C0
		panorama::IUIPanel* pos14; //0x00C8
		char pad_00D0[16]; //0x00D0
		panorama::IUIPanel* pos15; //0x00E0
		char pad_00E8[8]; //0x00E8
		char* eventName; //0x00F0
		char pad_00F8[16]; //0x00F8
		panorama::IUIPanel* pos16; //0x0108
		char pad_0110[8]; //0x0110
		panorama::IUIPanel* pos17; //0x0118
		char pad_0120[40]; //0x0120
		panorama::IUIPanel* pos18; //0x0148
		char pad_0150[8]; //0x0150
		panorama::IUIPanel* pos19; //0x0158
		char pad_0160[8]; //0x0160
		panorama::IUIPanel* pos20; //0x0168
		char pad_0170[32]; //0x0170
		panorama::IUIPanel* pos21; //0x0190
		char pad_0198[360]; //0x0198
	};

	class UIEngine
	{
	public:
		virtual void DESTROY() = 0; // 0
#ifndef WIN32    
		virtual void DESTROY2() = 0;
#endif
		virtual void StartupSubsystems(panorama::IUISettings*, void* PlatWindow_t__) = 0;
		virtual void ConCommandInit(void* IConCommandBaseAccessor) = 0;
		virtual void Shutdown(void) = 0;
		virtual void RequestShutdown(void) = 0;
		virtual void Run(void) = 0;
		virtual void RunFrame(void) = 0;
		virtual void SetAggressiveFrameRateLimit(bool) = 0;
		virtual bool IsRunning(void) = 0;
		virtual bool HasFocus(void) = 0;
		virtual double GetCurrentFrameTime(void) = 0;
		virtual void unk0() = 0; // getter
		virtual void unk1() = 0; // setter ^^
		virtual void INT3_WRAPPER() = 0;
		virtual void INT3_WRAPPER2() = 0;
		virtual void INT3_WRAPPER3() = 0; // probably windows specific
		virtual void INT3_WRAPPER4() = 0;
		virtual void INT3_WRAPPER5() = 0;
		virtual void CreateTextLayout(char const*, char const*, float, float, panorama::EFontWeight, panorama::EFontStyle, panorama::ETextAlign, bool, bool, int, float, float) = 0;
		virtual void CreateTextLayout(wchar_t const*, char const*, float, float, panorama::EFontWeight, panorama::EFontStyle, panorama::ETextAlign, bool, bool, int, float, float) = 0;
		virtual void FreeTextLayout(panorama::IUITextLayout*) = 0;
		virtual void GetSomeFontThing(void) = 0; //similar to function below
		virtual void GetSortedValidFontNames(void) = 0;
		virtual IUIInputEngine* UIInputEngine(void) = 0;
		virtual IUILocalize* UILocalize(void) = 0;
		virtual IUISoundSystem* UISoundSystem(void) = 0;
		virtual IUISettings* UISettings(void) = 0;
		virtual IUILayoutManager* UILayoutManager(void) = 0;
		virtual IUIFileSystem* UIFileSystem(void) = 0;
#ifndef WIN32
		virtual void INT3_WRAPPER6() = 0;
#endif
		virtual void RegisterFrameFunc(void (*)(void)) = 0;
		virtual void ReloadLayoutFile(panorama::CPanoramaSymbol) = 0;
		virtual void ToggleDebugMode(void) = 0;
		virtual const char* GetConsoleHistory(void) = 0;
		virtual PanelWrapper* CreatePanel(void) = 0;
		virtual void PanelDestroyed(panorama::IUIPanel*, panorama::IUIPanel*) = 0;
		virtual bool IsValidPanelPointer(panorama::IUIPanel const*) = 0;
		virtual PanelHandle_t* GetPanelHandle(panorama::IUIPanel const*) = 0;
		virtual IUIPanel* GetPanelPtr(panorama::PanelHandle_t const&) = 0;
		virtual void CallBeforeStyleAndLayout(panorama::IUIPanel*) = 0;
		virtual void RegisterForPanelDestroyed(); //CUtlDelegate<void ()(panorama::IUIPanel const*,panorama::IUIPanel*)>) = 0;
		virtual void UnregisterForPanelDestroyed(); //CUtlDelegate<void ()(panorama::IUIPanel const*,panorama::IUIPanel*)>) = 0;
		virtual void RegisterEventHandler(panorama::CPanoramaSymbol, panorama::IUIPanel*, CUtlAbstractDelegate) = 0;
		virtual void UnregisterEventHandler(panorama::CPanoramaSymbol, panorama::IUIPanel*, CUtlAbstractDelegate) = 0;
		virtual void RegisterEventHandler(panorama::CPanoramaSymbol, panorama::IUIPanelClient*, CUtlAbstractDelegate) = 0;
		virtual void UnregisterEventHandler(panorama::CPanoramaSymbol, panorama::IUIPanelClient*, CUtlAbstractDelegate) = 0;
		virtual void UnregisterEventHandlersForPanel(panorama::IUIPanel*) = 0;
		virtual void RegisterForUnhandledEvent(panorama::CPanoramaSymbol, CUtlAbstractDelegate) = 0;
		virtual void UnregisterForUnhandledEvent(panorama::CPanoramaSymbol, CUtlAbstractDelegate) = 0;
		virtual void UnregisterForUnhandledEvents(void*) = 0;
		virtual bool HaveEventHandlersRegisteredForType(panorama::CPanoramaSymbol) = 0;
		virtual void RegisterPanelTypeEventHandler(panorama::CPanoramaSymbol, panorama::CPanoramaSymbol, CUtlAbstractDelegate, bool) = 0;
		virtual void DispatchEvent(panorama::IUIEvent*) = 0;
		virtual void DispatchEventAsync(float, panorama::IUIEvent*) = 0;
		virtual bool AnyHandlerRegisteredForEvent(panorama::CPanoramaSymbol const&) = 0;
		virtual CPanoramaSymbol* GetLastDispatchedEventSymbol(void) = 0;
		virtual IUIPanel* GetLastDispatchedEventTargetPanel(void) = 0; // This function can fail sometimes and you need to check the result/call it later (YUCK!)
		virtual void RegisterEventFilter(CUtlAbstractDelegate) = 0;
		virtual void UnregisterEventFilter(CUtlAbstractDelegate) = 0;
		virtual void LayoutAndPaintWindows(void) = 0;
		virtual const char* GetApplicationInstallPath(void) = 0;
		virtual const char* GetApplicationUserDataPath(void) = 0;
		virtual void RegisterNamedLocalPath(char const*, char const*, bool, bool) = 0;
		virtual void RegisterNamedUserPath(char const*, char const*, bool, bool) = 0;
		virtual void RegisterCustomFontPath(char const*) = 0;
		virtual void GetLocalPathForNamedPath(char const*) = 0;
		virtual void GetLocalPathForRelativePath(char const*, char const*, void* CUtlString) = 0;
		virtual void RegisterNamedRemoteHost(char const*, char const*) = 0;
		virtual void GetRemoteHostForNamedHost(char const*) = 0;
		virtual void RegisterXHeader(char const*, char const*) = 0;
		virtual int GetXHeaderCount(void) = 0;
		virtual void GetXHeader(int, void* CUtlString, void* CUtlString2) = 0;
		virtual void SetCookieHeaderForNamedRemoteHost(); //char const*,CUtlVector<CUtlString,CUtlMemory<CUtlString,int>> const&) = 0;
		virtual void SetCookieHeaderForRemoteHost(); //char const*,CUtlVector<CUtlString,CUtlMemory<CUtlString,int>> const&) = 0;
		virtual void GetCookieHeadersForNamedRemoteHost(char const*) = 0;
		virtual void GetCookieHeadersForRemoteHost(char const*) = 0;
		virtual void GetCookieValueForRemoteHost(char const*, char const*, void* CUtlString) = 0;
		virtual void BSetCookieForWebRequests(char const*, char const*, char const*) = 0;
		virtual void BClearCookieForWebRequests(char const*, char const*, char const*) = 0;
		virtual void* AccessHTMLController(void) = 0;
		virtual void unk() = 0; // _SDL_ShowSimpleMessageBox with yes/no
		virtual void AddFrameListener(panorama::IUIEngineFrameListener*) = 0;
		virtual void RemoveFrameListener(panorama::IUIEngineFrameListener*) = 0;
		virtual void RegisterMouseCanActivateParent(panorama::IUIPanel*, char const*) = 0;
		virtual void UnregisterMouseCanActivateParent(panorama::IUIPanel*) = 0;
		virtual void GetMouseCanActivateParent(panorama::IUIPanel*) = 0;
		virtual bool AnyWindowHasFocus(void) = 0;
		virtual bool AnyVisibleWindowHasFocus(void) = 0;
		virtual bool AnyOverlayWindowHasFocus(void) = 0;
		virtual IUIWindow* GetFocusedWindow(void) = 0;
		virtual float GetLastInputTime(void) = 0;
		virtual float UpdateLastInputTime(void) = 0;
		virtual void ClearClipboard(void) = 0;
		virtual void CopyToClipboard(char const*, char const*) = 0;
		virtual void GetClipboardText(void* CUtlString, void* CUtlString2) = 0;
		virtual const char* GetDisplayLanguage(void) = 0;
		virtual void unk2() = 0;
		virtual void unk3() = 0;
		virtual void unk4() = 0;
		virtual bool HasOverlayForApp(unsigned long long, unsigned long long) = 0;
		virtual void TrackOverlayForApp(unsigned long long, unsigned long long, void*) = 0;
		virtual void DeleteOverlayInstanceForApp(unsigned long long, unsigned long long, void*) = 0;
		virtual void OverlayForApp(unsigned long long, unsigned long long) = 0;
#ifndef WIN32
		virtual void unk5() = 0;
#endif
		virtual IUIPanelStyle* AllocPanelStyle(panorama::IUIPanel*, float) = 0;
		virtual void FreePanelStyle(panorama::IUIPanelStyle*) = 0;
		virtual void SetPanelWaitingAsyncDelete(panorama::IUIPanel*) = 0;
		virtual bool IsPanelWaitingAsyncDelete(panorama::IUIPanel*) = 0;
		virtual void PulseActiveControllerHaptic(); //panorama::IUIEngine::EHapticFeedbackPosition,panorama::IUIEngine::EHapticFeedbackStrength) = 0;
		virtual void SomethingControllerHapticRelated() = 0;
		virtual void MarkLayerToRepaintThreadSafe(unsigned long long) = 0;
		virtual void AddDirectoryChangeWatch(char const*) = 0;
		virtual int GetWheelScrollLines(void) = 0; // default 3
		virtual int RunScript(panorama::IUIPanel* panel, char const* entireJSCode, char const* pathToXMLContext, int, int, bool alreadyCompiled
#ifdef WIN32
			,
			bool
#endif
		)
			= 0;

#ifdef WIN32
		virtual void unk5() = 0;
#endif

		virtual void ExposeObjectTypeToJavaScript(char const*, CUtlAbstractDelegate&) = 0;
		virtual bool IsObjectTypeExposedToJavaScript(char const*) = 0;
		virtual void ExposeGlobalObjectToJavaScript(char const*, void*, char const*, bool) = 0;
		virtual void ClearGlobalObjectForJavaScript(char const*, void*) = 0;
		virtual void DeleteJSObjectInstance(panorama::IUIJSObject*) = 0;
		virtual IUIPanel* GetPanelForJavaScriptContext(); //v8::Context *) = 0;
		virtual void* GetJavaScriptContextForPanel(panorama::IUIPanel*) = 0;
		virtual void OutputJSExceptionToConsole(); //v8::TryCatch &,panorama::IUIPanel *) = 0;
		virtual void AddGlobalV8FunctionTemplate(); //char const*,v8::Handle<v8::FunctionTemplate> *,bool) = 0;
		virtual void* GetV8GlobalContext(void) = 0;
		virtual void GetCurrentV8ObjectTemplateToSetup(void) = 0; //gets offset.
		virtual void GetCurrentV8ObjectTemplateToSetup2(void) = 0; //gets offset and calls some function.
		virtual void Grabber3(void) = 0; // returns offset in class
		virtual void Grabber4(void) = 0; // returns offset in class.
		virtual void UIStyleFactory(void) = 0;
		virtual void GetV8Isolate(void) = 0;
		virtual void RunFunction() = 0;
		virtual void RunFunction2() = 0; //different args
		virtual void CreateV8PanelInstance(panorama::IUIPanel*) = 0;
		virtual void CreateV8PanelStyleInstance(panorama::IUIPanelStyle*) = 0;
		virtual void CreateV8ObjectInstance(char const*, void*, panorama::IUIJSObject*) = 0;
		virtual void _unk() = 0;
		virtual void InitiateAsyncJSONWebAPIRequest(); //EHTTPMethod,CUtlString,panorama::IUIPanel *,void *,panorama::CJSONWebAPIParams *,uint) = 0;
		virtual void InitiateAsyncJSONWebAPIRequest(int overloadMeme); //EHTTPMethod,CUtlString,CUtlDelegate<void ()(unsigned long long,KeyValues *,void *)>,void *,panorama::CJSONWebAPIParams *,uint) = 0;
		virtual void CancelAsyncJSONWebAPIRequest(unsigned int) = 0;
		virtual void ResolvePath(char const*) = 0;
		virtual void RegisterEventWithEngine(panorama::CPanoramaSymbol, panorama::UIEventFactory) = 0;
		virtual bool IsValidEventName(panorama::CPanoramaSymbol) = 0;
		virtual bool IsValidPanelEvent(panorama::CPanoramaSymbol, int*) = 0;
		virtual void CreateInputEventFromSymbol(panorama::CPanoramaSymbol, panorama::IUIPanel*, panorama::EPanelEventSource_t, int) = 0;
		virtual void CreateEventFromString(panorama::IUIPanel*, char const*, char const**) = 0;
		virtual void unk11();
		virtual void RegisterPanelFactoryWithEngine(panorama::CPanoramaSymbol, panorama::CPanel2DFactory*) = 0;
		virtual bool RegisteredPanelType(panorama::CPanoramaSymbol) = 0;
		virtual PanelWrapper* CreatePanel(panorama::CPanoramaSymbol* panelType, char const* id, panorama::IUIPanel* createInThisParent) = 0; // paneltype (ex: Panel, Label), id (ex: <Label id="swag">)
		virtual void CreateDebuggerWindow(void) = 0;
		virtual void CloseDebuggerWindow(void) = 0;
		virtual void RegisterScheduledDelegate(); // double,CUtlDelegate<void ()(void)> ) = 0;
		virtual void CancelScheduledDelegate(int) = 0;
		virtual float GetLastScheduledDelegateRunTime(void) = 0;
		virtual CPanoramaSymbol MakeSymbol(char const*) = 0; // if the symbol already exists, you'll get that one.
		virtual const char* ResolveSymbol(unsigned short) = 0; // if you pass an invalid number into here, it can crash the program.
		virtual void QueueDecrementRefNextFrame(); //CRefCounted<CRefCountServiceBase<true,CRefMT>> *) = 0;
		virtual void RegisterJSGenericCallback(); //panorama::IUIPanel *,v8::Handle<v8::Function>) = 0;
		virtual void InvokeJSGenericCallback(); //int,int,v8::Handle<v8::Value> *,v8::Handle<v8::Value> *) = 0;
		virtual void UnregisterJSGenericCallback(int) = 0;
		virtual int GetNumRegisterJSScopes(void) = 0;
		virtual void GetRegisterJSScopeInfo(int, panorama::RegisterJSScopeInfo_t*) = 0;
		virtual void GetRegisterJSEntryInfo(int, int, panorama::RegisterJSEntryInfo_t*) = 0;
		virtual void StartRegisterJSScope(char const*, char const*) = 0;
		virtual void EndRegisterJSScope(void) = 0;
		virtual void NewRegisterJSEntry(char const*, unsigned int, char const*, panorama::RegisterJSType_t) = 0;
		virtual void SetRegisterJSEntryParams(int, unsigned char, panorama::RegisterJSType_t*) = 0;
		virtual void _padding() = 0;
		virtual void ClearFileCache(void) = 0;
		virtual void PrintCacheStatus(void) = 0;
		virtual void GetWindowsForDebugger(); //CUtlVector<panorama::IUIWindow *,CUtlMemory<panorama::IUIWindow *,int>> &) = 0;
		virtual void SetPaintCountTrackingEnabled(bool) = 0;
		virtual bool GetPaintCountTrackingEnabled(void) = 0;
		virtual void IncrementPaintCountForPanel(unsigned long long, bool, double) = 0;
		virtual void GetPanelPaintInfo(unsigned long long, unsigned int&, bool&, double&) = 0;
		virtual bool HasAnyWindows(void) = 0;
		virtual void TextEntryFocusChange(panorama::IUIPanel*) = 0;
		virtual void TextEntryInvalid(panorama::IUIPanel*) = 0;
		virtual void SetClipboardText(const char* text) = 0; // wrapper for _SDL_SetClipboardText
		virtual const char* GetClipboardText() = 0; // wrapper for _SDL_GetClipboardText
		virtual void OnFileCacheRemoved(panorama::CPanoramaSymbol) = 0;
		virtual void RunPlatformFrame(void) = 0;
		virtual void CreateSoundSystem(void) = 0;
	};
}

class IPanoramaUIEngine : IAppSystem {
public:
	virtual void SetupUIEngine(void) = 0;
	virtual void ShutdownUIEngine(void) = 0;
	virtual panorama::UIEngine* AccessUIEngine(void) = 0;
	virtual void HandleInputEvent() = 0;
	virtual void unk1() = 0;
	virtual void unk2() = 0;
};

namespace vgui {
	class SurfacePlat;
	class IClientPanel;

	class Panel;

	typedef uintptr_t VPANEL;

	class IPanel : public IBaseInterface {
	public:
		virtual void Init(VPANEL vguiPanel, IClientPanel* panel) = 0;
		virtual void SetPos(VPANEL vguiPanel, int x, int y) = 0;
		virtual void GetPos(VPANEL vguiPanel, int& x, int& y) = 0;
		virtual void SetSize(VPANEL vguiPanel, int wide, int tall) = 0;
		virtual void GetSize(VPANEL vguiPanel, int& wide, int& tall) = 0;
		virtual void SetMinimumSize(VPANEL vguiPanel, int wide, int tall) = 0;
		virtual void GetMinimumSize(VPANEL vguiPanel, int& wide, int& tall) = 0;
		virtual void SetZPos(VPANEL vguiPanel, int z) = 0;
		virtual int  GetZPos(VPANEL vguiPanel) = 0;
		virtual void GetAbsPos(VPANEL vguiPanel, int& x, int& y) = 0;
		virtual void GetClipRect(VPANEL vguiPanel, int& x0, int& y0, int& x1, int& y1) = 0;
		virtual void SetInset(VPANEL vguiPanel, int left, int top, int right, int bottom) = 0;
		virtual void GetInset(VPANEL vguiPanel, int& left, int& top, int& right, int& bottom) = 0;
		virtual void SetVisible(VPANEL vguiPanel, bool state) = 0;
		virtual bool IsVisible(VPANEL vguiPanel) = 0;
		virtual void SetParent(VPANEL vguiPanel, VPANEL newParent) = 0;
		virtual int GetChildCount(VPANEL vguiPanel) = 0;
		virtual VPANEL GetChild(VPANEL vguiPanel, int index) = 0;
		virtual void*/*CUtlVector< VPANEL >&*/ GetChildren(VPANEL vguiPanel) = 0;
		virtual VPANEL GetParent(VPANEL vguiPanel) = 0;
		virtual void MoveToFront(VPANEL vguiPanel) = 0;
		virtual void MoveToBack(VPANEL vguiPanel) = 0;
		virtual bool HasParent(VPANEL vguiPanel, VPANEL potentialParent) = 0;
		virtual bool IsPopup(VPANEL vguiPanel) = 0;
		virtual void SetPopup(VPANEL vguiPanel, bool state) = 0;
		virtual bool IsFullyVisible(VPANEL vguiPanel) = 0;
		virtual void* GetScheme(VPANEL vguiPanel) = 0;
		virtual bool IsProportional(VPANEL vguiPanel) = 0;
		virtual bool IsAutoDeleteSet(VPANEL vguiPanel) = 0;
		virtual void DeletePanel(VPANEL vguiPanel) = 0;
		virtual void SetKeyBoardInputEnabled(VPANEL vguiPanel, bool state) = 0;
		virtual void SetMouseInputEnabled(VPANEL vguiPanel, bool state) = 0;
		virtual bool IsKeyBoardInputEnabled(VPANEL vguiPanel) = 0;
		virtual bool IsMouseInputEnabled(VPANEL vguiPanel) = 0;
		virtual void Solve(VPANEL vguiPanel) = 0;
		virtual const char* GetName(VPANEL vguiPanel) = 0;
		virtual const char* GetClassName(VPANEL vguiPanel) = 0;
		virtual void SendMessage(VPANEL vguiPanel, /*KeyValues*/void* params, VPANEL ifromPanel) = 0;
		virtual void Think(VPANEL vguiPanel) = 0;
		virtual void PerformApplySchemeSettings(VPANEL vguiPanel) = 0;
		virtual void PaintTraverse(VPANEL vguiPanel, bool forceRepaint, bool allowForce = true) = 0;
		virtual void Repaint(VPANEL vguiPanel) = 0;
		virtual VPANEL IsWithinTraverse(VPANEL vguiPanel, int x, int y, bool traversePopups) = 0;
		virtual void OnChildAdded(VPANEL vguiPanel, VPANEL child) = 0;
		virtual void OnSizeChanged(VPANEL vguiPanel, int newWide, int newTall) = 0;
		virtual void InternalFocusChanged(VPANEL vguiPanel, bool lost) = 0;
		virtual bool RequestInfo(VPANEL vguiPanel, /*KeyValues*/void* outputData) = 0;
		virtual void RequestFocus(VPANEL vguiPanel, int direction = 0) = 0;
		virtual bool RequestFocusPrev(VPANEL vguiPanel, VPANEL existingPanel) = 0;
		virtual bool RequestFocusNext(VPANEL vguiPanel, VPANEL existingPanel) = 0;
		virtual VPANEL GetCurrentKeyFocus(VPANEL vguiPanel) = 0;
		virtual int GetTabPosition(VPANEL vguiPanel) = 0;
		virtual SurfacePlat* Plat(VPANEL vguiPanel) = 0;
		virtual void SetPlat(VPANEL vguiPanel, SurfacePlat* Plat) = 0;
		virtual Panel* GetPanel(VPANEL vguiPanel, const char* destinationModule) = 0;
		virtual bool IsEnabled(VPANEL vguiPanel) = 0;
		virtual void SetEnabled(VPANEL vguiPanel, bool state) = 0;
		virtual bool IsTopmostPopup(VPANEL vguiPanel) = 0;
		virtual void SetTopmostPopup(VPANEL vguiPanel, bool state) = 0;
		virtual void SetMessageContextId(VPANEL vguiPanel, int nContextId) = 0;
		virtual int GetMessageContextId(VPANEL vguiPanel) = 0;
	};
}

class ClientClass {
public:
	const char* GetName() {
		return m_pNetworkName;
	}

	CreateClientClassFn m_pCreateFn;
	CreateEventFn m_pCreateEventFn;
	char* m_pNetworkName;
	RecvTable* m_pRecvTable;
	ClientClass* m_pNext;
	int m_ClassID;
	const char* m_pMapClassname;
};

class IHandleEntity {
public:
	virtual ~IHandleEntity() {}
	virtual void SetRefEHandle(const CBaseHandle& handle) = 0;
	virtual const CBaseHandle& GetRefEHandle() const = 0;
};

class IPVSNotify {
public:
	virtual void OnPVSStatusChanged(bool bInPVS) = 0;
};

struct RenderableInstance_t {
	uint8_t m_nAlpha;
};

class IClientRenderable {
public:
	virtual class IClientUnknown* GetIClientUnknown() = 0;
	virtual Vector const& GetRenderOrigin(void) = 0;
	virtual QAngle const& GetRenderAngles(void) = 0;
	virtual bool ShouldDraw(void) = 0;
	virtual int GetRenderFlags(void) = 0; // ERENDERFLAGS_xxx
	virtual void Unused(void) const {}
	virtual ClientShadowHandle_t GetShadowHandle() const = 0;
	virtual void* RenderHandle() = 0;
	virtual const model_t* GetModel() const = 0;
	virtual int DrawModel(int flags, const RenderableInstance_t& instance) = 0;
	virtual int GetBody() = 0;
	virtual void GetColorModulation(float* color) = 0;
	virtual bool LODTest() = 0;
	virtual bool SetupBones(matrix3x4_t* pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime) = 0;
	virtual void SetupWeights(const matrix3x4_t* pBoneToWorld, int nFlexWeightCount, float* pFlexWeights, float* pFlexDelayedWeights) = 0;
	virtual void DoAnimationEvents(void) = 0;
	virtual IPVSNotify* GetPVSNotifyInterface() = 0;
	virtual void GetRenderBounds(Vector& mins, Vector& maxs) = 0;
	virtual void GetRenderBoundsWorldspace(Vector& mins, Vector& maxs) = 0;
	virtual void GetShadowRenderBounds(Vector& mins, Vector& maxs, ShadowType_t shadowType) = 0;
	virtual bool ShouldReceiveProjectedTextures(int flags) = 0;
	virtual bool GetShadowCastDistance(float* pDist, ShadowType_t shadowType) const = 0;
	virtual bool GetShadowCastDirection(Vector* pDirection, ShadowType_t shadowType) const = 0;
	virtual bool IsShadowDirty() = 0;
	virtual void MarkShadowDirty(bool bDirty) = 0;
	virtual IClientRenderable* GetShadowParent() = 0;
	virtual IClientRenderable* FirstShadowChild() = 0;
	virtual IClientRenderable* NextShadowPeer() = 0;
	virtual ShadowType_t ShadowCastType() = 0;
	virtual void Unused2() {}
	virtual void CreateModelInstance() = 0;
	virtual int GetModelInstance() = 0;
	virtual const matrix3x4_t& RenderableToWorldTransform() = 0;
	virtual int LookupAttachment(const char* pAttachmentName) = 0;
	virtual	bool GetAttachment(int number, Vector& origin, QAngle& angles) = 0;
	virtual bool GetAttachment(int number, matrix3x4_t& matrix) = 0;
	virtual bool ComputeLightingOrigin(int nAttachmentIndex, Vector modelLightingCenter, const matrix3x4_t& matrix, Vector& transformedLightingCenter) = 0;
	virtual float* GetRenderClipPlane(void) = 0;
	virtual int		GetSkin() = 0;
	virtual void	OnThreadedDrawSetup() = 0;
	virtual bool	UsesFlexDelayedWeights() = 0;
	virtual void	RecordToolMessage() = 0;
	virtual bool	ShouldDrawForSplitScreenUser(int nSlot) = 0;
	virtual uint8_t	OverrideAlphaModulation(uint8_t nAlpha) = 0;
	virtual uint8_t	OverrideShadowAlphaModulation(uint8_t nAlpha) = 0;
	virtual void* GetClientModelRenderable() = 0;
};

class IClientNetworkable {
public:
	virtual class IClientUnknown* GetIClientUnknown() = 0;
	virtual void Release() = 0;
	virtual ClientClass* GetClientClass() = 0;
	virtual void NotifyShouldTransmit(ShouldTransmitState_t state) = 0;
	virtual void OnPreDataChanged(DataUpdateType_t updateType) = 0;
	virtual void OnDataChanged(DataUpdateType_t updateType) = 0;
	virtual void PreDataUpdate(DataUpdateType_t updateType) = 0;
	virtual void PostDataUpdate(DataUpdateType_t updateType) = 0;
	virtual void OnDataUnchangedInPVS() = 0;
	virtual bool IsDormant(void) const = 0;
	virtual int entindex(void) const = 0;
	virtual void ReceiveMessage(int classID, void* msg) = 0;
	virtual void* GetDataTableBasePtr() = 0;
	virtual void SetDestroyedOnRecreateEntities(void) = 0;
};

struct EntityCacheInfo_t {
	IClientNetworkable* m_pNetworkable;
	unsigned short m_BaseEntitiesIndex;	// Index into m_BaseEntities (or m_BaseEntities.InvalidIndex() if none).
	unsigned short m_bDormant;	// cached dormant state - this is only a bit
};

class IClientThinkable {
public:
	virtual class IClientUnknown* GetIClientUnknown() = 0;
	virtual void				ClientThink() = 0;
	virtual ClientThinkHandle_t	GetThinkHandle() = 0;
	virtual void				SetThinkHandle(ClientThinkHandle_t hThink) = 0;
	virtual void				Release() = 0;
};

class IClientUnknown : public IHandleEntity {
public:
	virtual void* GetCollideable() = 0;
	virtual IClientNetworkable* GetClientNetworkable() = 0;
	virtual IClientRenderable* GetClientRenderable() = 0;
	virtual class IClientEntity* GetIClientEntity() = 0;
	virtual class C_BaseEntity* GetBaseEntity() = 0;
	virtual IClientThinkable* GetClientThinkable() = 0;
	virtual void* GetClientAlphaProperty() = 0;
};

class IClientEntity : public IClientUnknown, public IClientRenderable, public IClientNetworkable, public IClientThinkable {
public:
	virtual void			Release(void) = 0;
	virtual const Vector& GetAbsOrigin(void) const = 0;
	virtual const QAngle& GetAbsAngles(void) const = 0;
	virtual void* GetMouth(void) = 0;
	virtual bool GetSoundSpatialization(int& info) = 0;
	virtual bool IsBlurred(void) = 0;
};

class IClientModelRenderable {
public:
	virtual bool GetRenderData(void* pData, int nCategory) = 0;
};

class C_BaseEntity : public IClientEntity, public IClientModelRenderable {
public:
	virtual							~C_BaseEntity();
	static C_BaseEntity* CreatePredictedEntityByName(const char* classname, const char* module, int line, bool persist = false);
	static void						UpdateVisibilityAllEntities();
	virtual void					FireBullets(const int& info);
	virtual bool					ShouldDrawUnderwaterBulletBubbles();
	virtual bool					ShouldDrawWaterImpacts(void);
	virtual bool					HandleShotImpactingWater(const int& info, const Vector& vecEnd, void* pTraceFilter, Vector* pVecTracerDest);
	virtual void* GetBeamTraceFilter(void);
	virtual void					DispatchTraceAttack(const int& info, const Vector& vecDir, void* ptr);
	virtual void					TraceAttack(const int& info, const Vector& vecDir, void* ptr);
	virtual void					DoImpactEffect(void* tr, int nDamageType);
	virtual void					MakeTracer(const Vector& vecTracerSrc, const void* tr, int iTracerType);
	virtual int						GetTracerAttachment(void);
	void							ComputeTracerStartPosition(const Vector& vecShotSrc, Vector* pVecTracerStart);
	void							TraceBleed(float flDamage, const Vector& vecDir, void* ptr, int bitsDamageType);
	virtual int						BloodColor();
	virtual const char* GetTracerType();
	virtual void					TakeDamage(const int& info) {}
	virtual void					Spawn(void);
	virtual void					SpawnClientEntity(void);
	virtual void					Precache(void);
	virtual void					Activate();
	void							ParseMapData(void* mapData);
	virtual void					OnParseMapDataFinished();
	virtual bool					KeyValue(const char* szKeyName, const char* szValue);
	virtual bool					KeyValue(const char* szKeyName, float flValue);
	virtual bool					KeyValue(const char* szKeyName, int nValue);
	virtual bool					KeyValue(const char* szKeyName, const Vector& vecValue);
	virtual bool					GetKeyValue(const char* szKeyName, char* szValue, int iMaxLen);
	void							SetBlocksLOS(bool bBlocksLOS);
	bool							BlocksLOS(void);
	void							SetAIWalkable(bool bBlocksLOS);
	bool							IsAIWalkable(void);
	virtual void					InitSharedVars(void) {};
};

class CHLClient {
public:
	virtual int						Connect(CreateInterfaceFn appSystemFactory, void* pGlobals);
	virtual void                    Disconnect();
	virtual int						Init(CreateInterfaceFn appSystemFactory, void* pGlobals);
	virtual void					PostInit();
	virtual void					Shutdown(void);
	virtual void					LevelInitPreEntity(const char* pMapName);
	virtual void					LevelInitPostEntity();
	virtual void					LevelShutdown(void);
	virtual ClientClass* GetAllClasses(void);
	virtual int						HudVidInit(void);
	virtual void					HudProcessInput(bool bActive);
	virtual void					HudUpdate(bool bActive);
	virtual void					HudReset(void);
	virtual void					HudText(const char* message);
	virtual void					IN_ActivateMouse(void);
	virtual void					IN_DeactivateMouse(void);
	virtual void					IN_Accumulate(void);
	virtual void					IN_ClearStates(void);
	virtual bool					IN_IsKeyDown(const char* name, bool& isdown);
	//virtual int						IN_KeyEvent(int eventcode, ButtonCode_t keynum, const char* pszCurrentBinding);
	//virtual void					IN_SetSampleTime(float frametime);
	//virtual void					CreateMove(int sequence_number, float input_sample_frametime, bool active);
	//virtual void					ExtraMouseSample(float frametime, bool active);
	//virtual bool					WriteUsercmdDeltaToBuffer(int nSlot, bf_write* buf, int from, int to, bool isnewcommand);
	//virtual void					EncodeUserCmdToBuffer(int nSlot, bf_write& buf, int slot);
	//virtual void					DecodeUserCmdFromBuffer(int nSlot, bf_read& buf, int slot);
	//virtual void					View_Render(vrect_t* rect);
	//virtual void					RenderView(const CViewSetup& view, int nClearFlags, int whatToDraw);
	//virtual void					View_Fade(ScreenFade_t* pSF);
	//virtual void					SetCrosshairAngle(const QAngle& angle);
	//virtual void					InitSprite(CEngineSprite* pSprite, const char* loadname);
	//virtual void					ShutdownSprite(CEngineSprite* pSprite);
	//virtual int						GetSpriteSize(void) const;
	//virtual void					VoiceStatus(int entindex, int iSsSlot, qboolean bTalking);
	//virtual bool					PlayerAudible(int iPlayerIndex);
	//virtual void					InstallStringTableCallback(const char* tableName);
	//virtual void					FrameStageNotify(ClientFrameStage_t curStage);
	//virtual bool					DispatchUserMessage(int msg_type, int32 nFlags, int size, const void* msg);
	//virtual CSaveRestoreData* SaveInit(int size);
	//virtual void			SaveWriteFields(CSaveRestoreData*, const char*, void*, datamap_t*, typedescription_t*, int);
	//virtual void			SaveReadFields(CSaveRestoreData*, const char*, void*, datamap_t*, typedescription_t*, int);
	//virtual void			PreSave(CSaveRestoreData*);
	//virtual void			Save(CSaveRestoreData*);
	//virtual void			WriteSaveHeaders(CSaveRestoreData*);
	//virtual void			ReadRestoreHeaders(CSaveRestoreData*);
	//virtual void			Restore(CSaveRestoreData*, bool);
	//virtual void			DispatchOnRestore();
	//virtual void			WriteSaveGameScreenshot(const char* pFilename);
	//virtual void			EmitSentenceCloseCaption(char const* tokenstream);
	//virtual void			EmitCloseCaption(char const* captionname, float duration);
	//virtual CStandardRecvProxies* GetStandardRecvProxies();
	//virtual bool			CanRecordDemo(char* errorMsg, int length) const;
	//virtual bool			CanStopRecordDemo(char* errorMsg, int length) const;
	//virtual void			OnDemoRecordStart(char const* pDemoBaseName);
	//virtual void			OnDemoRecordStop();
	//virtual void			OnDemoPlaybackStart(char const* pDemoBaseName);
	//virtual void			OnDemoPlaybackRestart();
	//virtual void			OnDemoPlaybackStop();
	//virtual void			SetDemoPlaybackHighlightXuid(uint64 xuid, bool bLowlights);
	//virtual void			ShowHighlightSkippingMessage(bool bState, int nCurrentTick = 0, int nTickStart = 0, int nTickStop = 0);
	//virtual void			RecordDemoPolishUserInput(int nCmdIndex);
	//virtual bool			CacheReplayRagdolls(const char* pFilename, int nStartTick);
	//virtual void			ReplayUI_SendMessage(KeyValues* pMsg);
	//virtual IReplayFactory* GetReplayFactory();
	//virtual void			ClearLocalPlayerReplayPtr();
	//virtual bool			ShouldDrawDropdownConsole();
	//virtual int				GetScreenWidth();
	//virtual int				GetScreenHeight();
	//virtual void			WriteSaveGameScreenshotOfSize(const char* pFilename, int width, int height, bool bCreatePowerOf2Padded/*=false*/, bool bWriteVTF/*=false*/);
	//virtual void			WriteReplayScreenshot(WriteReplayScreenshotParams_t& params);
	//virtual void			UpdateReplayScreenshotCache();
	//virtual bool			GetPlayerView(CViewSetup& playerView);
	//virtual bool			ShouldHideLoadingPlaque(void);
	//virtual void			InvalidateMdlCache();
	//virtual void			OnActiveSplitscreenPlayerChanged(int nNewSlot);
	//virtual void			OnSplitScreenStateChanged();
	//virtual int 			GetSpectatorTarget(ClientDLLObserverMode_t* pObserverMode);
	//virtual void			CenterStringOff();
	//virtual void			OnScreenSizeChanged(int nOldWidth, int nOldHeight);
	//virtual IMaterialProxy* InstantiateMaterialProxy(const char* proxyName);
	//virtual vgui::VPANEL	GetFullscreenClientDLLVPanel(void);
	//virtual void			MarkEntitiesAsTouching(IClientEntity* e1, IClientEntity* e2);
	//virtual void			OnKeyBindingChanged(ButtonCode_t buttonCode, char const* pchKeyName, char const* pchNewBinding);
	//virtual bool			HandleGameUIEvent(const InputEvent_t& event);
	//virtual bool			GetSoundSpatialization(SpatializationInfo_t& info);
};

class IClientEntityList {
public:
	virtual IClientNetworkable* GetClientNetworkable(int entnum) = 0;
	virtual IClientNetworkable* GetClientNetworkableFromHandle(CBaseHandle hEnt) = 0;
	virtual IClientUnknown* GetClientUnknownFromHandle(CBaseHandle hEnt) = 0;
	virtual IClientEntity* GetClientEntity(int entnum) = 0;
	virtual IClientEntity* GetClientEntityFromHandle(CBaseHandle hEnt) = 0;
	virtual int NumberOfEntities(bool bIncludeNonNetworkable) = 0;
	virtual int GetHighestEntityIndex(void) = 0;
	virtual void SetMaxEntities(int maxents) = 0;
	virtual int GetMaxEntities() = 0;
	virtual EntityCacheInfo_t* GetClientNetworkableArray() = 0;
};

class OverlayText_t;

class IVDebugOverlay {
public:
	virtual void AddEntityTextOverlay(int ent_index, int line_offset, float duration, int r, int g, int b, int a, _Printf_format_string_ const char* format, ...) = 0;
	virtual void AddBoxOverlay(const Vector& origin, const Vector& mins, const Vector& max, QAngle const& orientation, int r, int g, int b, int a, float duration) = 0;
	virtual void AddSphereOverlay(const Vector& vOrigin, float flRadius, int nTheta, int nPhi, int r, int g, int b, int a, float flDuration) = 0;
	virtual void AddTriangleOverlay(const Vector& p1, const Vector& p2, const Vector& p3, int r, int g, int b, int a, bool noDepthTest, float duration) = 0;
	virtual void AddLineOverlay(const Vector& origin, const Vector& dest, int r, int g, int b, bool noDepthTest, float duration) = 0;
	virtual void AddTextOverlay(const Vector& origin, float duration, _Printf_format_string_ const char* format, ...) = 0;
	virtual void AddTextOverlay(const Vector& origin, int line_offset, float duration, _Printf_format_string_ const char* format, ...) = 0;
	virtual void AddScreenTextOverlay(float flXPos, float flYPos, float flDuration, int r, int g, int b, int a, const char* text) = 0;
	virtual void AddSweptBoxOverlay(const Vector& start, const Vector& end, const Vector& mins, const Vector& max, const QAngle& angles, int r, int g, int b, int a, float flDuration) = 0;
	virtual void AddGridOverlay(const Vector& origin) = 0;
	virtual void AddCoordFrameOverlay(const matrix3x4_t& frame, float flScale, int vColorTable[3][3] = NULL) = 0;
	virtual int ScreenPosition(const Vector& worldPosition, Vector& screen) = 0;
	virtual int ScreenPosition(float flXPos, float flYPos, Vector& screen) = 0;
	virtual OverlayText_t* GetFirst(void) = 0;
	virtual OverlayText_t* GetNext(OverlayText_t* current) = 0;
	virtual void ClearDeadOverlays(void) = 0;
	virtual void ClearAllOverlays() = 0;
	virtual void AddTextOverlayRGB(const Vector& origin, int line_offset, float duration, float r, float g, float b, float alpha, _Printf_format_string_ const char* format, ...) = 0;
	virtual void AddTextOverlayRGB(const Vector& origin, int line_offset, float duration, int r, int g, int b, int a, _Printf_format_string_ const char* format, ...) = 0;
	virtual void AddLineOverlayAlpha(const Vector& origin, const Vector& dest, int r, int g, int b, int a, bool noDepthTest, float duration) = 0;
	virtual void AddBoxOverlay2(const Vector& origin, const Vector& mins, const Vector& max, QAngle const& orientation, const Vector4D& faceColor, const Vector4D& edgeColor, float duration) = 0;
	virtual void AddLineOverlay(const Vector& origin, const Vector& dest, int r, int g, int b, int a, float thickness, float duration) = 0;
	virtual void PurgeTextOverlays() = 0;
	virtual void AddCapsuleOverlay(const Vector& vStart, const Vector& vEnd, const float& flRadius, int r, int g, int b, int a, float flDuration) = 0;
};

class CBaseEntity : public IClientEntity {
public:
	VAROFF(m_flAnimTime, int, 0x260);
	VAROFF(AnimTimeMustBeFirst, RecvTable*, 0x0);
	VAROFF(m_flSimulationTime, int, 0x268);
	VAROFF(m_cellbits, int, 0x74);
	VAROFF(m_cellX, int, 0x7c);
	VAROFF(m_cellY, int, 0x80);
	VAROFF(m_cellZ, int, 0x84);
	VAROFF(m_vecOrigin, Vector, 0x138);
	VAROFF(m_angRotation, Vector, 0x12c);
	VAROFF(m_nModelIndex, int, 0x258);
	VAROFF(m_fEffects, int, 0xf0);
	VAROFF(m_nRenderMode, int, 0x25b);
	VAROFF(m_nRenderFX, int, 0x25a);
	VAROFF(m_clrRender, int, 0x70);
	VAROFF(m_iTeamNum, int, 0xf4);
	VAROFF(m_iPendingTeamNum, int, 0xf8);
	VAROFF(m_CollisionGroup, int, 0x474);
	VAROFF(m_flElasticity, float, 0x300);
	VAROFF(m_flShadowCastDistance, float, 0x3a0);
	VAROFF(m_hOwnerEntity, int, 0x14c);
	VAROFF(m_hEffectEntity, int, 0x998);
	VAROFF(moveparent, int, 0x148);
	VAROFF(m_iParentAttachment, int, 0x2ec);
	VAROFF(m_iName, char*, 0x154);
	VAROFF(movetype, int, 0x0);
	VAROFF(movecollide, int, 0x0);
	VAROFF(m_vecMins, Vector, 0x8);
	VAROFF(m_vecMaxs, Vector, 0x14);
	VAROFF(m_nSolidType, int, 0x22);
	VAROFF(m_usSolidFlags, int, 0x20);
	VAROFF(m_nSurroundType, int, 0x2a);
	VAROFF(m_triggerBloat, int, 0x23);
	VAROFF(m_vecSpecifiedSurroundingMins, Vector, 0x2c);
	VAROFF(m_vecSpecifiedSurroundingMaxs, Vector, 0x38);
	VAROFF(m_Collision, RecvTable*, 0x320);
	VAROFF(m_iTextureFrameIndex, int, 0x98c);
	VAROFF(m_bSimulatedEveryTick, int, 0x93a);
	VAROFF(m_bAnimatedEveryTick, int, 0x93b);
	VAROFF(m_bAlternateSorting, int, 0x93c);
	VAROFF(m_bSpotted, int, 0x93d);
	VAROFF(m_bSpottedBy, RecvTable*, 0x93e);
	VAROFF(m_bSpottedByMask, RecvTable*, 0x980);
	VAROFF(m_bIsAutoaimTarget, int, 0x60);
	VAROFF(m_fadeMinDist, float, 0x2f4);
	VAROFF(m_fadeMaxDist, float, 0x2f8);
	VAROFF(m_flFadeScale, float, 0x2fc);
	VAROFF(m_nMinCPULevel, int, 0x988);
	VAROFF(m_nMaxCPULevel, int, 0x989);
	VAROFF(m_nMinGPULevel, int, 0x98a);
	VAROFF(m_nMaxGPULevel, int, 0x98b);
	VAROFF(m_flUseLookAtAngle, float, 0x2cc);
	VAROFF(m_flLastMadeNoiseTime, float, 0x20);
	VAROFF(m_flMaxFallVelocity, float, 0xdc);
	VAROFF(m_bEligibleForScreenHighlight, int, 0x9b9);
};

class CCSPlayer {
public:
	VAROFF(m_vecOrigin, Vector, 0x138);
	VAROFF(m_flStamina, float, 0x103d8);
	VAROFF(m_iDirection, int, 0x103dc);
	VAROFF(m_iShotsFired, int, 0x103e0);
	VAROFF(m_nNumFastDucks, int, 0x103e4);
	VAROFF(m_bDuckOverride, int, 0x103e8);
	VAROFF(m_flVelocityModifier, float, 0x103ec);
	VAROFF(m_bPlayerDominated, RecvTable*, 0x118e8);
	VAROFF(m_bPlayerDominatingMe, RecvTable*, 0x11929);
	VAROFF(m_iWeaponPurchasesThisRound, RecvTable*, 0x11298);
	VAROFF(m_unActiveQuestId, int, 0x11778);
	VAROFF(m_nQuestProgressReason, int, 0x1177c);
	VAROFF(m_iRetakesOffering, int, 0x11cf4);
	VAROFF(m_iRetakesOfferingCard, int, 0x11cf8);
	VAROFF(m_bRetakesHasDefuseKit, int, 0x11cfc);
	VAROFF(m_bRetakesMVPLastRound, int, 0x11cfd);
	VAROFF(m_iRetakesMVPBoostItem, int, 0x11d00);
	VAROFF(m_RetakesMVPBoostExtraUtility, int, 0x11d04);
	VAROFF(m_unPlayerTvControlFlags, int, 0x11780);
	VAROFF(cslocaldata, RecvTable*, 0x0);
	VAROFF(csnonlocaldata, RecvTable*, 0x0);
	VAROFF(m_iWeaponPurchasesThisMatch, RecvTable*, 0x10acc);
	VAROFF(m_EquippedLoadoutItemDefIndices, RecvTable*, 0x116a4);
	VAROFF(csteamdata, RecvTable*, 0x0);
	VAROFF(m_iAddonBits, int, 0x103c4);
	VAROFF(m_iPrimaryAddon, int, 0x103c8);
	VAROFF(m_iSecondaryAddon, int, 0x103cc);
	VAROFF(m_iThrowGrenadeCounter, int, 0x99b8);
	VAROFF(m_bWaitForNoAttack, int, 0x99bc);
	VAROFF(m_bIsRespawningForDMBonus, int, 0x99bd);
	VAROFF(m_iPlayerState, int, 0x9978);
	VAROFF(m_iAccount, int, 0x117b8);
	VAROFF(m_iStartAccount, int, 0x103f4);
	VAROFF(m_totalHitsOnServer, int, 0x103f8);
	VAROFF(m_bInBombZone, int, 0x99b4);
	VAROFF(m_bInBuyZone, int, 0x99b5);
	VAROFF(m_bInNoDefuseArea, int, 0x99b6);
	VAROFF(m_bKilledByTaser, int, 0x99cd);
	VAROFF(m_iMoveState, int, 0x99d0);
	VAROFF(m_iClass, int, 0x117c8);
	VAROFF(m_ArmorValue, int, 0x117cc);
	VAROFF(m_angEyeAngles, Vector, 0x117d0);
	VAROFF(m_bHasDefuser, int, 0x117dc);
	VAROFF(m_bNightVisionOn, int, 0x103e9);
	VAROFF(m_bHasNightVision, int, 0x103ea);
	VAROFF(m_bInHostageRescueZone, int, 0x117dd);
	VAROFF(m_bIsDefusing, int, 0x997c);
	VAROFF(m_bIsGrabbingHostage, int, 0x997d);
	VAROFF(m_iBlockingUseActionInProgress, int, 0x9980);
	VAROFF(m_bIsScoped, int, 0x9974);
	VAROFF(m_bIsWalking, int, 0x9975);
	VAROFF(m_nIsAutoMounting, int, 0x9ae4);
	VAROFF(m_bResumeZoom, int, 0x9976);
	VAROFF(m_fImmuneToGunGameDamageTime, float, 0x9988);
	VAROFF(m_bGunGameImmunity, int, 0x9990);
	VAROFF(m_bHasMovedSinceSpawn, int, 0x9991);
	VAROFF(m_bMadeFinalGunGameProgressiveKill, int, 0x9992);
	VAROFF(m_iGunGameProgressiveWeaponIndex, int, 0x9994);
	VAROFF(m_iNumGunGameTRKillPoints, int, 0x9998);
	VAROFF(m_iNumGunGameKillsWithCurrentWeapon, int, 0x999c);
	VAROFF(m_iNumRoundKills, int, 0x99a0);
	VAROFF(m_fMolotovUseTime, float, 0x99ac);
	VAROFF(m_fMolotovDamageTime, float, 0x99b0);
	VAROFF(m_szArmsModel, char*, 0x99d7);
	VAROFF(m_hCarriedHostage, int, 0x10448);
	VAROFF(m_hCarriedHostageProp, int, 0x1044c);
	VAROFF(m_bIsRescuing, int, 0x9984);
	VAROFF(m_flGroundAccelLinearFracLastTime, float, 0x103f0);
	VAROFF(m_bCanMoveDuringFreezePeriod, int, 0x99d4);
	VAROFF(m_isCurrentGunGameLeader, int, 0x99d5);
	VAROFF(m_isCurrentGunGameTeamLeader, int, 0x99d6);
	VAROFF(m_flGuardianTooFarDistFrac, float, 0x99c0);
	VAROFF(m_flDetectedByEnemySensorTime, float, 0x99c4);
	VAROFF(m_bIsPlayerGhost, int, 0x9ae1);
	VAROFF(m_iMatchStats_Kills, RecvTable*, 0x104b4);
	VAROFF(m_iMatchStats_Damage, RecvTable*, 0x1052c);
	VAROFF(m_iMatchStats_EquipmentValue, RecvTable*, 0x105a4);
	VAROFF(m_iMatchStats_MoneySaved, RecvTable*, 0x1061c);
	VAROFF(m_iMatchStats_KillReward, RecvTable*, 0x10694);
	VAROFF(m_iMatchStats_LiveTime, RecvTable*, 0x1070c);
	VAROFF(m_iMatchStats_Deaths, RecvTable*, 0x10784);
	VAROFF(m_iMatchStats_Assists, RecvTable*, 0x107fc);
	VAROFF(m_iMatchStats_HeadShotKills, RecvTable*, 0x10874);
	VAROFF(m_iMatchStats_Objective, RecvTable*, 0x108ec);
	VAROFF(m_iMatchStats_CashEarned, RecvTable*, 0x10964);
	VAROFF(m_iMatchStats_UtilityDamage, RecvTable*, 0x109dc);
	VAROFF(m_iMatchStats_EnemiesFlashed, RecvTable*, 0x10a54);
	VAROFF(m_rank, RecvTable*, 0x1178c);
	VAROFF(m_passiveItems, RecvTable*, 0x117a4);
	VAROFF(m_bHasParachute, int, 0x117a4);
	VAROFF(m_unMusicID, int, 0x117a8);
	VAROFF(m_bHasHelmet, int, 0x117c0);
	VAROFF(m_bHasHeavyArmor, int, 0x117c1);
	VAROFF(m_nHeavyAssaultSuitCooldownRemaining, int, 0x117c4);
	VAROFF(m_flFlashDuration, float, 0x10470);
	VAROFF(m_flFlashMaxAlpha, float, 0x1046c);
	VAROFF(m_iProgressBarDuration, int, 0x103d0);
	VAROFF(m_flProgressBarStartTime, float, 0x103d4);
	VAROFF(m_hRagdoll, int, 0x1043c);
	VAROFF(m_hPlayerPing, int, 0x10440);
	VAROFF(m_cycleLatch, int, 0x118e0);
	VAROFF(m_unCurrentEquipmentValue, int, 0x11784);
	VAROFF(m_unRoundStartEquipmentValue, int, 0x11786);
	VAROFF(m_unFreezetimeEndEquipmentValue, int, 0x11788);
	VAROFF(m_bIsControllingBot, int, 0x11a4d);
	VAROFF(m_bHasControlledBotThisRound, int, 0x11a5c);
	VAROFF(m_bCanControlObservedBot, int, 0x11a4e);
	VAROFF(m_iControlledBotEntIndex, int, 0x11a50);
	VAROFF(m_vecAutomoveTargetEnd, Vector, 0x9af4);
	VAROFF(m_flAutoMoveStartTime, float, 0x9b04);
	VAROFF(m_flAutoMoveTargetTime, float, 0x9b08);
	VAROFF(m_bIsAssassinationTarget, int, 0x11a4c);
	VAROFF(m_bHud_MiniScoreHidden, int, 0x117fe);
	VAROFF(m_bHud_RadarHidden, int, 0x117ff);
	VAROFF(m_nLastKillerIndex, int, 0x11800);
	VAROFF(m_nLastConcurrentKilled, int, 0x11804);
	VAROFF(m_nDeathCamMusic, int, 0x11808);
	VAROFF(m_bIsHoldingLookAtWeapon, int, 0x11975);
	VAROFF(m_bIsLookingAtWeapon, int, 0x11974);
	VAROFF(m_iNumRoundKillsHeadshots, int, 0x99a4);
	VAROFF(m_unTotalRoundDamageDealt, int, 0x99a8);
	VAROFF(m_flLowerBodyYawTarget, float, 0x9adc);
	VAROFF(m_bStrafing, int, 0x9ae0);
	VAROFF(m_flThirdpersonRecoil, float, 0x119ec);
	VAROFF(m_bHideTargetID, int, 0x119f0);
	VAROFF(m_bIsSpawnRappelling, int, 0x103fd);
	VAROFF(m_vecSpawnRappellingRopeOrigin, Vector, 0x10400);
	VAROFF(m_nSurvivalTeam, int, 0x10410);
	VAROFF(m_hSurvivalAssassinationTarget, int, 0x10414);
	VAROFF(m_flHealthShotBoostExpirationTime, float, 0x10418);
	VAROFF(m_flLastExoJumpTime, float, 0x9b0c);
	VAROFF(m_vecPlayerPatchEconIndices, RecvTable*, 0x119c4);
};


static constexpr unsigned int JS_MAX = 65535;
char jsCode[JS_MAX];
std::string mainXML = R"zk1sakz(
<root>
  <styles>
    <include src="file://{resources}/styles/csgostyles.css" />
    <include src="file://{resources}/styles/settings/settings.css" />
    <include src="file://{resources}/styles/mainmenu.css"/>
  </styles>

  <scripts>
    <include src="file://{resources}/scripts/settingsmenu.js" />
    <include src="file://{resources}/scripts/settingsmenu_shared.js" />
    <include src="file://{resources}/scripts/buymenu.js" />
  </scripts>

  <script>
    var updateConVarToggleButton = function (convarname) {
      var elToggle = $.GetContextPanel().FindChildInLayoutFile( convarname );
      GameInterfaceAPI.SetSettingString( convarname, elToggle.checked ? '1' : '0' );
    };
  </script>

  <Panel class='Main mainmenu-content__container' style='width: 700px; height: 500px; horizontal-align: right; vertical-align: center; background-color: gradient( linear, 0% 0%, 0% 100%, from( #1e2a38dd ), to( #19232fdd ) );'>
        <Label text='CrossSkelet : public Skeletux' style='horizontal-align: center; vertical-align: top; font-size: 24px; color: grey;'/>
        <Panel class="horizontal-separator"/>
        <Panel style='vertical-align: top; horizontal-align: left; flow-children: down;'>
          <Panel style='horizontal-align: left;margin-bottom: 10px; flow-children: right; color:red;'>
            <TextButton class="PopupButton" onactivate="SteamOverlayAPI.OpenURL( 'https://unknowncheats.me/' );" >
              <Label style="color:red;" text="Red Button"/>
            </TextButton>
            <TextButton class="PopupButton" text="button2" onactivate="SteamOverlayAPI.CopyTextToClipboard('rip ur buffer :trollface:');$.Msg('button pressed')" />
          </Panel>
          <Panel class="horizontal-separator"/>
       
          <Panel style='horizontal-align: left; margin-bottom: 10px;'>
            <ToggleButton id="skele_enable_bhop" data-category="skele" class="PopupButton Row" onactivate="updateConVarToggleButton('skele_enable_bhop');" >
              <Label id="" text="BunnyHop" />
            </ToggleButton>
          </Panel>
          <Panel style='horizontal-align: left; margin-bottom: 10px;'>
            <ToggleButton id="skele_enable_esp" data-category="skele" class="PopupButton Row" onactivate="updateConVarToggleButton('skele_enable_esp');" >
              <Label id="" text="Chicken ESP" />
            </ToggleButton>
          </Panel>

          <Panel class="horizontal-separator"/>

          <CSGOSettingsSlider text="example" max="8" min="0.05" displayprecision="2" convar="skele_example_slider"/>
          <CSGOSettingsSlider text="sensitivity" max="8" min="0.05" displayprecision="2" convar="sensitivity"/>
        </Panel>
    </Panel>
</root>

)zk1sakz";
