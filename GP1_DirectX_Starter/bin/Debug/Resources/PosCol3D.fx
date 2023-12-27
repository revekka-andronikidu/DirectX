// -----------------------------------------------------
// Global Variables
// -----------------------------------------------------

//Textures
Texture2D gDiffuseMap: DiffuseMap;

//Matrices
float4x4 gWorldViewProj : WorldViewProjection;

//Maths
float PI = float(3.14159265f);

SamplerState gSamStatePoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Wrap; //or Mirror, Clamp, Border
    AddressV = Wrap; //or Mirror, Clamp, Border
};

// -----------------------------------------------------
// Input/Output structs
// -----------------------------------------------------
struct VS_INPUT
{
    float3 Position : POSITION;
    float3 Color : COLOR;
    float2 TextureUV : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float3 Color : COLOR;
    float2 TextureUV : TEXCOORD;
 
};

// -----------------------------------------------------
// Vertex Shader
// -----------------------------------------------------
VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.Position = mul(float4(input.Position, 1.f), gWorldViewProj);
    output.Color = input.Color;
    output.TextureUV = input.TextureUV;

    return output;
}

// -----------------------------------------------------
// Pixel Shader
// -----------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_TARGET
{
    input.Color = gDiffuseMap.Sample(gSamStatePoint, input.TextureUV);
    return float4(input.Color, 1.f);
}

// -----------------------------------------------------
// Technique
// -----------------------------------------------------
technique11 DefaultTechnique
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS() ) );
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}
