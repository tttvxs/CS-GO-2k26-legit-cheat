#pragma once
#include<Windows.h>
#include <thread>
#include "utils/memory.h"
#include "utils/vector.h"
#include <unordered_set>
#include "dt/dt_recv.h"
#include <string>
#include <fstream>

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

class CBaseHandle;
struct model_t;

class CClientThinkHandlePtr;
typedef CClientThinkHandlePtr* ClientThinkHandle_t;
typedef void* (*CreateInterfaceFn)(const char* pName, int* pReturnCode);
typedef void* (*InstantiateInterfaceFn)();
typedef class IClientNetworkable* (*CreateClientClassFn)(int entnum, int serialNum);
typedef class IClientNetworkable* (*CreateEventFn)();

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