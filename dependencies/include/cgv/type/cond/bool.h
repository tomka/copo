#pragma once

namespace cgv {
	namespace type {
		namespace cond {
			/// type representing true
			struct true_type  { static const bool value = true; };
			/// type representing false
			struct false_type { static const bool value = false; };
			/// bool_type maps a boolean expression to the boolean types true_type or false_type
			template <bool flag> struct bool_type       : public false_type {};
			template <>          struct bool_type<true> : public true_type  {};
		}
	}
}
