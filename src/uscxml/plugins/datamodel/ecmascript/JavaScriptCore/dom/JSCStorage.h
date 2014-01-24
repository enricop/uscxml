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

#ifndef JSCStorage_h
#define JSCStorage_h

#include <string>
#include "DOM/Node.hpp"
#include <JavaScriptCore/JavaScriptCore.h>
#include "uscxml/plugins/datamodel/ecmascript/JavaScriptCore/JSCDOM.h"

namespace Arabica {
namespace DOM {

class JSCStorage {
public:
	struct JSCStoragePrivate {
		JSCDOM* dom;
		uscxml::Storage* nativeObj;
	};

	JSC_DESTRUCTOR_KEEP_WRAPPED(JSCStoragePrivate);

	static JSValueRef keyCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObj, size_t argumentCount, const JSValueRef* arguments, JSValueRef* exception);
	static JSValueRef getItemCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObj, size_t argumentCount, const JSValueRef* arguments, JSValueRef* exception);
	static JSValueRef setItemCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObj, size_t argumentCount, const JSValueRef* arguments, JSValueRef* exception);
	static JSValueRef removeItemCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObj, size_t argumentCount, const JSValueRef* arguments, JSValueRef* exception);
	static JSValueRef clearCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObj, size_t argumentCount, const JSValueRef* arguments, JSValueRef* exception);

	static JSValueRef lengthAttrGetter(JSContextRef ctx, JSObjectRef thisObj, JSStringRef propertyName, JSValueRef* exception);


	static JSStaticValue staticValues[];
	static JSStaticFunction staticFunctions[];

	static JSClassRef Tmpl;
	static JSClassRef getTmpl() {
		if (Tmpl == NULL) {
			JSClassDefinition classDef = kJSClassDefinitionEmpty;
			classDef.staticValues = staticValues;
			classDef.staticFunctions = staticFunctions;
			classDef.finalize = jsDestructor;
			classDef.className = "Storage";

			Tmpl = JSClassCreate(&classDef);
			JSClassRetain(Tmpl);
		}
		return Tmpl;
	}


};

}
}

#endif // JSCStorage_h
