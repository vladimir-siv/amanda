#pragma once

#include "components.h"
#include "scanning/component_scanner.h"
#include "scanning/scanner.h"

#include "../xml/api.h"
#include "../lib/string_builder.h"

#include <dependency.h>
#include <structures/specialized/vlist.h>

class HardwareController
{
	protected: vlist<IComponent> _components;
	
	public: HardwareController() : _components(dependency::node_allocator()) { }
	public: virtual ~HardwareController() { }
	
	public: unsigned int size() const { return _components.size(); }
	
	public: const IComponent* operator[](unsigned int index) const { return const_cast<HardwareController&>(*this)[index]; }
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
	
	public: virtual void scan(Scanner* scanner)
	{
		ComponentScanner csc = scanner;

		auto it = _components.begin();
		auto end = _components.end();
		
		for (; it != end; ++it) csc.scan(*it);
	}
};
