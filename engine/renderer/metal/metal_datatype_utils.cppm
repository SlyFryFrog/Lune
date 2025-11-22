module;
#include <Metal/Metal.hpp>
#include <string_view>
export module lune:metal_datatype_utils;

namespace lune::metal
{
	export constexpr std::string_view to_string(const MTL::DataType type)
	{
		switch (type)
		{
		case MTL::DataTypeNone:
			return "DataTypeNone";
		case MTL::DataTypeStruct:
			return "DataTypeStruct";
		case MTL::DataTypeArray:
			return "DataTypeArray";

		case MTL::DataTypeFloat:
			return "DataTypeFloat";
		case MTL::DataTypeFloat2:
			return "DataTypeFloat2";
		case MTL::DataTypeFloat3:
			return "DataTypeFloat3";
		case MTL::DataTypeFloat4:
			return "DataTypeFloat4";
		case MTL::DataTypeFloat2x2:
			return "DataTypeFloat2x2";
		case MTL::DataTypeFloat2x3:
			return "DataTypeFloat2x3";
		case MTL::DataTypeFloat2x4:
			return "DataTypeFloat2x4";
		case MTL::DataTypeFloat3x2:
			return "DataTypeFloat3x2";
		case MTL::DataTypeFloat3x3:
			return "DataTypeFloat3x3";
		case MTL::DataTypeFloat3x4:
			return "DataTypeFloat3x4";
		case MTL::DataTypeFloat4x2:
			return "DataTypeFloat4x2";
		case MTL::DataTypeFloat4x3:
			return "DataTypeFloat4x3";
		case MTL::DataTypeFloat4x4:
			return "DataTypeFloat4x4";

		case MTL::DataTypeHalf:
			return "DataTypeHalf";
		case MTL::DataTypeHalf2:
			return "DataTypeHalf2";
		case MTL::DataTypeHalf3:
			return "DataTypeHalf3";
		case MTL::DataTypeHalf4:
			return "DataTypeHalf4";
		case MTL::DataTypeHalf2x2:
			return "DataTypeHalf2x2";
		case MTL::DataTypeHalf2x3:
			return "DataTypeHalf2x3";
		case MTL::DataTypeHalf2x4:
			return "DataTypeHalf2x4";
		case MTL::DataTypeHalf3x2:
			return "DataTypeHalf3x2";
		case MTL::DataTypeHalf3x3:
			return "DataTypeHalf3x3";
		case MTL::DataTypeHalf3x4:
			return "DataTypeHalf3x4";
		case MTL::DataTypeHalf4x2:
			return "DataTypeHalf4x2";
		case MTL::DataTypeHalf4x3:
			return "DataTypeHalf4x3";
		case MTL::DataTypeHalf4x4:
			return "DataTypeHalf4x4";

		case MTL::DataTypeInt:
			return "DataTypeInt";
		case MTL::DataTypeInt2:
			return "DataTypeInt2";
		case MTL::DataTypeInt3:
			return "DataTypeInt3";
		case MTL::DataTypeInt4:
			return "DataTypeInt4";

		case MTL::DataTypeUInt:
			return "DataTypeUInt";
		case MTL::DataTypeUInt2:
			return "DataTypeUInt2";
		case MTL::DataTypeUInt3:
			return "DataTypeUInt3";
		case MTL::DataTypeUInt4:
			return "DataTypeUInt4";

		case MTL::DataTypeShort:
			return "DataTypeShort";
		case MTL::DataTypeShort2:
			return "DataTypeShort2";
		case MTL::DataTypeShort3:
			return "DataTypeShort3";
		case MTL::DataTypeShort4:
			return "DataTypeShort4";

		case MTL::DataTypeUShort:
			return "DataTypeUShort";
		case MTL::DataTypeUShort2:
			return "DataTypeUShort2";
		case MTL::DataTypeUShort3:
			return "DataTypeUShort3";
		case MTL::DataTypeUShort4:
			return "DataTypeUShort4";

		case MTL::DataTypeChar:
			return "DataTypeChar";
		case MTL::DataTypeChar2:
			return "DataTypeChar2";
		case MTL::DataTypeChar3:
			return "DataTypeChar3";
		case MTL::DataTypeChar4:
			return "DataTypeChar4";

		case MTL::DataTypeUChar:
			return "DataTypeUChar";
		case MTL::DataTypeUChar2:
			return "DataTypeUChar2";
		case MTL::DataTypeUChar3:
			return "DataTypeUChar3";
		case MTL::DataTypeUChar4:
			return "DataTypeUChar4";

		case MTL::DataTypeBool:
			return "DataTypeBool";
		case MTL::DataTypeBool2:
			return "DataTypeBool2";
		case MTL::DataTypeBool3:
			return "DataTypeBool3";
		case MTL::DataTypeBool4:
			return "DataTypeBool4";

		case MTL::DataTypeTexture:
			return "DataTypeTexture";
		case MTL::DataTypeSampler:
			return "DataTypeSampler";
		case MTL::DataTypePointer:
			return "DataTypePointer";

		case MTL::DataTypeR8Unorm:
			return "DataTypeR8Unorm";
		case MTL::DataTypeR8Snorm:
			return "DataTypeR8Snorm";
		case MTL::DataTypeR16Unorm:
			return "DataTypeR16Unorm";
		case MTL::DataTypeR16Snorm:
			return "DataTypeR16Snorm";

		case MTL::DataTypeRG8Unorm:
			return "DataTypeRG8Unorm";
		case MTL::DataTypeRG8Snorm:
			return "DataTypeRG8Snorm";
		case MTL::DataTypeRG16Unorm:
			return "DataTypeRG16Unorm";
		case MTL::DataTypeRG16Snorm:
			return "DataTypeRG16Snorm";

		case MTL::DataTypeRGBA8Unorm:
			return "DataTypeRGBA8Unorm";
		case MTL::DataTypeRGBA8Unorm_sRGB:
			return "DataTypeRGBA8Unorm_sRGB";
		case MTL::DataTypeRGBA8Snorm:
			return "DataTypeRGBA8Snorm";

		case MTL::DataTypeRGBA16Unorm:
			return "DataTypeRGBA16Unorm";
		case MTL::DataTypeRGBA16Snorm:
			return "DataTypeRGBA16Snorm";

		case MTL::DataTypeRGB10A2Unorm:
			return "DataTypeRGB10A2Unorm";
		case MTL::DataTypeRG11B10Float:
			return "DataTypeRG11B10Float";
		case MTL::DataTypeRGB9E5Float:
			return "DataTypeRGB9E5Float";

		case MTL::DataTypeRenderPipeline:
			return "DataTypeRenderPipeline";
		case MTL::DataTypeComputePipeline:
			return "DataTypeComputePipeline";
		case MTL::DataTypeIndirectCommandBuffer:
			return "DataTypeIndirectCommandBuffer";

		case MTL::DataTypeLong:
			return "DataTypeLong";
		case MTL::DataTypeLong2:
			return "DataTypeLong2";
		case MTL::DataTypeLong3:
			return "DataTypeLong3";
		case MTL::DataTypeLong4:
			return "DataTypeLong4";

		case MTL::DataTypeULong:
			return "DataTypeULong";
		case MTL::DataTypeULong2:
			return "DataTypeULong2";
		case MTL::DataTypeULong3:
			return "DataTypeULong3";
		case MTL::DataTypeULong4:
			return "DataTypeULong4";

		case MTL::DataTypeVisibleFunctionTable:
			return "DataTypeVisibleFunctionTable";
		case MTL::DataTypeIntersectionFunctionTable:
			return "DataTypeIntersectionFunctionTable";
		case MTL::DataTypePrimitiveAccelerationStructure:
			return "DataTypePrimitiveAccelerationStructure";
		case MTL::DataTypeInstanceAccelerationStructure:
			return "DataTypeInstanceAccelerationStructure";

		case MTL::DataTypeBFloat:
			return "DataTypeBFloat";
		case MTL::DataTypeBFloat2:
			return "DataTypeBFloat2";
		case MTL::DataTypeBFloat3:
			return "DataTypeBFloat3";
		case MTL::DataTypeBFloat4:
			return "DataTypeBFloat4";

		case MTL::DataTypeDepthStencilState:
			return "DataTypeDepthStencilState";
		case MTL::DataTypeTensor:
			return "DataTypeTensor";

		default:
			return "UnknownDataType";
		}
	}


	export constexpr std::string_view to_string(const MTL::IndexType type)
	{
		switch (type)
		{
		case MTL::IndexTypeUInt16:
			return "IndexTypeUInt16";
		case MTL::IndexTypeUInt32:
			return "IndexTypeUInt32";
		default:
			return "UnknownIndexType";
		}
	}


	export constexpr std::string_view to_string(const MTL::BindingType type)
	{
		switch (type)
		{
		case MTL::BindingTypeBuffer:
			return "BindingTypeBuffer";
		case MTL::BindingTypeThreadgroupMemory:
			return "BindingTypeThreadgroupMemory";
		case MTL::BindingTypeTexture:
			return "BindingTypeTexture";
		case MTL::BindingTypeSampler:
			return "BindingTypeSampler";
		case MTL::BindingTypeImageblockData:
			return "BindingTypeImageblockData";
		case MTL::BindingTypeImageblock:
			return "BindingTypeImageblock";
		case MTL::BindingTypeVisibleFunctionTable:
			return "BindingTypeVisibleFunctionTable";
		case MTL::BindingTypePrimitiveAccelerationStructure:
			return "BindingTypePrimitiveAccelerationStructure";
		case MTL::BindingTypeInstanceAccelerationStructure:
			return "BindingTypeInstanceAccelerationStructure";
		case MTL::BindingTypeIntersectionFunctionTable:
			return "BindingTypeIntersectionFunctionTable";
		case MTL::BindingTypeObjectPayload:
			return "BindingTypeObjectPayload";
		case MTL::BindingTypeTensor:
			return "BindingTypeTensor";
		default:
			return "UnknownBindingType";
		}
	}


	export constexpr std::string_view to_string(const MTL::ArgumentType type)
	{
		switch (type)
		{
		case MTL::ArgumentTypeBuffer:
			return "ArgumentTypeBuffer";
		case MTL::ArgumentTypeThreadgroupMemory:
			return "ArgumentTypeThreadgroupMemory";
		case MTL::ArgumentTypeTexture:
			return "ArgumentTypeTexture";
		case MTL::ArgumentTypeSampler:
			return "ArgumentTypeSampler";
		case MTL::ArgumentTypeImageblockData:
			return "ArgumentTypeImageblockData";
		case MTL::ArgumentTypeImageblock:
			return "ArgumentTypeImageblock";
		case MTL::ArgumentTypeVisibleFunctionTable:
			return "ArgumentTypeVisibleFunctionTable";
		case MTL::ArgumentTypePrimitiveAccelerationStructure:
			return "ArgumentTypePrimitiveAccelerationStructure";
		case MTL::ArgumentTypeInstanceAccelerationStructure:
			return "ArgumentTypeInstanceAccelerationStructure";
		case MTL::ArgumentTypeIntersectionFunctionTable:
			return "ArgumentTypeIntersectionFunctionTable";
		default:
			return "UnknownArgumentType";
		}
	}


	export constexpr std::string_view to_string(const MTL::BindingAccess access)
	{
		switch (access)
		{
		case MTL::BindingAccessReadOnly:
			return "BindingAccessReadOnly";
		case MTL::BindingAccessReadWrite:
			return "BindingAccessReadWrite";
		case MTL::BindingAccessWriteOnly:
			return "BindingAccessWriteOnly";
		default:
			return "UnknownBindingAccess";
		}
	}


	export enum PixelFormat
	{
		Undefined = 0,

		R8_UNorm,
		RG8_UNorm,
		RGBA8_UNorm,
		RGBA8_sRGB,

		BGRA8_UNorm,
		BGRA8_sRGB,

		R16_Float,
		RG16_Float,
		RGBA16_Float,

		R32_Float,
		RG32_Float,
		RGBA32_Float,
	};


	MTL::PixelFormat toMetalPixelFormat(const PixelFormat pixelFormat)
	{
		using PF = PixelFormat;

		switch (pixelFormat)
		{
		case PF::R8_UNorm:
			return MTL::PixelFormatR8Unorm;
		case PF::RG8_UNorm:
			return MTL::PixelFormatRG8Unorm;
		case PF::RGBA8_UNorm:
			return MTL::PixelFormatRGBA8Unorm;
		case PF::RGBA8_sRGB:
			return MTL::PixelFormatRGBA8Unorm_sRGB;

		case PF::BGRA8_UNorm:
			return MTL::PixelFormatBGRA8Unorm;
		case PF::BGRA8_sRGB:
			return MTL::PixelFormatBGRA8Unorm_sRGB;

		case PF::R16_Float:
			return MTL::PixelFormatR16Float;
		case PF::RG16_Float:
			return MTL::PixelFormatRG16Float;
		case PF::RGBA16_Float:
			return MTL::PixelFormatRGBA16Float;

		case PF::R32_Float:
			return MTL::PixelFormatR32Float;
		case PF::RG32_Float:
			return MTL::PixelFormatRG32Float;
		case PF::RGBA32_Float:
			return MTL::PixelFormatRGBA32Float;

		default:
			return MTL::PixelFormatInvalid;
		}
	}
}
