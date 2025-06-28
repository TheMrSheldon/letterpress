#ifndef LETTERPRESS_SCRIPTENGINE_OBJECT_HPP
#define LETTERPRESS_SCRIPTENGINE_OBJECT_HPP

class asITypeInfo;
class asIScriptObject;

namespace lp::script {
	class Object {
	private:
		asITypeInfo* typeinfo;
		asIScriptObject* obj;

		/**
		 * @brief Invalidates (/nulls) the instance without destroying it.
		 */
		void invalidate() noexcept;
		/**
		 * @brief Destroys and invalidates the instance.
		 */
		void destroy() noexcept;

	public:
		Object() noexcept;
		Object(asITypeInfo* typeinfo, asIScriptObject* obj) noexcept;
		Object(const Object& other) noexcept;
		Object(Object&& other) noexcept;
		~Object();

		Object& operator=(const Object& other) noexcept;
		Object& operator=(Object&& other) noexcept;

		bool isScriptObject() const noexcept;

		constexpr void* data() const noexcept { return obj; }
	};
} // namespace lp::script

#endif