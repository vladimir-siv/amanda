#pragma once

#include "components.h"
#include "scanning/component_scanner.h"

#include "../common/data/stream.h"

#include "../xml/api.h"

#include <dependency.h>
#include <structures/specialized/vlist.h>

class HardwareController
{
	protected: vlist<IComponent> _components;
	
	public: HardwareController() : _components(D::nodes) { *this += &TMR::instance(); }
	public: virtual ~HardwareController() { }
	
	public: unsigned int size() const { return _components.size(); }
	
	public: const IComponent* operator[](unsigned int index) const
	{
		return const_cast<HardwareController&>(*this)[index];
	}
	public: IComponent* operator[](unsigned int index)
	{
		auto it = _components.begin();
		for (unsigned int i = 0; i < index; ++i) ++it;
		return *it;
	}
	
	public: HardwareController& operator+=(IComponent* component)
	{
		_components.push_back(component);
		return *this;
	}
	
	public: vlist_enumerator<IComponent> begin() { return _components.begin(); }
	public: vlist_enumerator<IComponent> end() { return _components.end(); }
	public: const vlist_enumerator<IComponent> cbegin() const { return _components.cbegin(); }
	public: const vlist_enumerator<IComponent> cend() const { return _components.cend(); }
	
	public: IComponent* find(VID vid, const char* name) { return find(vid, IComponent::resolveType(name)); }
	public: IComponent* find(VID vid, IComponent::Type type)
	{
		if (type == IComponent::Type::NONE) return nullptr;

		for (auto i = begin(); i != end(); ++i)
		{
			if (i->is(type) && i->ID() == vid) return *i;
		}

		return nullptr;
	}
	
	public: virtual void scan(data::OutputStream* stream)
	{
		if (stream == nullptr) return;

		ComponentScanner csc = *stream;

		auto it = _components.begin();
		auto end = _components.end();
		
		stream->print(F("<scan>"));
		for (; it != end; ++it) csc.scan(*it);
		stream->print(F("</scan>"));
	}
};
