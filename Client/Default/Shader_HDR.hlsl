Texture2D<float4> hdrTexture : register(t0);
RWTexture2D<float> luminanceTexture : register(u0);

[numthreads(16, 16, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    float4 color = hdrTexture[DTid.xy];
    float luminance = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
    luminanceTexture[DTid.xy] = luminance;
}