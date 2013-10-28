#ifndef JAVADataModel_H_7U446XJQ
#define JAVADataModel_H_7U446XJQ

#include "../../../uscxml/Message.h"
#include "../../../uscxml/Factory.h"
#include "../../../uscxml/Interpreter.h"

namespace uscxml {

class JavaDataModel : public DataModelImpl {
public:
	JavaDataModel();
	virtual ~JavaDataModel();

	virtual JavaDataModel* create(Interpreter interpreter) {
		return new JavaDataModel();
	}

	virtual boost::shared_ptr<DataModelImpl> create(InterpreterImpl* interpreter) {
		return boost::shared_ptr<DataModelImpl>(create(interpreter->shared_from_this()));
	}
	virtual std::set<std::string> getNames() {
		return std::set<std::string>();
	};

	virtual bool validate(const std::string& location, const std::string& schema) {
		return true;
	}
	virtual void setEvent(const Event& event) {}
	virtual Data getStringAsData(const std::string& content) {
		Data data;
		return data;
	}

	// foreach
	virtual uint32_t getLength(const std::string& expr) {
		return 0;
	}
	virtual void setForeach(const std::string& item,
	                        const std::string& array,
	                        const std::string& index,
	                        uint32_t iteration) {}
	virtual void pushContext() {}
	virtual void popContext() {}

	virtual void eval(const Arabica::DOM::Element<std::string>& scriptElem,
	                  const std::string& expr) {
		std::ostringstream ssEval;
		ssEval << scriptElem;
		eval(ssEval.str(), expr);
	}

	virtual std::string evalAsString(const std::string& expr) {
		return "";
	}
	virtual bool evalAsBool(const std::string& expr) {
		return false;
	}

	virtual bool evalAsBool(const Arabica::DOM::Node<std::string>& node, const std::string& expr) {
		return false;
	}

	virtual bool isDeclared(const std::string& expr) {
		return false;
	}

	virtual void assign(const Arabica::DOM::Element<std::string>& assignElem,
	                    const Arabica::DOM::Node<std::string>& node,
	                    const std::string& content) {
		// convert XML back into strings
		std::string location;
		if (assignElem.hasAttribute("location")) {
			location = assignElem.getAttribute("location");
		}
		std::ostringstream ssAssign;
		ssAssign << assignElem;
		std::string tmp;
		if (node) {
			std::ostringstream ssContent;
			ssContent << node;
			tmp = ssContent.str();
		} else {
			tmp = content;
		}
		assign(location, ssAssign.str(), tmp);
	}

	virtual void assign(const std::string& location, const Data& data) {
		init("", location, Data::toJSON(data));
	}

	// this is assign the function exposed to java

	virtual void init(const Arabica::DOM::Element<std::string>& dataElem,
	                  const Arabica::DOM::Node<std::string>& node,
	                  const std::string& content) {
		// convert XML back into strings
		std::string location;
		if (dataElem.hasAttribute("id")) {
			location = dataElem.getAttribute("id");
		}
		std::ostringstream ssData;
		if (dataElem)
			ssData << dataElem;
		std::string tmp;
		if (node) {
			std::ostringstream ssContent;
			ssContent << node;
			tmp = ssContent.str();
		} else {
			tmp = content;
		}
		init(location, ssData.str(), tmp);
	}

	virtual void init(const std::string& location, const Data& data) {
		init("", location, Data::toJSON(data));
	}

	// these functions are exposed to java
	virtual void init(const std::string& dataElem, const std::string& location, const std::string& content) {}
	virtual void assign(const std::string& assignElem, const std::string& location, const std::string& content) {}
	virtual void eval(const std::string& scriptElem, const std::string& expr) {}

};

}

#endif /* end of include guard: JAVADataModel_H_7U446XJQ */
