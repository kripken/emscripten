#ifndef WEBGPU_CPP_H_
#define WEBGPU_CPP_H_

#include "webgpu/webgpu.h"

#include <type_traits>

namespace wgpu {

    template <typename T>
    struct IsDawnBitmask {
        static constexpr bool enable = false;
    };

    template <typename T, typename Enable = void>
    struct LowerBitmask {
        static constexpr bool enable = false;
    };

    template <typename T>
    struct LowerBitmask<T, typename std::enable_if<IsDawnBitmask<T>::enable>::type> {
        static constexpr bool enable = true;
        using type = T;
        constexpr static T Lower(T t) {
            return t;
        }
    };

    template <typename T>
    struct BoolConvertible {
        using Integral = typename std::underlying_type<T>::type;

        constexpr BoolConvertible(Integral value) : value(value) {
        }
        constexpr operator bool() const {
            return value != 0;
        }
        constexpr operator T() const {
            return static_cast<T>(value);
        }

        Integral value;
    };

    template <typename T>
    struct LowerBitmask<BoolConvertible<T>> {
        static constexpr bool enable = true;
        using type = T;
        static constexpr type Lower(BoolConvertible<T> t) {
            return t;
        }
    };

    template <typename T1,
              typename T2,
              typename = typename std::enable_if<LowerBitmask<T1>::enable &&
                                                 LowerBitmask<T2>::enable>::type>
    constexpr BoolConvertible<typename LowerBitmask<T1>::type> operator|(T1 left, T2 right) {
        using T = typename LowerBitmask<T1>::type;
        using Integral = typename std::underlying_type<T>::type;
        return static_cast<Integral>(LowerBitmask<T1>::Lower(left)) |
               static_cast<Integral>(LowerBitmask<T2>::Lower(right));
    }

    template <typename T1,
              typename T2,
              typename = typename std::enable_if<LowerBitmask<T1>::enable &&
                                                 LowerBitmask<T2>::enable>::type>
    constexpr BoolConvertible<typename LowerBitmask<T1>::type> operator&(T1 left, T2 right) {
        using T = typename LowerBitmask<T1>::type;
        using Integral = typename std::underlying_type<T>::type;
        return static_cast<Integral>(LowerBitmask<T1>::Lower(left)) &
               static_cast<Integral>(LowerBitmask<T2>::Lower(right));
    }

    template <typename T1,
              typename T2,
              typename = typename std::enable_if<LowerBitmask<T1>::enable &&
                                                 LowerBitmask<T2>::enable>::type>
    constexpr BoolConvertible<typename LowerBitmask<T1>::type> operator^(T1 left, T2 right) {
        using T = typename LowerBitmask<T1>::type;
        using Integral = typename std::underlying_type<T>::type;
        return static_cast<Integral>(LowerBitmask<T1>::Lower(left)) ^
               static_cast<Integral>(LowerBitmask<T2>::Lower(right));
    }

    template <typename T1>
    constexpr BoolConvertible<typename LowerBitmask<T1>::type> operator~(T1 t) {
        using T = typename LowerBitmask<T1>::type;
        using Integral = typename std::underlying_type<T>::type;
        return ~static_cast<Integral>(LowerBitmask<T1>::Lower(t));
    }

    template <typename T,
              typename T2,
              typename = typename std::enable_if<IsDawnBitmask<T>::enable &&
                                                 LowerBitmask<T2>::enable>::type>
    constexpr T& operator&=(T& l, T2 right) {
        T r = LowerBitmask<T2>::Lower(right);
        l = l & r;
        return l;
    }

    template <typename T,
              typename T2,
              typename = typename std::enable_if<IsDawnBitmask<T>::enable &&
                                                 LowerBitmask<T2>::enable>::type>
    constexpr T& operator|=(T& l, T2 right) {
        T r = LowerBitmask<T2>::Lower(right);
        l = l | r;
        return l;
    }

    template <typename T,
              typename T2,
              typename = typename std::enable_if<IsDawnBitmask<T>::enable &&
                                                 LowerBitmask<T2>::enable>::type>
    constexpr T& operator^=(T& l, T2 right) {
        T r = LowerBitmask<T2>::Lower(right);
        l = l ^ r;
        return l;
    }

    template <typename T>
    constexpr bool HasZeroOrOneBits(T value) {
        using Integral = typename std::underlying_type<T>::type;
        return (static_cast<Integral>(value) & (static_cast<Integral>(value) - 1)) == 0;
    }

    static constexpr uint64_t kWholeSize = WGPU_WHOLE_SIZE;
    // TODO(crbug.com/520): Remove kStrideUndefined in favor of kCopyStrideUndefined.
    static constexpr uint32_t kStrideUndefined = WGPU_STRIDE_UNDEFINED;
    static constexpr uint32_t kCopyStrideUndefined = WGPU_COPY_STRIDE_UNDEFINED;

    enum class AdapterType : uint32_t {
        DiscreteGPU = 0x00000000,
        IntegratedGPU = 0x00000001,
        CPU = 0x00000002,
        Unknown = 0x00000003,
    };

    enum class AddressMode : uint32_t {
        Repeat = 0x00000000,
        MirrorRepeat = 0x00000001,
        ClampToEdge = 0x00000002,
    };

    enum class BackendType : uint32_t {
        Null = 0x00000000,
        D3D11 = 0x00000001,
        D3D12 = 0x00000002,
        Metal = 0x00000003,
        Vulkan = 0x00000004,
        OpenGL = 0x00000005,
        OpenGLES = 0x00000006,
    };

    enum class BindingType : uint32_t {
        Undefined = 0x00000000,
        UniformBuffer = 0x00000001,
        StorageBuffer = 0x00000002,
        ReadonlyStorageBuffer = 0x00000003,
        Sampler = 0x00000004,
        ComparisonSampler = 0x00000005,
        SampledTexture = 0x00000006,
        MultisampledTexture = 0x00000007,
        ReadonlyStorageTexture = 0x00000008,
        WriteonlyStorageTexture = 0x00000009,
    };

    enum class BlendFactor : uint32_t {
        Zero = 0x00000000,
        One = 0x00000001,
        Src = 0x00000002,
        OneMinusSrc = 0x00000003,
        SrcAlpha = 0x00000004,
        OneMinusSrcAlpha = 0x00000005,
        Dst = 0x00000006,
        OneMinusDst = 0x00000007,
        DstAlpha = 0x00000008,
        OneMinusDstAlpha = 0x00000009,
        SrcAlphaSaturated = 0x0000000A,
        Constant = 0x0000000B,
        OneMinusConstant = 0x0000000C,
        SrcColor = 0x00000066,
        OneMinusSrcColor = 0x00000067,
        DstColor = 0x0000006A,
        OneMinusDstColor = 0x0000006B,
        BlendColor = 0x0000006F,
        OneMinusBlendColor = 0x00000070,
    };

    enum class BlendOperation : uint32_t {
        Add = 0x00000000,
        Subtract = 0x00000001,
        ReverseSubtract = 0x00000002,
        Min = 0x00000003,
        Max = 0x00000004,
    };

    enum class BufferBindingType : uint32_t {
        Undefined = 0x00000000,
        Uniform = 0x00000001,
        Storage = 0x00000002,
        ReadOnlyStorage = 0x00000003,
    };

    enum class BufferMapAsyncStatus : uint32_t {
        Success = 0x00000000,
        Error = 0x00000001,
        Unknown = 0x00000002,
        DeviceLost = 0x00000003,
        DestroyedBeforeCallback = 0x00000004,
        UnmappedBeforeCallback = 0x00000005,
    };

    enum class CompareFunction : uint32_t {
        Undefined = 0x00000000,
        Never = 0x00000001,
        Less = 0x00000002,
        LessEqual = 0x00000003,
        Greater = 0x00000004,
        GreaterEqual = 0x00000005,
        Equal = 0x00000006,
        NotEqual = 0x00000007,
        Always = 0x00000008,
    };

    enum class CreatePipelineAsyncStatus : uint32_t {
        Success = 0x00000000,
        Error = 0x00000001,
        DeviceLost = 0x00000002,
        DeviceDestroyed = 0x00000003,
        Unknown = 0x00000004,
    };

    enum class CullMode : uint32_t {
        None = 0x00000000,
        Front = 0x00000001,
        Back = 0x00000002,
    };

    enum class ErrorFilter : uint32_t {
        None = 0x00000000,
        Validation = 0x00000001,
        OutOfMemory = 0x00000002,
    };

    enum class ErrorType : uint32_t {
        NoError = 0x00000000,
        Validation = 0x00000001,
        OutOfMemory = 0x00000002,
        Unknown = 0x00000003,
        DeviceLost = 0x00000004,
    };

    enum class FenceCompletionStatus : uint32_t {
        Success = 0x00000000,
        Error = 0x00000001,
        Unknown = 0x00000002,
        DeviceLost = 0x00000003,
    };

    enum class FilterMode : uint32_t {
        Nearest = 0x00000000,
        Linear = 0x00000001,
    };

    enum class FrontFace : uint32_t {
        CCW = 0x00000000,
        CW = 0x00000001,
    };

    enum class IndexFormat : uint32_t {
        Undefined = 0x00000000,
        Uint16 = 0x00000001,
        Uint32 = 0x00000002,
    };

    enum class InputStepMode : uint32_t {
        Vertex = 0x00000000,
        Instance = 0x00000001,
    };

    enum class LoadOp : uint32_t {
        Clear = 0x00000000,
        Load = 0x00000001,
    };

    enum class PipelineStatisticName : uint32_t {
        VertexShaderInvocations = 0x00000000,
        ClipperInvocations = 0x00000001,
        ClipperPrimitivesOut = 0x00000002,
        FragmentShaderInvocations = 0x00000003,
        ComputeShaderInvocations = 0x00000004,
    };

    enum class PresentMode : uint32_t {
        Immediate = 0x00000000,
        Mailbox = 0x00000001,
        Fifo = 0x00000002,
    };

    enum class PrimitiveTopology : uint32_t {
        PointList = 0x00000000,
        LineList = 0x00000001,
        LineStrip = 0x00000002,
        TriangleList = 0x00000003,
        TriangleStrip = 0x00000004,
    };

    enum class QueryType : uint32_t {
        Occlusion = 0x00000000,
        PipelineStatistics = 0x00000001,
        Timestamp = 0x00000002,
    };

    enum class QueueWorkDoneStatus : uint32_t {
        Success = 0x00000000,
        Error = 0x00000001,
        Unknown = 0x00000002,
        DeviceLost = 0x00000003,
    };

    enum class SType : uint32_t {
        Invalid = 0x00000000,
        SurfaceDescriptorFromMetalLayer = 0x00000001,
        SurfaceDescriptorFromWindowsHWND = 0x00000002,
        SurfaceDescriptorFromXlib = 0x00000003,
        SurfaceDescriptorFromCanvasHTMLSelector = 0x00000004,
        ShaderModuleSPIRVDescriptor = 0x00000005,
        ShaderModuleWGSLDescriptor = 0x00000006,
        PrimitiveDepthClampingState = 0x00000007,
    };

    enum class SamplerBindingType : uint32_t {
        Undefined = 0x00000000,
        Filtering = 0x00000001,
        NonFiltering = 0x00000002,
        Comparison = 0x00000003,
    };

    enum class StencilOperation : uint32_t {
        Keep = 0x00000000,
        Zero = 0x00000001,
        Replace = 0x00000002,
        Invert = 0x00000003,
        IncrementClamp = 0x00000004,
        DecrementClamp = 0x00000005,
        IncrementWrap = 0x00000006,
        DecrementWrap = 0x00000007,
    };

    enum class StorageTextureAccess : uint32_t {
        Undefined = 0x00000000,
        ReadOnly = 0x00000001,
        WriteOnly = 0x00000002,
    };

    enum class StoreOp : uint32_t {
        Store = 0x00000000,
        Clear = 0x00000001,
    };

    enum class TextureAspect : uint32_t {
        All = 0x00000000,
        StencilOnly = 0x00000001,
        DepthOnly = 0x00000002,
        Plane0Only = 0x00000003,
        Plane1Only = 0x00000004,
    };

    enum class TextureComponentType : uint32_t {
        Float = 0x00000000,
        Sint = 0x00000001,
        Uint = 0x00000002,
        DepthComparison = 0x00000003,
    };

    enum class TextureDimension : uint32_t {
        e1D = 0x00000000,
        e2D = 0x00000001,
        e3D = 0x00000002,
    };

    enum class TextureFormat : uint32_t {
        Undefined = 0x00000000,
        R8Unorm = 0x00000001,
        R8Snorm = 0x00000002,
        R8Uint = 0x00000003,
        R8Sint = 0x00000004,
        R16Uint = 0x00000005,
        R16Sint = 0x00000006,
        R16Float = 0x00000007,
        RG8Unorm = 0x00000008,
        RG8Snorm = 0x00000009,
        RG8Uint = 0x0000000A,
        RG8Sint = 0x0000000B,
        R32Float = 0x0000000C,
        R32Uint = 0x0000000D,
        R32Sint = 0x0000000E,
        RG16Uint = 0x0000000F,
        RG16Sint = 0x00000010,
        RG16Float = 0x00000011,
        RGBA8Unorm = 0x00000012,
        RGBA8UnormSrgb = 0x00000013,
        RGBA8Snorm = 0x00000014,
        RGBA8Uint = 0x00000015,
        RGBA8Sint = 0x00000016,
        BGRA8Unorm = 0x00000017,
        BGRA8UnormSrgb = 0x00000018,
        RGB10A2Unorm = 0x00000019,
        RG11B10Ufloat = 0x0000001A,
        RGB9E5Ufloat = 0x0000001B,
        RG32Float = 0x0000001C,
        RG32Uint = 0x0000001D,
        RG32Sint = 0x0000001E,
        RGBA16Uint = 0x0000001F,
        RGBA16Sint = 0x00000020,
        RGBA16Float = 0x00000021,
        RGBA32Float = 0x00000022,
        RGBA32Uint = 0x00000023,
        RGBA32Sint = 0x00000024,
        Depth32Float = 0x00000025,
        Depth24Plus = 0x00000026,
        Stencil8 = 0x00000027,
        Depth24PlusStencil8 = 0x00000028,
        BC1RGBAUnorm = 0x00000029,
        BC1RGBAUnormSrgb = 0x0000002A,
        BC2RGBAUnorm = 0x0000002B,
        BC2RGBAUnormSrgb = 0x0000002C,
        BC3RGBAUnorm = 0x0000002D,
        BC3RGBAUnormSrgb = 0x0000002E,
        BC4RUnorm = 0x0000002F,
        BC4RSnorm = 0x00000030,
        BC5RGUnorm = 0x00000031,
        BC5RGSnorm = 0x00000032,
        BC6HRGBUfloat = 0x00000033,
        BC6HRGBFloat = 0x00000034,
        BC7RGBAUnorm = 0x00000035,
        BC7RGBAUnormSrgb = 0x00000036,
        R8BG8Biplanar420Unorm = 0x00000037,
    };

    enum class TextureSampleType : uint32_t {
        Undefined = 0x00000000,
        Float = 0x00000001,
        UnfilterableFloat = 0x00000002,
        Depth = 0x00000003,
        Sint = 0x00000004,
        Uint = 0x00000005,
    };

    enum class TextureViewDimension : uint32_t {
        Undefined = 0x00000000,
        e1D = 0x00000001,
        e2D = 0x00000002,
        e2DArray = 0x00000003,
        Cube = 0x00000004,
        CubeArray = 0x00000005,
        e3D = 0x00000006,
    };

    enum class VertexFormat : uint32_t {
        Undefined = 0x00000000,
        Uint8x2 = 0x00000001,
        Uint8x4 = 0x00000002,
        Sint8x2 = 0x00000003,
        Sint8x4 = 0x00000004,
        Unorm8x2 = 0x00000005,
        Unorm8x4 = 0x00000006,
        Snorm8x2 = 0x00000007,
        Snorm8x4 = 0x00000008,
        Uint16x2 = 0x00000009,
        Uint16x4 = 0x0000000A,
        Sint16x2 = 0x0000000B,
        Sint16x4 = 0x0000000C,
        Unorm16x2 = 0x0000000D,
        Unorm16x4 = 0x0000000E,
        Snorm16x2 = 0x0000000F,
        Snorm16x4 = 0x00000010,
        Float16x2 = 0x00000011,
        Float16x4 = 0x00000012,
        Float32 = 0x00000013,
        Float32x2 = 0x00000014,
        Float32x3 = 0x00000015,
        Float32x4 = 0x00000016,
        Uint32 = 0x00000017,
        Uint32x2 = 0x00000018,
        Uint32x3 = 0x00000019,
        Uint32x4 = 0x0000001A,
        Sint32 = 0x0000001B,
        Sint32x2 = 0x0000001C,
        Sint32x3 = 0x0000001D,
        Sint32x4 = 0x0000001E,
        UChar2 = 0x00000065,
        UChar4 = 0x00000066,
        Char2 = 0x00000067,
        Char4 = 0x00000068,
        UChar2Norm = 0x00000069,
        UChar4Norm = 0x0000006A,
        Char2Norm = 0x0000006B,
        Char4Norm = 0x0000006C,
        UShort2 = 0x0000006D,
        UShort4 = 0x0000006E,
        Short2 = 0x0000006F,
        Short4 = 0x00000070,
        UShort2Norm = 0x00000071,
        UShort4Norm = 0x00000072,
        Short2Norm = 0x00000073,
        Short4Norm = 0x00000074,
        Half2 = 0x00000075,
        Half4 = 0x00000076,
        Float = 0x00000077,
        Float2 = 0x00000078,
        Float3 = 0x00000079,
        Float4 = 0x0000007A,
        UInt = 0x0000007B,
        UInt2 = 0x0000007C,
        UInt3 = 0x0000007D,
        UInt4 = 0x0000007E,
        Int = 0x0000007F,
        Int2 = 0x00000080,
        Int3 = 0x00000081,
        Int4 = 0x00000082,
    };


    enum class BufferUsage : uint32_t {
        None = 0x00000000,
        MapRead = 0x00000001,
        MapWrite = 0x00000002,
        CopySrc = 0x00000004,
        CopyDst = 0x00000008,
        Index = 0x00000010,
        Vertex = 0x00000020,
        Uniform = 0x00000040,
        Storage = 0x00000080,
        Indirect = 0x00000100,
        QueryResolve = 0x00000200,
    };

    enum class ColorWriteMask : uint32_t {
        None = 0x00000000,
        Red = 0x00000001,
        Green = 0x00000002,
        Blue = 0x00000004,
        Alpha = 0x00000008,
        All = 0x0000000F,
    };

    enum class MapMode : uint32_t {
        None = 0x00000000,
        Read = 0x00000001,
        Write = 0x00000002,
    };

    enum class ShaderStage : uint32_t {
        None = 0x00000000,
        Vertex = 0x00000001,
        Fragment = 0x00000002,
        Compute = 0x00000004,
    };

    enum class TextureUsage : uint32_t {
        None = 0x00000000,
        CopySrc = 0x00000001,
        CopyDst = 0x00000002,
        Sampled = 0x00000004,
        Storage = 0x00000008,
        OutputAttachment = 0x00000010,
        RenderAttachment = 0x00000010,
        Present = 0x00000020,
    };


    template<>
    struct IsDawnBitmask<BufferUsage> {
        static constexpr bool enable = true;
    };

    template<>
    struct IsDawnBitmask<ColorWriteMask> {
        static constexpr bool enable = true;
    };

    template<>
    struct IsDawnBitmask<MapMode> {
        static constexpr bool enable = true;
    };

    template<>
    struct IsDawnBitmask<ShaderStage> {
        static constexpr bool enable = true;
    };

    template<>
    struct IsDawnBitmask<TextureUsage> {
        static constexpr bool enable = true;
    };


    using Proc = WGPUProc;
    using BufferMapCallback = WGPUBufferMapCallback;
    using CreateComputePipelineAsyncCallback = WGPUCreateComputePipelineAsyncCallback;
    using CreateRenderPipelineAsyncCallback = WGPUCreateRenderPipelineAsyncCallback;
    using DeviceLostCallback = WGPUDeviceLostCallback;
    using ErrorCallback = WGPUErrorCallback;
    using FenceOnCompletionCallback = WGPUFenceOnCompletionCallback;
    using QueueWorkDoneCallback = WGPUQueueWorkDoneCallback;

    class BindGroup;
    class BindGroupLayout;
    class Buffer;
    class CommandBuffer;
    class CommandEncoder;
    class ComputePassEncoder;
    class ComputePipeline;
    class Device;
    class ExternalTexture;
    class Fence;
    class Instance;
    class PipelineLayout;
    class QuerySet;
    class Queue;
    class RenderBundle;
    class RenderBundleEncoder;
    class RenderPassEncoder;
    class RenderPipeline;
    class Sampler;
    class ShaderModule;
    class Surface;
    class SwapChain;
    class Texture;
    class TextureView;

    struct AdapterProperties;
    struct BindGroupEntry;
    struct BlendComponent;
    struct BufferBindingLayout;
    struct BufferDescriptor;
    struct Color;
    struct CommandBufferDescriptor;
    struct CommandEncoderDescriptor;
    struct ComputePassDescriptor;
    struct CopyTextureForBrowserOptions;
    struct DeviceProperties;
    struct Extent3D;
    struct ExternalTextureDescriptor;
    struct FenceDescriptor;
    struct InstanceDescriptor;
    struct MultisampleState;
    struct Origin3D;
    struct PipelineLayoutDescriptor;
    struct PrimitiveDepthClampingState;
    struct PrimitiveState;
    struct ProgrammableStageDescriptor;
    struct QuerySetDescriptor;
    struct RasterizationStateDescriptor;
    struct RenderBundleDescriptor;
    struct RenderBundleEncoderDescriptor;
    struct RenderPassDepthStencilAttachment;
    struct SamplerBindingLayout;
    struct SamplerDescriptor;
    struct ShaderModuleDescriptor;
    struct ShaderModuleSPIRVDescriptor;
    struct ShaderModuleWGSLDescriptor;
    struct StencilFaceState;
    struct StorageTextureBindingLayout;
    struct SurfaceDescriptor;
    struct SurfaceDescriptorFromCanvasHTMLSelector;
    struct SurfaceDescriptorFromMetalLayer;
    struct SurfaceDescriptorFromWindowsHWND;
    struct SurfaceDescriptorFromXlib;
    struct SwapChainDescriptor;
    struct TextureBindingLayout;
    struct TextureDataLayout;
    struct TextureViewDescriptor;
    struct VertexAttribute;
    struct BindGroupDescriptor;
    struct BindGroupLayoutEntry;
    struct BlendState;
    struct ColorStateDescriptor;
    struct ComputePipelineDescriptor;
    struct DepthStencilState;
    struct DepthStencilStateDescriptor;
    struct ImageCopyBuffer;
    struct ImageCopyTexture;
    struct RenderPassColorAttachment;
    struct TextureDescriptor;
    struct VertexBufferLayout;
    struct BindGroupLayoutDescriptor;
    struct ColorTargetState;
    struct RenderPassDescriptor;
    struct VertexState;
    struct VertexStateDescriptor;
    struct FragmentState;
    struct RenderPipelineDescriptor;
    struct RenderPipelineDescriptor2;

    // BlendDescriptor is deprecated.
    // Use BlendComponent instead.
    using BlendDescriptor = BlendComponent;

    // BufferCopyView is deprecated.
    // Use ImageCopyBuffer instead.
    using BufferCopyView = ImageCopyBuffer;

    // RenderPassColorAttachmentDescriptor is deprecated.
    // Use RenderPassColorAttachment instead.
    using RenderPassColorAttachmentDescriptor = RenderPassColorAttachment;

    // RenderPassDepthStencilAttachmentDescriptor is deprecated.
    // Use RenderPassDepthStencilAttachment instead.
    using RenderPassDepthStencilAttachmentDescriptor = RenderPassDepthStencilAttachment;

    // StencilStateFaceDescriptor is deprecated.
    // Use StencilFaceState instead.
    using StencilStateFaceDescriptor = StencilFaceState;

    // TextureCopyView is deprecated.
    // Use ImageCopyTexture instead.
    using TextureCopyView = ImageCopyTexture;

    // VertexAttributeDescriptor is deprecated.
    // Use VertexAttribute instead.
    using VertexAttributeDescriptor = VertexAttribute;

    // VertexBufferLayoutDescriptor is deprecated.
    // Use VertexBufferLayout instead.
    using VertexBufferLayoutDescriptor = VertexBufferLayout;


    template<typename Derived, typename CType>
    class ObjectBase {
      public:
        ObjectBase() = default;
        ObjectBase(CType handle): mHandle(handle) {
            if (mHandle) Derived::WGPUReference(mHandle);
        }
        ~ObjectBase() {
            if (mHandle) Derived::WGPURelease(mHandle);
        }

        ObjectBase(ObjectBase const& other)
            : ObjectBase(other.Get()) {
        }
        Derived& operator=(ObjectBase const& other) {
            if (&other != this) {
                if (mHandle) Derived::WGPURelease(mHandle);
                mHandle = other.mHandle;
                if (mHandle) Derived::WGPUReference(mHandle);
            }

            return static_cast<Derived&>(*this);
        }

        ObjectBase(ObjectBase&& other) {
            mHandle = other.mHandle;
            other.mHandle = 0;
        }
        Derived& operator=(ObjectBase&& other) {
            if (&other != this) {
                if (mHandle) Derived::WGPURelease(mHandle);
                mHandle = other.mHandle;
                other.mHandle = 0;
            }

            return static_cast<Derived&>(*this);
        }

        ObjectBase(std::nullptr_t) {}
        Derived& operator=(std::nullptr_t) {
            if (mHandle != nullptr) {
                Derived::WGPURelease(mHandle);
                mHandle = nullptr;
            }
            return static_cast<Derived&>(*this);
        }

        bool operator==(std::nullptr_t) const {
            return mHandle == nullptr;
        }
        bool operator!=(std::nullptr_t) const {
            return mHandle != nullptr;
        }

        explicit operator bool() const {
            return mHandle != nullptr;
        }
        CType Get() const {
            return mHandle;
        }
        CType Release() {
            CType result = mHandle;
            mHandle = 0;
            return result;
        }
        static Derived Acquire(CType handle) {
            Derived result;
            result.mHandle = handle;
            return result;
        }

      protected:
        CType mHandle = nullptr;
    };



    class BindGroup : public ObjectBase<BindGroup, WGPUBindGroup> {
      public:
        using ObjectBase::ObjectBase;
        using ObjectBase::operator=;


      private:
        friend ObjectBase<BindGroup, WGPUBindGroup>;
        static void WGPUReference(WGPUBindGroup handle);
        static void WGPURelease(WGPUBindGroup handle);
    };

    class BindGroupLayout : public ObjectBase<BindGroupLayout, WGPUBindGroupLayout> {
      public:
        using ObjectBase::ObjectBase;
        using ObjectBase::operator=;


      private:
        friend ObjectBase<BindGroupLayout, WGPUBindGroupLayout>;
        static void WGPUReference(WGPUBindGroupLayout handle);
        static void WGPURelease(WGPUBindGroupLayout handle);
    };

    class Buffer : public ObjectBase<Buffer, WGPUBuffer> {
      public:
        using ObjectBase::ObjectBase;
        using ObjectBase::operator=;

        void Destroy() const;
        void const * GetConstMappedRange(size_t offset = 0, size_t size = 0) const;
        void * GetMappedRange(size_t offset = 0, size_t size = 0) const;
        void MapAsync(MapMode mode, size_t offset, size_t size, BufferMapCallback callback, void * userdata) const;
        void Unmap() const;

      private:
        friend ObjectBase<Buffer, WGPUBuffer>;
        static void WGPUReference(WGPUBuffer handle);
        static void WGPURelease(WGPUBuffer handle);
    };

    class CommandBuffer : public ObjectBase<CommandBuffer, WGPUCommandBuffer> {
      public:
        using ObjectBase::ObjectBase;
        using ObjectBase::operator=;


      private:
        friend ObjectBase<CommandBuffer, WGPUCommandBuffer>;
        static void WGPUReference(WGPUCommandBuffer handle);
        static void WGPURelease(WGPUCommandBuffer handle);
    };

    class CommandEncoder : public ObjectBase<CommandEncoder, WGPUCommandEncoder> {
      public:
        using ObjectBase::ObjectBase;
        using ObjectBase::operator=;

        ComputePassEncoder BeginComputePass(ComputePassDescriptor const * descriptor = nullptr) const;
        RenderPassEncoder BeginRenderPass(RenderPassDescriptor const * descriptor) const;
        void CopyBufferToBuffer(Buffer const& source, uint64_t sourceOffset, Buffer const& destination, uint64_t destinationOffset, uint64_t size) const;
        void CopyBufferToTexture(ImageCopyBuffer const * source, ImageCopyTexture const * destination, Extent3D const * copySize) const;
        void CopyTextureToBuffer(ImageCopyTexture const * source, ImageCopyBuffer const * destination, Extent3D const * copySize) const;
        void CopyTextureToTexture(ImageCopyTexture const * source, ImageCopyTexture const * destination, Extent3D const * copySize) const;
        CommandBuffer Finish(CommandBufferDescriptor const * descriptor = nullptr) const;
        void InsertDebugMarker(char const * markerLabel) const;
        void PopDebugGroup() const;
        void PushDebugGroup(char const * groupLabel) const;
        void ResolveQuerySet(QuerySet const& querySet, uint32_t firstQuery, uint32_t queryCount, Buffer const& destination, uint64_t destinationOffset) const;
        void WriteTimestamp(QuerySet const& querySet, uint32_t queryIndex) const;

      private:
        friend ObjectBase<CommandEncoder, WGPUCommandEncoder>;
        static void WGPUReference(WGPUCommandEncoder handle);
        static void WGPURelease(WGPUCommandEncoder handle);
    };

    class ComputePassEncoder : public ObjectBase<ComputePassEncoder, WGPUComputePassEncoder> {
      public:
        using ObjectBase::ObjectBase;
        using ObjectBase::operator=;

        void Dispatch(uint32_t x, uint32_t y = 1, uint32_t z = 1) const;
        void DispatchIndirect(Buffer const& indirectBuffer, uint64_t indirectOffset) const;
        void EndPass() const;
        void InsertDebugMarker(char const * markerLabel) const;
        void PopDebugGroup() const;
        void PushDebugGroup(char const * groupLabel) const;
        void SetBindGroup(uint32_t groupIndex, BindGroup const& group, uint32_t dynamicOffsetCount = 0, uint32_t const * dynamicOffsets = nullptr) const;
        void SetPipeline(ComputePipeline const& pipeline) const;
        void WriteTimestamp(QuerySet const& querySet, uint32_t queryIndex) const;

      private:
        friend ObjectBase<ComputePassEncoder, WGPUComputePassEncoder>;
        static void WGPUReference(WGPUComputePassEncoder handle);
        static void WGPURelease(WGPUComputePassEncoder handle);
    };

    class ComputePipeline : public ObjectBase<ComputePipeline, WGPUComputePipeline> {
      public:
        using ObjectBase::ObjectBase;
        using ObjectBase::operator=;

        BindGroupLayout GetBindGroupLayout(uint32_t groupIndex) const;

      private:
        friend ObjectBase<ComputePipeline, WGPUComputePipeline>;
        static void WGPUReference(WGPUComputePipeline handle);
        static void WGPURelease(WGPUComputePipeline handle);
    };

    class Device : public ObjectBase<Device, WGPUDevice> {
      public:
        using ObjectBase::ObjectBase;
        using ObjectBase::operator=;

        BindGroup CreateBindGroup(BindGroupDescriptor const * descriptor) const;
        BindGroupLayout CreateBindGroupLayout(BindGroupLayoutDescriptor const * descriptor) const;
        Buffer CreateBuffer(BufferDescriptor const * descriptor) const;
        CommandEncoder CreateCommandEncoder(CommandEncoderDescriptor const * descriptor = nullptr) const;
        ComputePipeline CreateComputePipeline(ComputePipelineDescriptor const * descriptor) const;
        void CreateComputePipelineAsync(ComputePipelineDescriptor const * descriptor, CreateComputePipelineAsyncCallback callback, void * userdata) const;
        ExternalTexture CreateExternalTexture(ExternalTextureDescriptor const * externalTextureDescriptor) const;
        PipelineLayout CreatePipelineLayout(PipelineLayoutDescriptor const * descriptor) const;
        QuerySet CreateQuerySet(QuerySetDescriptor const * descriptor) const;
        RenderBundleEncoder CreateRenderBundleEncoder(RenderBundleEncoderDescriptor const * descriptor) const;
        RenderPipeline CreateRenderPipeline(RenderPipelineDescriptor const * descriptor) const;
        RenderPipeline CreateRenderPipeline2(RenderPipelineDescriptor2 const * descriptor) const;
        void CreateRenderPipelineAsync(RenderPipelineDescriptor2 const * descriptor, CreateRenderPipelineAsyncCallback callback, void * userdata) const;
        Sampler CreateSampler(SamplerDescriptor const * descriptor = nullptr) const;
        ShaderModule CreateShaderModule(ShaderModuleDescriptor const * descriptor) const;
        SwapChain CreateSwapChain(Surface const& surface, SwapChainDescriptor const * descriptor) const;
        Texture CreateTexture(TextureDescriptor const * descriptor) const;
        Queue GetDefaultQueue() const;
        Queue GetQueue() const;
        bool PopErrorScope(ErrorCallback callback, void * userdata) const;
        void PushErrorScope(ErrorFilter filter) const;
        void SetDeviceLostCallback(DeviceLostCallback callback, void * userdata) const;
        void SetUncapturedErrorCallback(ErrorCallback callback, void * userdata) const;

      private:
        friend ObjectBase<Device, WGPUDevice>;
        static void WGPUReference(WGPUDevice handle);
        static void WGPURelease(WGPUDevice handle);
    };

    class ExternalTexture : public ObjectBase<ExternalTexture, WGPUExternalTexture> {
      public:
        using ObjectBase::ObjectBase;
        using ObjectBase::operator=;

        void Destroy() const;

      private:
        friend ObjectBase<ExternalTexture, WGPUExternalTexture>;
        static void WGPUReference(WGPUExternalTexture handle);
        static void WGPURelease(WGPUExternalTexture handle);
    };

    class Fence : public ObjectBase<Fence, WGPUFence> {
      public:
        using ObjectBase::ObjectBase;
        using ObjectBase::operator=;

        uint64_t GetCompletedValue() const;
        void OnCompletion(uint64_t value, FenceOnCompletionCallback callback, void * userdata) const;

      private:
        friend ObjectBase<Fence, WGPUFence>;
        static void WGPUReference(WGPUFence handle);
        static void WGPURelease(WGPUFence handle);
    };

    class Instance : public ObjectBase<Instance, WGPUInstance> {
      public:
        using ObjectBase::ObjectBase;
        using ObjectBase::operator=;

        Surface CreateSurface(SurfaceDescriptor const * descriptor) const;

      private:
        friend ObjectBase<Instance, WGPUInstance>;
        static void WGPUReference(WGPUInstance handle);
        static void WGPURelease(WGPUInstance handle);
    };

    class PipelineLayout : public ObjectBase<PipelineLayout, WGPUPipelineLayout> {
      public:
        using ObjectBase::ObjectBase;
        using ObjectBase::operator=;


      private:
        friend ObjectBase<PipelineLayout, WGPUPipelineLayout>;
        static void WGPUReference(WGPUPipelineLayout handle);
        static void WGPURelease(WGPUPipelineLayout handle);
    };

    class QuerySet : public ObjectBase<QuerySet, WGPUQuerySet> {
      public:
        using ObjectBase::ObjectBase;
        using ObjectBase::operator=;

        void Destroy() const;

      private:
        friend ObjectBase<QuerySet, WGPUQuerySet>;
        static void WGPUReference(WGPUQuerySet handle);
        static void WGPURelease(WGPUQuerySet handle);
    };

    class Queue : public ObjectBase<Queue, WGPUQueue> {
      public:
        using ObjectBase::ObjectBase;
        using ObjectBase::operator=;

        void CopyTextureForBrowser(ImageCopyTexture const * source, ImageCopyTexture const * destination, Extent3D const * copySize, CopyTextureForBrowserOptions const * options) const;
        Fence CreateFence(FenceDescriptor const * descriptor = nullptr) const;
        void OnSubmittedWorkDone(uint64_t signalValue, QueueWorkDoneCallback callback, void * userdata) const;
        void Signal(Fence const& fence, uint64_t signalValue) const;
        void Submit(uint32_t commandCount, CommandBuffer const * commands) const;
        void WriteBuffer(Buffer const& buffer, uint64_t bufferOffset, void const * data, size_t size) const;
        void WriteTexture(ImageCopyTexture const * destination, void const * data, size_t dataSize, TextureDataLayout const * dataLayout, Extent3D const * writeSize) const;

      private:
        friend ObjectBase<Queue, WGPUQueue>;
        static void WGPUReference(WGPUQueue handle);
        static void WGPURelease(WGPUQueue handle);
    };

    class RenderBundle : public ObjectBase<RenderBundle, WGPURenderBundle> {
      public:
        using ObjectBase::ObjectBase;
        using ObjectBase::operator=;


      private:
        friend ObjectBase<RenderBundle, WGPURenderBundle>;
        static void WGPUReference(WGPURenderBundle handle);
        static void WGPURelease(WGPURenderBundle handle);
    };

    class RenderBundleEncoder : public ObjectBase<RenderBundleEncoder, WGPURenderBundleEncoder> {
      public:
        using ObjectBase::ObjectBase;
        using ObjectBase::operator=;

        void Draw(uint32_t vertexCount, uint32_t instanceCount = 1, uint32_t firstVertex = 0, uint32_t firstInstance = 0) const;
        void DrawIndexed(uint32_t indexCount, uint32_t instanceCount = 1, uint32_t firstIndex = 0, int32_t baseVertex = 0, uint32_t firstInstance = 0) const;
        void DrawIndexedIndirect(Buffer const& indirectBuffer, uint64_t indirectOffset) const;
        void DrawIndirect(Buffer const& indirectBuffer, uint64_t indirectOffset) const;
        RenderBundle Finish(RenderBundleDescriptor const * descriptor = nullptr) const;
        void InsertDebugMarker(char const * markerLabel) const;
        void PopDebugGroup() const;
        void PushDebugGroup(char const * groupLabel) const;
        void SetBindGroup(uint32_t groupIndex, BindGroup const& group, uint32_t dynamicOffsetCount = 0, uint32_t const * dynamicOffsets = nullptr) const;
        void SetIndexBuffer(Buffer const& buffer, IndexFormat format, uint64_t offset = 0, uint64_t size = 0) const;
        void SetIndexBufferWithFormat(Buffer const& buffer, IndexFormat format, uint64_t offset = 0, uint64_t size = 0) const;
        void SetPipeline(RenderPipeline const& pipeline) const;
        void SetVertexBuffer(uint32_t slot, Buffer const& buffer, uint64_t offset = 0, uint64_t size = 0) const;

      private:
        friend ObjectBase<RenderBundleEncoder, WGPURenderBundleEncoder>;
        static void WGPUReference(WGPURenderBundleEncoder handle);
        static void WGPURelease(WGPURenderBundleEncoder handle);
    };

    class RenderPassEncoder : public ObjectBase<RenderPassEncoder, WGPURenderPassEncoder> {
      public:
        using ObjectBase::ObjectBase;
        using ObjectBase::operator=;

        void BeginOcclusionQuery(uint32_t queryIndex) const;
        void Draw(uint32_t vertexCount, uint32_t instanceCount = 1, uint32_t firstVertex = 0, uint32_t firstInstance = 0) const;
        void DrawIndexed(uint32_t indexCount, uint32_t instanceCount = 1, uint32_t firstIndex = 0, int32_t baseVertex = 0, uint32_t firstInstance = 0) const;
        void DrawIndexedIndirect(Buffer const& indirectBuffer, uint64_t indirectOffset) const;
        void DrawIndirect(Buffer const& indirectBuffer, uint64_t indirectOffset) const;
        void EndOcclusionQuery() const;
        void EndPass() const;
        void ExecuteBundles(uint32_t bundlesCount, RenderBundle const * bundles) const;
        void InsertDebugMarker(char const * markerLabel) const;
        void PopDebugGroup() const;
        void PushDebugGroup(char const * groupLabel) const;
        void SetBindGroup(uint32_t groupIndex, BindGroup const& group, uint32_t dynamicOffsetCount = 0, uint32_t const * dynamicOffsets = nullptr) const;
        void SetBlendColor(Color const * color) const;
        void SetBlendConstant(Color const * color) const;
        void SetIndexBuffer(Buffer const& buffer, IndexFormat format, uint64_t offset = 0, uint64_t size = 0) const;
        void SetIndexBufferWithFormat(Buffer const& buffer, IndexFormat format, uint64_t offset = 0, uint64_t size = 0) const;
        void SetPipeline(RenderPipeline const& pipeline) const;
        void SetScissorRect(uint32_t x, uint32_t y, uint32_t width, uint32_t height) const;
        void SetStencilReference(uint32_t reference) const;
        void SetVertexBuffer(uint32_t slot, Buffer const& buffer, uint64_t offset = 0, uint64_t size = 0) const;
        void SetViewport(float x, float y, float width, float height, float minDepth, float maxDepth) const;
        void WriteTimestamp(QuerySet const& querySet, uint32_t queryIndex) const;

      private:
        friend ObjectBase<RenderPassEncoder, WGPURenderPassEncoder>;
        static void WGPUReference(WGPURenderPassEncoder handle);
        static void WGPURelease(WGPURenderPassEncoder handle);
    };

    class RenderPipeline : public ObjectBase<RenderPipeline, WGPURenderPipeline> {
      public:
        using ObjectBase::ObjectBase;
        using ObjectBase::operator=;

        BindGroupLayout GetBindGroupLayout(uint32_t groupIndex) const;

      private:
        friend ObjectBase<RenderPipeline, WGPURenderPipeline>;
        static void WGPUReference(WGPURenderPipeline handle);
        static void WGPURelease(WGPURenderPipeline handle);
    };

    class Sampler : public ObjectBase<Sampler, WGPUSampler> {
      public:
        using ObjectBase::ObjectBase;
        using ObjectBase::operator=;


      private:
        friend ObjectBase<Sampler, WGPUSampler>;
        static void WGPUReference(WGPUSampler handle);
        static void WGPURelease(WGPUSampler handle);
    };

    class ShaderModule : public ObjectBase<ShaderModule, WGPUShaderModule> {
      public:
        using ObjectBase::ObjectBase;
        using ObjectBase::operator=;


      private:
        friend ObjectBase<ShaderModule, WGPUShaderModule>;
        static void WGPUReference(WGPUShaderModule handle);
        static void WGPURelease(WGPUShaderModule handle);
    };

    class Surface : public ObjectBase<Surface, WGPUSurface> {
      public:
        using ObjectBase::ObjectBase;
        using ObjectBase::operator=;


      private:
        friend ObjectBase<Surface, WGPUSurface>;
        static void WGPUReference(WGPUSurface handle);
        static void WGPURelease(WGPUSurface handle);
    };

    class SwapChain : public ObjectBase<SwapChain, WGPUSwapChain> {
      public:
        using ObjectBase::ObjectBase;
        using ObjectBase::operator=;

        void Configure(TextureFormat format, TextureUsage allowedUsage, uint32_t width, uint32_t height) const;
        TextureView GetCurrentTextureView() const;
        void Present() const;

      private:
        friend ObjectBase<SwapChain, WGPUSwapChain>;
        static void WGPUReference(WGPUSwapChain handle);
        static void WGPURelease(WGPUSwapChain handle);
    };

    class Texture : public ObjectBase<Texture, WGPUTexture> {
      public:
        using ObjectBase::ObjectBase;
        using ObjectBase::operator=;

        TextureView CreateView(TextureViewDescriptor const * descriptor = nullptr) const;
        void Destroy() const;

      private:
        friend ObjectBase<Texture, WGPUTexture>;
        static void WGPUReference(WGPUTexture handle);
        static void WGPURelease(WGPUTexture handle);
    };

    class TextureView : public ObjectBase<TextureView, WGPUTextureView> {
      public:
        using ObjectBase::ObjectBase;
        using ObjectBase::operator=;


      private:
        friend ObjectBase<TextureView, WGPUTextureView>;
        static void WGPUReference(WGPUTextureView handle);
        static void WGPURelease(WGPUTextureView handle);
    };


    Instance CreateInstance(InstanceDescriptor const * descriptor = nullptr);
    Proc GetProcAddress(Device const& device, const char* procName);

    struct ChainedStruct {
        ChainedStruct const * nextInChain = nullptr;
        SType sType = SType::Invalid;
    };

    struct AdapterProperties {
        ChainedStruct const * nextInChain = nullptr;
        uint32_t deviceID;
        uint32_t vendorID;
        char const * name;
        char const * driverDescription;
        AdapterType adapterType;
        BackendType backendType;
    };

    struct BindGroupEntry {
        uint32_t binding;
        Buffer buffer = nullptr;
        uint64_t offset = 0;
        uint64_t size;
        Sampler sampler = nullptr;
        TextureView textureView = nullptr;
    };

    struct BlendComponent {
        BlendOperation operation = BlendOperation::Add;
        BlendFactor srcFactor = BlendFactor::One;
        BlendFactor dstFactor = BlendFactor::Zero;
    };

    struct BufferBindingLayout {
        ChainedStruct const * nextInChain = nullptr;
        BufferBindingType type = BufferBindingType::Undefined;
        bool hasDynamicOffset = false;
        uint64_t minBindingSize = 0;
    };

    struct BufferDescriptor {
        ChainedStruct const * nextInChain = nullptr;
        char const * label = nullptr;
        BufferUsage usage;
        uint64_t size;
        bool mappedAtCreation = false;
    };

    struct Color {
        double r;
        double g;
        double b;
        double a;
    };

    struct CommandBufferDescriptor {
        ChainedStruct const * nextInChain = nullptr;
        char const * label = nullptr;
    };

    struct CommandEncoderDescriptor {
        ChainedStruct const * nextInChain = nullptr;
        char const * label = nullptr;
    };

    struct ComputePassDescriptor {
        ChainedStruct const * nextInChain = nullptr;
        char const * label = nullptr;
    };

    struct CopyTextureForBrowserOptions {
        ChainedStruct const * nextInChain = nullptr;
        bool flipY = false;
    };

    struct DeviceProperties {
        bool textureCompressionBC = false;
        bool shaderFloat16 = false;
        bool pipelineStatisticsQuery = false;
        bool timestampQuery = false;
        bool multiPlanarFormats = false;
        bool depthClamping = false;
    };

    struct Extent3D {
        uint32_t width;
        uint32_t height = 1;
        uint32_t depthOrArrayLayers = 1;
        uint32_t depth = 1;
    };

    struct ExternalTextureDescriptor {
        ChainedStruct const * nextInChain = nullptr;
        TextureView plane0;
        TextureFormat format;
    };

    struct FenceDescriptor {
        ChainedStruct const * nextInChain = nullptr;
        char const * label = nullptr;
        uint64_t initialValue = 0;
    };

    struct InstanceDescriptor {
        ChainedStruct const * nextInChain = nullptr;
    };

    struct MultisampleState {
        ChainedStruct const * nextInChain = nullptr;
        uint32_t count = 1;
        uint32_t mask = 0xFFFFFFFF;
        bool alphaToCoverageEnabled = false;
    };

    struct Origin3D {
        uint32_t x = 0;
        uint32_t y = 0;
        uint32_t z = 0;
    };

    struct PipelineLayoutDescriptor {
        ChainedStruct const * nextInChain = nullptr;
        char const * label = nullptr;
        uint32_t bindGroupLayoutCount;
        BindGroupLayout const * bindGroupLayouts;
    };

    struct PrimitiveDepthClampingState : ChainedStruct {
        PrimitiveDepthClampingState() {
            sType = SType::PrimitiveDepthClampingState;
        }
        alignas(ChainedStruct) bool clampDepth = false;
    };

    struct PrimitiveState {
        ChainedStruct const * nextInChain = nullptr;
        PrimitiveTopology topology = PrimitiveTopology::TriangleList;
        IndexFormat stripIndexFormat = IndexFormat::Undefined;
        FrontFace frontFace = FrontFace::CCW;
        CullMode cullMode = CullMode::None;
    };

    struct ProgrammableStageDescriptor {
        ChainedStruct const * nextInChain = nullptr;
        ShaderModule module;
        char const * entryPoint;
    };

    struct QuerySetDescriptor {
        ChainedStruct const * nextInChain = nullptr;
        char const * label = nullptr;
        QueryType type;
        uint32_t count;
        PipelineStatisticName const * pipelineStatistics;
        uint32_t pipelineStatisticsCount = 0;
    };

    struct RasterizationStateDescriptor {
        ChainedStruct const * nextInChain = nullptr;
        FrontFace frontFace = FrontFace::CCW;
        CullMode cullMode = CullMode::None;
        int32_t depthBias = 0;
        float depthBiasSlopeScale = 0.0f;
        float depthBiasClamp = 0.0f;
    };

    struct RenderBundleDescriptor {
        ChainedStruct const * nextInChain = nullptr;
        char const * label = nullptr;
    };

    struct RenderBundleEncoderDescriptor {
        ChainedStruct const * nextInChain = nullptr;
        char const * label = nullptr;
        uint32_t colorFormatsCount;
        TextureFormat const * colorFormats;
        TextureFormat depthStencilFormat = TextureFormat::Undefined;
        uint32_t sampleCount = 1;
    };

    struct RenderPassDepthStencilAttachment {
        TextureView view = nullptr;
        LoadOp depthLoadOp;
        StoreOp depthStoreOp;
        float clearDepth;
        bool depthReadOnly = false;
        LoadOp stencilLoadOp;
        StoreOp stencilStoreOp;
        uint32_t clearStencil = 0;
        bool stencilReadOnly = false;
        TextureView attachment = nullptr;
    };

    struct SamplerBindingLayout {
        ChainedStruct const * nextInChain = nullptr;
        SamplerBindingType type = SamplerBindingType::Undefined;
    };

    struct SamplerDescriptor {
        ChainedStruct const * nextInChain = nullptr;
        char const * label = nullptr;
        AddressMode addressModeU = AddressMode::ClampToEdge;
        AddressMode addressModeV = AddressMode::ClampToEdge;
        AddressMode addressModeW = AddressMode::ClampToEdge;
        FilterMode magFilter = FilterMode::Nearest;
        FilterMode minFilter = FilterMode::Nearest;
        FilterMode mipmapFilter = FilterMode::Nearest;
        float lodMinClamp = 0.0f;
        float lodMaxClamp = 1000.0f;
        CompareFunction compare = CompareFunction::Undefined;
        uint16_t maxAnisotropy = 1;
    };

    struct ShaderModuleDescriptor {
        ChainedStruct const * nextInChain = nullptr;
        char const * label = nullptr;
    };

    struct ShaderModuleSPIRVDescriptor : ChainedStruct {
        ShaderModuleSPIRVDescriptor() {
            sType = SType::ShaderModuleSPIRVDescriptor;
        }
        alignas(ChainedStruct) uint32_t codeSize;
        uint32_t const * code;
    };

    struct ShaderModuleWGSLDescriptor : ChainedStruct {
        ShaderModuleWGSLDescriptor() {
            sType = SType::ShaderModuleWGSLDescriptor;
        }
        alignas(ChainedStruct) char const * source;
    };

    struct StencilFaceState {
        CompareFunction compare = CompareFunction::Always;
        StencilOperation failOp = StencilOperation::Keep;
        StencilOperation depthFailOp = StencilOperation::Keep;
        StencilOperation passOp = StencilOperation::Keep;
    };

    struct StorageTextureBindingLayout {
        ChainedStruct const * nextInChain = nullptr;
        StorageTextureAccess access = StorageTextureAccess::Undefined;
        TextureFormat format = TextureFormat::Undefined;
        TextureViewDimension viewDimension = TextureViewDimension::Undefined;
    };

    struct SurfaceDescriptor {
        ChainedStruct const * nextInChain = nullptr;
        char const * label = nullptr;
    };

    struct SurfaceDescriptorFromCanvasHTMLSelector : ChainedStruct {
        SurfaceDescriptorFromCanvasHTMLSelector() {
            sType = SType::SurfaceDescriptorFromCanvasHTMLSelector;
        }
        alignas(ChainedStruct) char const * selector;
    };

    struct SurfaceDescriptorFromMetalLayer : ChainedStruct {
        SurfaceDescriptorFromMetalLayer() {
            sType = SType::SurfaceDescriptorFromMetalLayer;
        }
        alignas(ChainedStruct) void * layer;
    };

    struct SurfaceDescriptorFromWindowsHWND : ChainedStruct {
        SurfaceDescriptorFromWindowsHWND() {
            sType = SType::SurfaceDescriptorFromWindowsHWND;
        }
        alignas(ChainedStruct) void * hinstance;
        void * hwnd;
    };

    struct SurfaceDescriptorFromXlib : ChainedStruct {
        SurfaceDescriptorFromXlib() {
            sType = SType::SurfaceDescriptorFromXlib;
        }
        alignas(ChainedStruct) void * display;
        uint32_t window;
    };

    struct SwapChainDescriptor {
        ChainedStruct const * nextInChain = nullptr;
        char const * label = nullptr;
        TextureUsage usage;
        TextureFormat format;
        uint32_t width;
        uint32_t height;
        PresentMode presentMode;
    };

    struct TextureBindingLayout {
        ChainedStruct const * nextInChain = nullptr;
        TextureSampleType sampleType = TextureSampleType::Undefined;
        TextureViewDimension viewDimension = TextureViewDimension::Undefined;
        bool multisampled = false;
    };

    struct TextureDataLayout {
        ChainedStruct const * nextInChain = nullptr;
        uint64_t offset = 0;
        uint32_t bytesPerRow = WGPU_COPY_STRIDE_UNDEFINED;
        uint32_t rowsPerImage = WGPU_COPY_STRIDE_UNDEFINED;
    };

    struct TextureViewDescriptor {
        ChainedStruct const * nextInChain = nullptr;
        char const * label = nullptr;
        TextureFormat format = TextureFormat::Undefined;
        TextureViewDimension dimension = TextureViewDimension::Undefined;
        uint32_t baseMipLevel = 0;
        uint32_t mipLevelCount = 0;
        uint32_t baseArrayLayer = 0;
        uint32_t arrayLayerCount = 0;
        TextureAspect aspect = TextureAspect::All;
    };

    struct VertexAttribute {
        VertexFormat format;
        uint64_t offset;
        uint32_t shaderLocation;
    };

    struct BindGroupDescriptor {
        ChainedStruct const * nextInChain = nullptr;
        char const * label = nullptr;
        BindGroupLayout layout;
        uint32_t entryCount;
        BindGroupEntry const * entries;
    };

    struct BindGroupLayoutEntry {
        ChainedStruct const * nextInChain = nullptr;
        uint32_t binding;
        ShaderStage visibility;
        BindingType type = BindingType::Undefined;
        bool hasDynamicOffset = false;
        uint64_t minBufferBindingSize = 0;
        TextureViewDimension viewDimension = TextureViewDimension::Undefined;
        TextureComponentType textureComponentType = TextureComponentType::Float;
        TextureFormat storageTextureFormat = TextureFormat::Undefined;
        BufferBindingLayout buffer;
        SamplerBindingLayout sampler;
        TextureBindingLayout texture;
        StorageTextureBindingLayout storageTexture;
    };

    struct BlendState {
        BlendComponent color;
        BlendComponent alpha;
    };

    struct ColorStateDescriptor {
        ChainedStruct const * nextInChain = nullptr;
        TextureFormat format;
        BlendComponent alphaBlend;
        BlendComponent colorBlend;
        ColorWriteMask writeMask = ColorWriteMask::All;
    };

    struct ComputePipelineDescriptor {
        ChainedStruct const * nextInChain = nullptr;
        char const * label = nullptr;
        PipelineLayout layout = nullptr;
        ProgrammableStageDescriptor computeStage;
    };

    struct DepthStencilState {
        ChainedStruct const * nextInChain = nullptr;
        TextureFormat format;
        bool depthWriteEnabled = false;
        CompareFunction depthCompare = CompareFunction::Always;
        StencilFaceState stencilFront;
        StencilFaceState stencilBack;
        uint32_t stencilReadMask = 0xFFFFFFFF;
        uint32_t stencilWriteMask = 0xFFFFFFFF;
        int32_t depthBias = 0;
        float depthBiasSlopeScale = 0.0f;
        float depthBiasClamp = 0.0f;
    };

    struct DepthStencilStateDescriptor {
        ChainedStruct const * nextInChain = nullptr;
        TextureFormat format;
        bool depthWriteEnabled = false;
        CompareFunction depthCompare = CompareFunction::Always;
        StencilFaceState stencilFront;
        StencilFaceState stencilBack;
        uint32_t stencilReadMask = 0xFFFFFFFF;
        uint32_t stencilWriteMask = 0xFFFFFFFF;
    };

    struct ImageCopyBuffer {
        ChainedStruct const * nextInChain = nullptr;
        TextureDataLayout layout;
        Buffer buffer;
    };

    struct ImageCopyTexture {
        ChainedStruct const * nextInChain = nullptr;
        Texture texture;
        uint32_t mipLevel = 0;
        Origin3D origin;
        TextureAspect aspect = TextureAspect::All;
    };

    struct RenderPassColorAttachment {
        TextureView view = nullptr;
        TextureView resolveTarget = nullptr;
        LoadOp loadOp;
        StoreOp storeOp;
        Color clearColor;
        TextureView attachment = nullptr;
    };

    struct TextureDescriptor {
        ChainedStruct const * nextInChain = nullptr;
        char const * label = nullptr;
        TextureUsage usage;
        TextureDimension dimension = TextureDimension::e2D;
        Extent3D size;
        TextureFormat format;
        uint32_t mipLevelCount = 1;
        uint32_t sampleCount = 1;
    };

    struct VertexBufferLayout {
        uint64_t arrayStride;
        InputStepMode stepMode = InputStepMode::Vertex;
        uint32_t attributeCount;
        VertexAttribute const * attributes;
    };

    struct BindGroupLayoutDescriptor {
        ChainedStruct const * nextInChain = nullptr;
        char const * label = nullptr;
        uint32_t entryCount;
        BindGroupLayoutEntry const * entries;
    };

    struct ColorTargetState {
        ChainedStruct const * nextInChain = nullptr;
        TextureFormat format;
        BlendState const * blend = nullptr;
        ColorWriteMask writeMask = ColorWriteMask::All;
    };

    struct RenderPassDescriptor {
        ChainedStruct const * nextInChain = nullptr;
        char const * label = nullptr;
        uint32_t colorAttachmentCount;
        RenderPassColorAttachment const * colorAttachments;
        RenderPassDepthStencilAttachment const * depthStencilAttachment = nullptr;
        QuerySet occlusionQuerySet = nullptr;
    };

    struct VertexState {
        ChainedStruct const * nextInChain = nullptr;
        ShaderModule module;
        char const * entryPoint;
        uint32_t bufferCount = 0;
        VertexBufferLayout const * buffers;
    };

    struct VertexStateDescriptor {
        ChainedStruct const * nextInChain = nullptr;
        IndexFormat indexFormat = IndexFormat::Undefined;
        uint32_t vertexBufferCount = 0;
        VertexBufferLayout const * vertexBuffers;
    };

    struct FragmentState {
        ChainedStruct const * nextInChain = nullptr;
        ShaderModule module;
        char const * entryPoint;
        uint32_t targetCount;
        ColorTargetState const * targets;
    };

    struct RenderPipelineDescriptor {
        ChainedStruct const * nextInChain = nullptr;
        char const * label = nullptr;
        PipelineLayout layout = nullptr;
        ProgrammableStageDescriptor vertexStage;
        ProgrammableStageDescriptor const * fragmentStage = nullptr;
        VertexStateDescriptor const * vertexState = nullptr;
        PrimitiveTopology primitiveTopology;
        RasterizationStateDescriptor const * rasterizationState = nullptr;
        uint32_t sampleCount = 1;
        DepthStencilStateDescriptor const * depthStencilState = nullptr;
        uint32_t colorStateCount;
        ColorStateDescriptor const * colorStates;
        uint32_t sampleMask = 0xFFFFFFFF;
        bool alphaToCoverageEnabled = false;
    };

    struct RenderPipelineDescriptor2 {
        ChainedStruct const * nextInChain = nullptr;
        char const * label = nullptr;
        PipelineLayout layout = nullptr;
        VertexState vertex;
        PrimitiveState primitive;
        DepthStencilState const * depthStencil = nullptr;
        MultisampleState multisample;
        FragmentState const * fragment = nullptr;
    };


}  // namespace wgpu

#endif // WEBGPU_CPP_H_
