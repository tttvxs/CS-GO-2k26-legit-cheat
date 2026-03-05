#pragma once
#include "dt_common.h"

class CRecvProxyData {
public:
	void* m_pRecvProp;		// The property it's receiving.
	DVariant		m_Value;			// The value given to you to store.
	int				m_iElement;			// Which array element you're getting.
	int				m_ObjectID;			// The object being referred to.
};

typedef void (*RecvVarProxyFn)(const CRecvProxyData* pData, void* pStruct, void* pOut);
typedef void (*ArrayLengthRecvProxyFn)(void* pStruct, int objectID, int currentArrayLength);
typedef void (*DataTableRecvVarProxyFn)(void* pProp, void** pOut, void* pData, int objectID);

class CStandardRecvProxies {
public:
	CStandardRecvProxies() {};
	RecvVarProxyFn m_Int32ToInt8;
	RecvVarProxyFn m_Int32ToInt16;
	RecvVarProxyFn m_Int32ToInt32;
	RecvVarProxyFn m_Int64ToInt64;
	RecvVarProxyFn m_FloatToFloat;
	RecvVarProxyFn m_VectorToVector;
};

class RecvProp {
public:
	RecvProp() {};
	void					InitArray(int nElements, int elementStride);
	int						GetNumElements() const;
	void					SetNumElements(int nElements);
	int						GetElementStride() const;
	void					SetElementStride(int stride);
	int						GetFlags() const;
	const char* GetName() const;
	SendPropType			GetType() const;
	void* GetDataTable() const;
	void					SetDataTable(void* pTable);
	RecvVarProxyFn			GetProxyFn() const;
	void					SetProxyFn(RecvVarProxyFn fn);
	DataTableRecvVarProxyFn	GetDataTableProxyFn() const;
	void					SetDataTableProxyFn(DataTableRecvVarProxyFn fn);
	int						GetOffset() const;
	void					SetOffset(int o);
	RecvProp* GetArrayProp() const;
	void					SetArrayProp(RecvProp* pProp);
	void					SetArrayLengthProxy(ArrayLengthRecvProxyFn proxy);
	ArrayLengthRecvProxyFn	GetArrayLengthProxy() const;
	bool					IsInsideArray() const;
	void					SetInsideArray();
	const void* GetExtraData() const;
	void SetExtraData(const void* pData);
	const char* GetParentArrayPropName();
	void SetParentArrayPropName(const char* pArrayPropName);

public:
	char* m_pVarName;
	SendPropType			m_RecvType;
	int						m_Flags;
	int						m_StringBufferSize;

private:
	bool m_bInsideArray;		// Set to true by the engine if this property sits inside an array.
	const void* m_pExtraData;
	RecvProp* m_pArrayProp;
	ArrayLengthRecvProxyFn	m_ArrayLengthProxy;
	RecvVarProxyFn m_ProxyFn;
	DataTableRecvVarProxyFn	m_DataTableProxyFn;	// For RDT_DataTable.
	void* m_pDataTable;		// For RDT_DataTable.
	int m_Offset;
	int m_ElementStride;
	int m_nElements;
	const char* m_pParentArrayPropName;
};


class RecvTable {
public:
	typedef RecvProp PropType;
	RecvTable() {};

	int			GetNumProps();
	RecvProp* GetProp(int i);
	const char* GetName() const;
	void		SetInitialized(bool bInitialized);
	bool		IsInitialized() const;
	void		SetInMainList(bool bInList);
	bool		IsInMainList() const;

	RecvProp* m_pProps;
	int				m_nProps;
	void* m_pDecoder;
	char* m_pNetTableName;
private:
	bool			m_bInitialized;
	bool			m_bInMainList;
};
