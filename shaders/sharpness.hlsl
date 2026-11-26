// Adapted from AMD FidelityFX CAS (sharpen-only path).
// Copyright (c) 2020 Advanced Micro Devices, Inc. All rights reserved.
// See licenses/AMD-FidelityFX-CAS.txt for the MIT license.

Texture2D<float4> Scene : register(t0);
RWTexture2D<float4> Sharpened : register(u0);

cbuffer Sharpness : register(b0) {
    uint2 OutputSize;
    float Peak;
    float PreExposure;
    float ChannelLimit;
};

float Luma(int2 p) {
    const float3 rgb = Scene.Load(int3(clamp(p, 0, int2(OutputSize) - 1), 0)).rgb;
    return max(dot(rgb, float3(0.2126, 0.7152, 0.0722)), 0.0);
}

float AnalysisSignal(float luma) {
    // Bound the contrast analysis in pre-exposed linear HDR. The filter and
    // writeback still operate on the original linear signal.
    return luma / (luma + PreExposure);
}

[numthreads(8, 8, 1)]
void main(uint3 id : SV_DispatchThreadID) {
    if (any(id.xy >= OutputSize)) return;
    const int2 p = id.xy;
    const float4 center = Scene.Load(int3(p, 0));
    const float e = dot(center.rgb, float3(0.2126, 0.7152, 0.0722));
    const float b = Luma(p + int2(0, -1));
    const float d = Luma(p + int2(-1, 0));
    const float f = Luma(p + int2(1, 0));
    const float h = Luma(p + int2(0, 1));
    const float a = Luma(p + int2(-1, -1));
    const float c = Luma(p + int2(1, -1));
    const float g = Luma(p + int2(-1, 1));
    const float i = Luma(p + int2(1, 1));
    const float crossMin = min(e, min(min(b, d), min(f, h)));
    const float crossMax = max(e, max(max(b, d), max(f, h)));
    const float localMin = min(crossMin, min(min(a, c), min(g, i)));
    const float localMax = max(crossMax, max(max(a, c), max(g, i)));
    const float softMin = AnalysisSignal(max(crossMin, 0.0)) + AnalysisSignal(max(localMin, 0.0));
    const float softMax = AnalysisSignal(crossMax) + AnalysisSignal(localMax);
    const float amplitude = sqrt(saturate(min(softMin, 2.0 - softMax) / max(softMax, 1e-6)));
    const float w = amplitude * Peak;
    const float filtered = clamp(e + w * ((b + d + f + h) - 4.0 * e) / (1.0 + 4.0 * w), localMin, localMax);
    float gain = 1.0;
    if (e > 1e-6 && filtered != e && isfinite(filtered)) {
        const float maxChannel = max(max(abs(center.r), abs(center.g)), abs(center.b));
        gain = clamp(1.0 + (filtered - e) / e, 0.75, 1.25);
        if (maxChannel * gain > ChannelLimit) gain = ChannelLimit / maxChannel;
    }
    // A shared luminance gain preserves the center pixel's chromaticity and alpha.
    Sharpened[p] = float4(center.rgb * gain, center.a);
}
