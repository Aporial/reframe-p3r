Texture2D<float2> ObjectMotion : register(t0);
Texture2D<float> SceneDepth : register(t1);
Texture2D<float4> EyeAdaptation : register(t2);
RWTexture2D<float2> HistoryMotion : register(u0);
RWTexture2D<float> Exposure : register(u1);

cbuffer Reprojection : register(b0) {
    row_major float4x4 CurrentToHistory;
    float2 InputSize;
};

// The game encodes current minus history in NDC. The SDKs consume history minus
// current in render pixels, with Y down. Coefficients come from the game DXIL.
float2 DecodePixelDelta(float2 rg) {
    const float2 historyDeltaNdc = 2.0039775371551513671875 - rg * 4.008016109466552734375;
    return historyDeltaNdc * (InputSize * float2(0.5, -0.5));
}

float2 ReprojectPixel(uint2 pixel) {
    const float2 center = float2(pixel) + 0.5;
    const float2 ndc = (center / InputSize - 0.5) * float2(2, -2);
    const float depth = SceneDepth.Load(int3(pixel, 0));
    const float4 history = mul(float4(ndc, depth, 1), CurrentToHistory);
    if (abs(history.w) < 1e-6) return float2(0, 0);
    const float2 historyCenter = (history.xy / history.w * float2(0.5, -0.5) + 0.5) * InputSize;
    return historyCenter - center;
}

[numthreads(16, 4, 1)]
void main(uint2 pixel : SV_DispatchThreadID) {
    if (all(pixel == 0)) {
        const float eye = EyeAdaptation.Load(int3(0, 0, 0)).r;
        Exposure[uint2(0, 0)] = eye > 0 && isfinite(eye) ? eye : 1;
    }
    if (any(pixel >= uint2(InputSize))) return;

    const float2 packed = ObjectMotion.Load(int3(pixel, 0));
    const float2 delta = packed.r > 0 ? DecodePixelDelta(packed) : ReprojectPixel(pixel);
    HistoryMotion[pixel] = all(isfinite(delta)) ? delta : float2(0, 0);
}
