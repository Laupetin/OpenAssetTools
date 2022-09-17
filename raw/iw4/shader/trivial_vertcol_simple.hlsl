#include "include/IW4.hlsl"

struct VSInput
{
	float3 position : POSITION;
    half4 color : COLOR0;
    half2 texcoord : TEXCOORD0;
};

struct VSOutput
{
	float4 position : SV_POSITION;
	half4 color : COLOR0;
    half2 texcoord : TEXCOORD0;
};

VSOutput VSMain(VSInput vin)
{
	VSOutput vout = (VSOutput)0;

	vout.position = mul(mul(float4(vin.position, 1.0f), worldMatrix), viewProjectionMatrix);
	vout.color = vin.color;
	vout.texcoord = vin.texcoord;

	return vout;
}

extern sampler2D colorMapSampler;

half4 PSMain(VSOutput input) : SV_TARGET
{
    return half4(tex2D(colorMapSampler, input.texcoord)) * input.color;
}