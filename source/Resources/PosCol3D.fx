// Input/Output structs
struct VS_INPUT
{
    float3 Position : POSITION;
    float2 UV : TEXCOORD;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float4 WorldPosition : COLOR;
    float2 UV : TEXCOORD;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
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
float4x4 gWorld : World;
float3 gCameraPos : CameraPos;

Texture2D gDiffuse : DiffuseMap;
Texture2D gSpecular : SpecularMap;
Texture2D gNormal : NormalMap;
Texture2D gGlossiness : GlossinessMap;

// Constants
float3 gLightDir : LightDir = float3(0.577f, -0.577f, 0.577f);
float gPI = 3.1415926f;
float gLightIntensity = 7.f;
float gShininess = 25.f;
float3 gAmbient = float3(0.025f, 0.025f, 0.025f);

// Vertex Shader
VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.Position = mul(float4(input.Position, 1.f), gWorldViewProjection);
    output.WorldPosition = mul(float4(input.Position, 1.f), gWorld);

    output.UV = input.UV;

    output.Normal = mul(normalize(input.Normal), (float3x3)gWorld);
    output.Tangent = mul(normalize(input.Tangent), (float3x3)gWorld);

    return output;
}

// Pixel shader
// BRDF Phong
float BRDFPhong(float3 specular, float phongExponent, float3 viewDirection, float3 normal)
{
    float3 reflected = reflect(gLightDir, normal);
    float dotVal = dot(reflected, viewDirection);
    float angle = max(0.f, dotVal);
    float phongValue = specular * pow(angle, phongExponent);

    return phongValue;
}

// BRDF Lambert
float3 BRDFLambert(float kd, float3 color)
{
    return mul(color, kd / gPI);
}

// Pixel shader
float3 Shade(VS_OUTPUT input, SamplerState state)
{
    float3 normalMapSample = gNormal.Sample(state, input.UV).xyz;
    float3 specularMapSample = gSpecular.Sample(state, input.UV).xyz;
    float3 glossinessMapSample = gGlossiness.Sample(state, input.UV).xyz;
    float3 diffuseMapSample = gDiffuse.Sample(state, input.UV).xyz;

    float3 bi = normalize(cross(input.Normal, input.Tangent));
    // Construct tangent space matrix for transformation
    float3x3 tangentSpaceAxis = float3x3(normalize(input.Tangent), bi, normalize(input.Normal));
    float3 transformedNormal = 2.f * normalMapSample - float3(1.0f, 1.0f, 1.0f);
    float3 normal = normalize(mul(transformedNormal, tangentSpaceAxis));

    float cosineLaw = clamp(dot(normal, -gLightDir), 0.f, 1.f);
    float3 viewDirection = -normalize(gCameraPos - input.WorldPosition.xyz);

    float phongValue = BRDFPhong(specularMapSample, glossinessMapSample.x * gShininess, -viewDirection, normal);
    float3 diffuse = BRDFLambert(1.f, diffuseMapSample);

    return ((gLightIntensity * diffuse) + phongValue + gAmbient) * cosineLaw;

}

float4 PS_Point(VS_OUTPUT input) : SV_Target
{
    return float4(Shade(input, pointSampler), 1);
}

float4 PS_Linear(VS_OUTPUT input) : SV_Target
{
    return float4(Shade(input, linearSampler), 1);
}

float4 PS_Anisotropic(VS_OUTPUT input) : SV_Target
{
    return float4(Shade(input, anisotropicSampler), 1);
}

float4 PS_NormalDisplay(VS_OUTPUT input) : SV_Target
{
    return float4(input.Normal, 1);
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

technique11 NormalDisplayTechnique
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_NormalDisplay()));
    }
}
