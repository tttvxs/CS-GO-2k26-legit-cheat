#include "dt_recv.h"

 void RecvProp::InitArray(int nElements, int elementStride) {
	m_RecvType = DPT_Array;
	m_nElements = nElements;
	m_ElementStride = elementStride;
}

 int RecvProp::GetNumElements() const {
	return m_nElements;
}

 void RecvProp::SetNumElements(int nElements) {
	m_nElements = nElements;
}

 int RecvProp::GetElementStride() const {
	return m_ElementStride;
}

 void RecvProp::SetElementStride(int stride) {
	m_ElementStride = stride;
}

 int RecvProp::GetFlags() const {
	return m_Flags;
}

 const char* RecvProp::GetName() const {
	return m_pVarName;
}

 SendPropType RecvProp::GetType() const {
	return m_RecvType;
}

 void* RecvProp::GetDataTable() const {
	return m_pDataTable;
}

 void RecvProp::SetDataTable(void* pTable) {
	m_pDataTable = pTable;
}

 RecvVarProxyFn RecvProp::GetProxyFn() const {
	return m_ProxyFn;
}

 void RecvProp::SetProxyFn(RecvVarProxyFn fn) {
	m_ProxyFn = fn;
}

 DataTableRecvVarProxyFn RecvProp::GetDataTableProxyFn() const  {
	return m_DataTableProxyFn;
}

 void RecvProp::SetDataTableProxyFn(DataTableRecvVarProxyFn fn) {
	m_DataTableProxyFn = fn;
}

 int RecvProp::GetOffset() const {
	return m_Offset;
}

 void RecvProp::SetOffset(int o) {
	m_Offset = o;
}

 RecvProp* RecvProp::GetArrayProp() const {
	return m_pArrayProp;
}

 void RecvProp::SetArrayProp(RecvProp* pProp) {
	m_pArrayProp = pProp;
}

 void RecvProp::SetArrayLengthProxy(ArrayLengthRecvProxyFn proxy) {
	m_ArrayLengthProxy = proxy;
}

 ArrayLengthRecvProxyFn RecvProp::GetArrayLengthProxy() const {
	return m_ArrayLengthProxy;
}

 bool RecvProp::IsInsideArray() const {
	return m_bInsideArray;
}

 void RecvProp::SetInsideArray() {
	m_bInsideArray = true;
}

 const void* RecvProp::GetExtraData() const {
	return m_pExtraData;
}

 void RecvProp::SetExtraData(const void* pData) {
	m_pExtraData = pData;
}

 const char* RecvProp::GetParentArrayPropName() {
	return m_pParentArrayPropName;
}

void	RecvProp::SetParentArrayPropName(const char* pArrayPropName) {
	m_pParentArrayPropName = pArrayPropName;
}

int RecvTable::GetNumProps() {
	return m_nProps;
}

RecvProp* RecvTable::GetProp(int i) {
	return &m_pProps[i];
}

const char* RecvTable::GetName() const {
	return m_pNetTableName;
}

void RecvTable::SetInitialized(bool bInitialized) {
	m_bInitialized = bInitialized;
}

bool RecvTable::IsInitialized() const {
	return m_bInitialized;
}

void RecvTable::SetInMainList(bool bInList) {
	m_bInMainList = bInList;
}

bool RecvTable::IsInMainList() const {
	return m_bInMainList;
}
