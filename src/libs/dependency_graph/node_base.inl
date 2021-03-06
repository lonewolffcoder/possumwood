#pragma once

#include "node_base.h"
#include "network.h"
#include "rtti.h"
#include "graph.h"
#include "datablock.inl"

namespace dependency_graph {

template<typename T>
void NodeBase::setBlindData(const T& value) {
	// create blind data if they're not present
	bool newData = false;
	if(m_blindData.get() == NULL) {
		m_blindData = std::unique_ptr<BaseData>(new Data<T>());
		newData = true;
	}

	// retype
	Data<T>& val = dynamic_cast<Data<T>&>(*m_blindData);

	// set the value
	if(val.value != value) {
		val.value = value;

		// and call the callback
		if(!newData)
			network().graph().blindDataChanged(*this);
	}
}

/// blind per-node data, to be used by the client application
///   to store visual information (e.g., node position, colour...)
template<typename T>
const T& NodeBase::blindData() const {
	// retype and return
	assert(m_blindData != NULL);
	assert(m_blindData->type() == unmangledTypeId<T>());
	const Data<T>& val = dynamic_cast<const Data<T>&>(*m_blindData);
	return val.value;
}

// template<typename T>
// bool NodeBase::is() const {
// 	return dynamic_cast<const T*>(this) != nullptr;
// }

template<typename T>
const T& NodeBase::as() const {
	assert(is<T>());
	return dynamic_cast<const T&>(*this);
}

template<typename T>
T& NodeBase::as() {
	assert(is<T>());
	return dynamic_cast<T&>(*this);
}

}
