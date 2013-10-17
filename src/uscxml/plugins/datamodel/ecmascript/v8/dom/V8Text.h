/**
 *  @file
 *  @author     This file has been generated by generate-bindings.pl. DO NOT MODIFY!
 *  @copyright  Simplified BSD
 *
 *  @cond
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the FreeBSD license as published by the FreeBSD
 *  project.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *  You should have received a copy of the FreeBSD license along with this
 *  program. If not, see <http://www.opensource.org/licenses/bsd-license>.
 *  @endcond
 */


#ifndef V8Text_h
#define V8Text_h

#include <string>
#include "DOM/Node.hpp"
#include "V8CharacterData.h"
#include "string"
#include "uscxml/plugins/datamodel/ecmascript/v8/V8DOM.h"
#include <v8.h>

namespace Arabica {
namespace DOM {

class V8Text {
public:
	struct V8TextPrivate {
		V8DOM* dom;
		Arabica::DOM::Text<std::string>* nativeObj;
	};

	V8_DESTRUCTOR(V8TextPrivate);
	static bool hasInstance(v8::Handle<v8::Value>);

	static v8::Handle<v8::Value> splitTextCallback(const v8::Arguments&);


	static v8::Persistent<v8::FunctionTemplate> Tmpl;
	static v8::Handle<v8::FunctionTemplate> getTmpl() {
		if (Tmpl.IsEmpty()) {
			v8::Handle<v8::FunctionTemplate> tmpl = v8::FunctionTemplate::New();
			tmpl->SetClassName(v8::String::New("Text"));
			tmpl->ReadOnlyPrototype();

			v8::Local<v8::ObjectTemplate> instance = tmpl->InstanceTemplate();
			v8::Local<v8::ObjectTemplate> prototype = tmpl->PrototypeTemplate();
			(void)prototype; // surpress unused warnings

			instance->SetInternalFieldCount(1);


			prototype->Set(v8::String::NewSymbol("splitText"),
			               v8::FunctionTemplate::New(V8Text::splitTextCallback, v8::Undefined()), static_cast<v8::PropertyAttribute>(v8::DontDelete));


			tmpl->Inherit(V8CharacterData::getTmpl());
			Tmpl = v8::Persistent<v8::FunctionTemplate>::New(tmpl);
		}
		return Tmpl;
	}


};

}
}

#endif // V8Text_h
