#ifndef LETTERPRESS_SCRIPTENGINE_METHOD_HPP
#define LETTERPRESS_SCRIPTENGINE_METHOD_HPP

#include "../parser/types.hpp"
#include "./error.hpp"

#include <expected>
#include <vector>

class asIScriptObject;
class asIScriptFunction;

namespace lp::script {
	class Context;

	struct Param {
		int typeId;
	};

	class Method final {
		friend Context;

	private:
		Context& context;
		asIScriptFunction* func;
		asIScriptObject* thisarg;

		Method(Context& context, asIScriptFunction* func, asIScriptObject* thisarg = nullptr);

	public:
		std::expected<lp::LPValue, Error> invoke(lp::LPArray& arguments);
		const char* name() const noexcept;

		std::vector<Param> params() const noexcept;
	};

} // namespace lp::script

#endif