#ifndef CEREAL_TYPES_VARIANT_HPP_
#define CEREAL_TYPES_VARIANT_HPP_

#include <cereal/cereal.hpp>
#include <variant>

namespace cereal
{
	namespace variant_detail
	{
		//! @internal
		template <uint32_t N, typename Archive, typename... VariantTypes>
		std::enable_if_t<( N == sizeof...( VariantTypes ) )> load_variant( Archive& archive, uint32_t target, std::variant<VariantTypes...>& variant )
		{
			throw Exception{ "Error traversing variant during load" };
		}

		//! @internal
		template <uint32_t N, typename Archive, typename... VariantTypes>
		std::enable_if_t<( N < sizeof...( VariantTypes ) )> load_variant( Archive& archive, uint32_t target, std::variant<VariantTypes...>& variant )
		{
			if( N == target )
			{
				std::decay_t<decltype( std::get<N>( variant ) )> value;
				archive( make_nvp( "data", value ) );
				variant = value;
			}
			else
				load_variant<N + 1>( archive, target, variant );
		}
	}

	//! Serialization for std::monostate
	template <typename Archive>
	inline void CEREAL_SERIALIZE_FUNCTION_NAME( Archive& archive, const std::monostate& monostate )
	{
		// this space is intentionally left blank
	}

	//! Saving for std::variant
	template <typename Archive, typename... VariantTypes>
	inline void CEREAL_SAVE_FUNCTION_NAME( Archive& archive, const std::variant<VariantTypes...>& variant )
	{
		int32_t index = variant.index();

		archive( make_nvp( "index", index ) );

		std::visit( [&archive]( const auto& data )
		{
			archive( make_nvp( "data", data ) );
		}
		, variant );
	}

	//! Loading for std::variant
	template <typename Archive, typename... VariantTypes>
	inline void CEREAL_LOAD_FUNCTION_NAME( Archive& archive, std::variant<VariantTypes...>& variant )
	{
		int32_t index;

		archive( make_nvp( "index", index ) );

		if( index >= std::variant_size_v<std::decay_t<decltype( variant )>> )
			throw Exception( "Invalid 'index' selector when deserializing std::variant" );

		variant_detail::load_variant<0>( archive, index, variant );
	}
}	// namespace cereal

#endif	 // CEREAL_TYPES_VARIANT_HPP_
