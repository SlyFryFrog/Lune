module;
#include <Metal/Metal.hpp>
#include <string_view>
#include <array>
export module lune:metal_datatype_utils;

namespace lune::metal
{
	export constexpr std::string_view to_string(const MTL::DataType type)
	{
		constexpr std::array<std::pair<MTL::DataType, std::string_view>, 97> mapping{{
			{MTL::DataTypeNone, "DataTypeNone"},
			{MTL::DataTypeStruct, "DataTypeStruct"},
			{MTL::DataTypeArray, "DataTypeArray"},
			{MTL::DataTypeFloat, "DataTypeFloat"},
			{MTL::DataTypeFloat2, "DataTypeFloat2"},
			{MTL::DataTypeFloat3, "DataTypeFloat3"},
			{MTL::DataTypeFloat4, "DataTypeFloat4"},
			{MTL::DataTypeFloat2x2, "DataTypeFloat2x2"},
			{MTL::DataTypeFloat2x3, "DataTypeFloat2x3"},
			{MTL::DataTypeFloat2x4, "DataTypeFloat2x4"},
			{MTL::DataTypeFloat3x2, "DataTypeFloat3x2"},
			{MTL::DataTypeFloat3x3, "DataTypeFloat3x3"},
			{MTL::DataTypeFloat3x4, "DataTypeFloat3x4"},
			{MTL::DataTypeFloat4x2, "DataTypeFloat4x2"},
			{MTL::DataTypeFloat4x3, "DataTypeFloat4x3"},
			{MTL::DataTypeFloat4x4, "DataTypeFloat4x4"},
			{MTL::DataTypeHalf, "DataTypeHalf"},
			{MTL::DataTypeHalf2, "DataTypeHalf2"},
			{MTL::DataTypeHalf3, "DataTypeHalf3"},
			{MTL::DataTypeHalf4, "DataTypeHalf4"},
			{MTL::DataTypeHalf2x2, "DataTypeHalf2x2"},
			{MTL::DataTypeHalf2x3, "DataTypeHalf2x3"},
			{MTL::DataTypeHalf2x4, "DataTypeHalf2x4"},
			{MTL::DataTypeHalf3x2, "DataTypeHalf3x2"},
			{MTL::DataTypeHalf3x3, "DataTypeHalf3x3"},
			{MTL::DataTypeHalf3x4, "DataTypeHalf3x4"},
			{MTL::DataTypeHalf4x2, "DataTypeHalf4x2"},
			{MTL::DataTypeHalf4x3, "DataTypeHalf4x3"},
			{MTL::DataTypeHalf4x4, "DataTypeHalf4x4"},
			{MTL::DataTypeInt, "DataTypeInt"},
			{MTL::DataTypeInt2, "DataTypeInt2"},
			{MTL::DataTypeInt3, "DataTypeInt3"},
			{MTL::DataTypeInt4, "DataTypeInt4"},
			{MTL::DataTypeUInt, "DataTypeUInt"},
			{MTL::DataTypeUInt2, "DataTypeUInt2"},
			{MTL::DataTypeUInt3, "DataTypeUInt3"},
			{MTL::DataTypeUInt4, "DataTypeUInt4"},
			{MTL::DataTypeShort, "DataTypeShort"},
			{MTL::DataTypeShort2, "DataTypeShort2"},
			{MTL::DataTypeShort3, "DataTypeShort3"},
			{MTL::DataTypeShort4, "DataTypeShort4"},
			{MTL::DataTypeUShort, "DataTypeUShort"},
			{MTL::DataTypeUShort2, "DataTypeUShort2"},
			{MTL::DataTypeUShort3, "DataTypeUShort3"},
			{MTL::DataTypeUShort4, "DataTypeUShort4"},
			{MTL::DataTypeChar, "DataTypeChar"},
			{MTL::DataTypeChar2, "DataTypeChar2"},
			{MTL::DataTypeChar3, "DataTypeChar3"},
			{MTL::DataTypeChar4, "DataTypeChar4"},
			{MTL::DataTypeUChar, "DataTypeUChar"},
			{MTL::DataTypeUChar2, "DataTypeUChar2"},
			{MTL::DataTypeUChar3, "DataTypeUChar3"},
			{MTL::DataTypeUChar4, "DataTypeUChar4"},
			{MTL::DataTypeBool, "DataTypeBool"},
			{MTL::DataTypeBool2, "DataTypeBool2"},
			{MTL::DataTypeBool3, "DataTypeBool3"},
			{MTL::DataTypeBool4, "DataTypeBool4"},
			{MTL::DataTypeTexture, "DataTypeTexture"},
			{MTL::DataTypeSampler, "DataTypeSampler"},
			{MTL::DataTypePointer, "DataTypePointer"},
			{MTL::DataTypeR8Unorm, "DataTypeR8Unorm"},
			{MTL::DataTypeR8Snorm, "DataTypeR8Snorm"},
			{MTL::DataTypeR16Unorm, "DataTypeR16Unorm"},
			{MTL::DataTypeR16Snorm, "DataTypeR16Snorm"},
			{MTL::DataTypeRG8Unorm, "DataTypeRG8Unorm"},
			{MTL::DataTypeRG8Snorm, "DataTypeRG8Snorm"},
			{MTL::DataTypeRG16Unorm, "DataTypeRG16Unorm"},
			{MTL::DataTypeRG16Snorm, "DataTypeRG16Snorm"},
			{MTL::DataTypeRGBA8Unorm, "DataTypeRGBA8Unorm"},
			{MTL::DataTypeRGBA8Unorm_sRGB, "DataTypeRGBA8Unorm_sRGB"},
			{MTL::DataTypeRGBA8Snorm, "DataTypeRGBA8Snorm"},
			{MTL::DataTypeRGBA16Unorm, "DataTypeRGBA16Unorm"},
			{MTL::DataTypeRGBA16Snorm, "DataTypeRGBA16Snorm"},
			{MTL::DataTypeRGB10A2Unorm, "DataTypeRGB10A2Unorm"},
			{MTL::DataTypeRG11B10Float, "DataTypeRG11B10Float"},
			{MTL::DataTypeRGB9E5Float, "DataTypeRGB9E5Float"},
			{MTL::DataTypeRenderPipeline, "DataTypeRenderPipeline"},
			{MTL::DataTypeComputePipeline, "DataTypeComputePipeline"},
			{MTL::DataTypeIndirectCommandBuffer, "DataTypeIndirectCommandBuffer"},
			{MTL::DataTypeLong, "DataTypeLong"},
			{MTL::DataTypeLong2, "DataTypeLong2"},
			{MTL::DataTypeLong3, "DataTypeLong3"},
			{MTL::DataTypeLong4, "DataTypeLong4"},
			{MTL::DataTypeULong, "DataTypeULong"},
			{MTL::DataTypeULong2, "DataTypeULong2"},
			{MTL::DataTypeULong3, "DataTypeULong3"},
			{MTL::DataTypeULong4, "DataTypeULong4"},
			{MTL::DataTypeVisibleFunctionTable, "DataTypeVisibleFunctionTable"},
			{MTL::DataTypeIntersectionFunctionTable, "DataTypeIntersectionFunctionTable"},
			{MTL::DataTypePrimitiveAccelerationStructure, "DataTypePrimitiveAccelerationStructure"},
			{MTL::DataTypeInstanceAccelerationStructure, "DataTypeInstanceAccelerationStructure"},
			{MTL::DataTypeBFloat, "DataTypeBFloat"},
			{MTL::DataTypeBFloat2, "DataTypeBFloat2"},
			{MTL::DataTypeBFloat3, "DataTypeBFloat3"},
			{MTL::DataTypeBFloat4, "DataTypeBFloat4"},
			{MTL::DataTypeDepthStencilState, "DataTypeDepthStencilState"},
			{MTL::DataTypeTensor, "DataTypeTensor"}
		}};

		for (auto [dt, name] : mapping)
			if (dt == type)
				return name;
		return "UnknownDataType";
	}


	export constexpr std::string_view to_string(const MTL::IndexType type)
	{
		constexpr std::array<std::pair<MTL::IndexType, std::string_view>, 2> mapping{{
			{MTL::IndexTypeUInt16, "IndexTypeUInt16"},
			{MTL::IndexTypeUInt32, "IndexTypeUInt32"},
		}};

		for (auto [val, name] : mapping)
			if (val == type)
				return name;

		return "UnknownIndexType";
	}


	export constexpr std::string_view to_string(const MTL::BindingType type)
	{
		// clang-format off
		constexpr std::array<std::pair<MTL::BindingType, std::string_view>, 12> mapping{{
			{MTL::BindingTypeBuffer, "BindingTypeBuffer"},
			{MTL::BindingTypeThreadgroupMemory, "BindingTypeThreadgroupMemory"},
			{MTL::BindingTypeTexture, "BindingTypeTexture"},
			{MTL::BindingTypeSampler, "BindingTypeSampler"},
			{MTL::BindingTypeImageblockData, "BindingTypeImageblockData"},
			{MTL::BindingTypeImageblock, "BindingTypeImageblock"},
			{MTL::BindingTypeVisibleFunctionTable, "BindingTypeVisibleFunctionTable"},
			{MTL::BindingTypePrimitiveAccelerationStructure, "BindingTypePrimitiveAccelerationStructure"},
			{MTL::BindingTypeInstanceAccelerationStructure, "BindingTypeInstanceAccelerationStructure"},
			{MTL::BindingTypeIntersectionFunctionTable, "BindingTypeIntersectionFunctionTable"},
			{MTL::BindingTypeObjectPayload, "BindingTypeObjectPayload"},
			{MTL::BindingTypeTensor, "BindingTypeTensor"},
		}};
		// clang-format on

		for (auto [val, name] : mapping)
			if (val == type)
				return name;

		return "UnknownBindingType";
	}


	export constexpr std::string_view to_string(const MTL::ArgumentType type)
	{
		// clang-format off
		constexpr std::array<std::pair<MTL::ArgumentType, std::string_view>, 10> mapping{{
			{MTL::ArgumentTypeBuffer, "ArgumentTypeBuffer"},
			{MTL::ArgumentTypeThreadgroupMemory, "ArgumentTypeThreadgroupMemory"},
			{MTL::ArgumentTypeTexture, "ArgumentTypeTexture"},
			{MTL::ArgumentTypeSampler, "ArgumentTypeSampler"},
			{MTL::ArgumentTypeImageblockData, "ArgumentTypeImageblockData"},
			{MTL::ArgumentTypeImageblock, "ArgumentTypeImageblock"},
			{MTL::ArgumentTypeVisibleFunctionTable, "ArgumentTypeVisibleFunctionTable"},
			{MTL::ArgumentTypePrimitiveAccelerationStructure, "ArgumentTypePrimitiveAccelerationStructure"},
			{MTL::ArgumentTypeInstanceAccelerationStructure, "ArgumentTypeInstanceAccelerationStructure"},
			{MTL::ArgumentTypeIntersectionFunctionTable, "ArgumentTypeIntersectionFunctionTable"},
		}};
		// clang-format on

		for (auto [val, name] : mapping)
			if (val == type)
				return name;

		return "UnknownArgumentType";
	}


	export constexpr std::string_view to_string(const MTL::BindingAccess access)
	{
		constexpr std::array<std::pair<MTL::BindingAccess, std::string_view>, 3> mapping{{
			{MTL::BindingAccessReadOnly, "BindingAccessReadOnly"},
			{MTL::BindingAccessReadWrite, "BindingAccessReadWrite"},
			{MTL::BindingAccessWriteOnly, "BindingAccessWriteOnly"}
		}};

		for (auto [val, name] : mapping)
			if (val == access)
				return name;

		return "UnknownBindingAccess";
	}
}
