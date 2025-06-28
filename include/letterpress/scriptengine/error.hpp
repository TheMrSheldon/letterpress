#ifndef LETTERPRESS_SCRIPTENGINE_ERROR_HPP
#define LETTERPRESS_SCRIPTENGINE_ERROR_HPP

namespace lp::script {
	enum class Error { OK, GeneralError, NotFound, TypeNotFound, FunctionNotFound };
} // namespace lp::script

#endif