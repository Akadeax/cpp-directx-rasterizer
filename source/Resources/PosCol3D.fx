// Input/Output structs
struct VS_INPUT
{
    float3 Position : POSITION;
    float2 UV : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float2 UV : TEXCOORD;
};

SamplerState pointSampler
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Wrap;
    AddressV = Wrap;
};
SamplerState linearSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};
SamplerState anisotropicSampler
{
    Filter = ANISOTROPIC;
    AddressU = Wrap;
    AddressV = Wrap;
};
// Globals
float4x4 gWorldViewProjection : WorldViewProjection;
Texture2D gDiffuseMap : DiffuseMap;

// Vertex Shader
VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.Position = mul(float4(input.Position, 1.f), gWorldViewProjection);

    output.UV = input.UV;
    return output;
}

// Pixel shader
float4 PS_Point(VS_OUTPUT input) : SV_Target
{
    float3 diffuse = gDiffuseMap.Sample(pointSampler, input.UV).rgb;
    return float4(diffuse, 1);
}
float4 PS_Linear(VS_OUTPUT input) : SV_Target
{
    float3 diffuse = gDiffuseMap.Sample(linearSampler, input.UV).rgb;
    return float4(diffuse, 1);
}
float4 PS_Anisotropic(VS_OUTPUT input) : SV_Target
{
    float3 diffuse = gDiffuseMap.Sample(anisotropicSampler, input.UV).rgb;
    return float4(diffuse, 1);
}
// Technique
technique11 PointTechnique
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_Point()));
    }
}
technique11 LinearTechnique
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_Linear()));
    }
}

technique11 AnisotropicTechnique
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_Anisotropic()));
    }
}
