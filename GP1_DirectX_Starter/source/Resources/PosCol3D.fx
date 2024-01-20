// -----------------------------------------------------
// Global Variables
// -----------------------------------------------------

//Textures
Texture2D gDiffuseMap : DiffuseMap;
Texture2D gNormalMap : NormalMap;
Texture2D gSpecularMap : SpecularMap;
Texture2D gGlossinessMap : GlossinessMap;

bool gUseNormalMap : UseNormalMap;


//Matrices
float4x4 gWorldViewProj : WorldViewProjection;
float4x4 gWorldMatrix : World;
float4x4 gViewInverseMatrix : ViewInverse;

//Light
float3 gLightDirection = normalize(float3(0.577f, -0.577f, 0.577f));
float gLightIntensity = 7.f;

float gShininess = 25.f;

//Camera
float3 gCameraPosition;

//Maths
float PI = float(3.14159265f);

SamplerState gSamStatePoint : SampleState
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Wrap; // or Mirror, Clamp, Border
    AddressV = Wrap; // or Mirror, Clamp, Border
};

SamplerState gSamStateLinear : SampleState
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap; // or Mirror, Clamp, Border
    AddressV = Wrap; // or Mirror, Clamp, Border
};

SamplerState gSamStateAnisotropic : SampleState
{
    Filter = ANISOTROPIC;
    AddressU = Wrap; // or Mirror, Clamp, Border
    AddressV = Wrap; // or Mirror, Clamp, Border
};


// -----------------------------------------------------
// Input/Output structs
// -----------------------------------------------------
struct VS_INPUT
{
    float3 Position : POSITION;
    float2 UV : TEXCOORD;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
};

struct VS_OUTPUT
{
    float4 Position      : SV_POSITION0;
    float4 WorldPosition : TEXCOORD0;
    float2 UV            : TEXCOORD1;
    float3 Normal        : NORMAL;
    float3 Tangent       : TANGENT;
   
 
   // float4 WorldPosition: WORLDPOSITION;
};

// -----------------------------------------------------
// Vertex Shader
// -----------------------------------------------------
VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.Position = mul(float4(input.Position, 1.f), gWorldViewProj);
    //output.Color = input.Color;
    output.UV = input.UV;
    output.Tangent = mul(normalize(input.Tangent), (float3x3)gWorldMatrix);
    output.Normal = mul(normalize(input.Normal), (float3x3)gWorldMatrix);

    // output.WorldPosition = mul(input.Position, gWorldMatrix);

    return output;
};

// -----------------------------------------------------
// Materials
// -----------------------------------------------------

float4 Lambert(float kd, float4 cd)
{
    float4 lambert = (cd * kd) / PI;
    return  lambert;
}

float Phong(float ks, float exp, float3 lightVector, float3 viewDirection,float3 normal)
{
    float3 reflection = reflect(lightVector, normal);
    float cosA = saturate( dot(reflection, viewDirection));
    float specularReflection = 0;
    if (cosA > 0)
    {
        specularReflection = ks * pow(cosA, exp);
    }
    return specularReflection;
}

// -----------------------------------------------------
// Pixel Shader
// -----------------------------------------------------
float4 PS_Phong(VS_OUTPUT input, SamplerState state) : SV_TARGET
{
    const float3 binormal = cross(input.Normal, input.Tangent);
    float4 zeroVector = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float4x4 tangentSpaceAxis = float4x4( float4(input.Tangent, 0.0f), float4(binormal, 0.0f), float4(input.Normal, 0.0f), zeroVector );

    const float3 normalSampleVec = (2.0f * gNormalMap.Sample(state, input.UV).rgb - float3(1.f,1.f,1.f) );
   
    float3 normal;
    if(gUseNormalMap)
    {
        normal = mul(float4(normalSampleVec, 0.f), tangentSpaceAxis);
    }
    else
    {
        normal = input.Normal;
    }

    const float3 invViewDirection = normalize(gCameraPosition - input.WorldPosition.xyz);

    const float observedArea = saturate(dot(normal, -gLightDirection) );
    const float4 lambert = Lambert(1.0f, gDiffuseMap.Sample(state, input.UV));
    const float specularVal = gShininess * gGlossinessMap.Sample(state, input.UV).r ;
    const float4 specular = gSpecularMap.Sample(state, input.UV) * Phong(1.0f, specularVal, -gLightDirection, invViewDirection, input.Normal);
       
    return float4 ((gLightIntensity * lambert + specular) * observedArea);
}

float4 PS_Point(VS_OUTPUT input) : SV_TARGET
{
    //float3 diffuse = gDiffuseMap.Sample(gSamStatePoint, input.UV);
    //return float4(diffuse, 1.f);
  return PS_Phong(input,gSamStatePoint); 
}

float4 PS_Linear(VS_OUTPUT input) : SV_TARGET
{
    //float3 diffuse = gDiffuseMap.Sample(gSamStateLinear, input.UV);
   // return float4(diffuse, 1.f);
  return PS_Phong(input,gSamStateLinear); 
}

float4 PS_Anisotropic(VS_OUTPUT input) : SV_TARGET
{
    //float3 diffuse = gDiffuseMap.Sample(gSamStateAnisotropic, input.UV);
   // return float4(diffuse, 1.f);
   return PS_Phong(input, gSamStateAnisotropic);
}


// RasterizerState
RasterizerState gRasterizerState
{
    CullMode = back;
    FrontCounterClockwise = false; // Default
};


//BlendState
BlendState gBlendState
{
    BlendEnable[0] = false;
    RenderTargetWriteMask[0] = 0x0F;
};


//DepthState
DepthStencilState gDepthStencilState
{
    DepthEnable = true;
    DepthWriteMask = 1;
    DepthFunc = less;
    StencilEnable = false;
};
// -----------------------------------------------------
// Technique
// -----------------------------------------------------
technique11 PointFilteringTechnique
{
    pass P0
    {
        SetRasterizerState(gRasterizerState);
       SetDepthStencilState(gDepthStencilState, 0);
        SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_Point()));
    }
}

technique11 LinearFilteringTechnique
{
    pass P0
    {
        SetRasterizerState(gRasterizerState);
       SetDepthStencilState(gDepthStencilState, 0);
       SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_Linear()));
    }
}

technique11 AnisotropicFilteringTechnique
{
    pass P0
    {
        SetRasterizerState(gRasterizerState);
        SetDepthStencilState(gDepthStencilState, 0);
        SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_Anisotropic()));
    }
}

//technique11 DefaultTechnique
//{
//    pass P0
//    {
//        SetVertexShader(CompileShader(vs_5_0, VS() ) );
//        SetGeometryShader(NULL);
//        SetPixelShader(CompileShader(ps_5_0, PS()));
//    }
//}