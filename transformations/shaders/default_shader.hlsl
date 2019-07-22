//struct ModelViewProjection
//{
//    matrix MVP;
//};
//ConstantBuffer<ModelViewProjection> ModelViewProjectionCB : register(b0);

// described using an inline descriptor CB
cbuffer ViewProjectionCB : register(b0)
{
    // view matrix changes when the camera moves/rotates
    // projection matrix changes when the window is resized
    // only one total exist
    matrix View;
    matrix Projection;
}

// Descriptor Table with a range containing CBV
cbuffer ModelCB : register(b1)
{
    // world matrix of an object changes when it moves/rotates/scales
    // one for each model exist
    matrix Model;
}

struct VertexPosColor
{
    float3 Position : POSITION;
    float3 Color : COLOR;
};

struct VertexShaderOutput
{
    float4 Color : COLOR;
    float4 Position : SV_Position;
};

VertexShaderOutput VS(VertexPosColor IN)
{
    VertexShaderOutput OUT;

    matrix mvp = mul(mul(Model, View), Projection);
    OUT.Position = mul(float4(IN.Position, 1.0f), mvp);
    // The vertex is now in Homogeneous clip space.

    OUT.Color = float4(IN.Color, 1.0f);
    return OUT;
}

VertexShaderOutput VS_NoProj(VertexPosColor IN)
{
    VertexShaderOutput OUT;

    OUT.Position = float4(IN.Position, 1.0f);
    OUT.Color = float4(IN.Color, 1.0f);

    return OUT;
}

cbuffer srtCB : register(b2)
{
    matrix Scaling;
    matrix Rotation;
    matrix Translation;
}

VertexShaderOutput VS_Outline(VertexPosColor IN)
{
    VertexShaderOutput OUT;

    matrix sr = mul(Scaling, Rotation);
    matrix Model_SRT = mul(sr, Translation);
    matrix mvp = mul(mul(Model_SRT, View), Projection);
    OUT.Position = mul(float4(IN.Position, 1.0f), mvp);
    // The vertex is now in Homogeneous clip space.

    OUT.Color = float4(IN.Color, 1.0f);
    return OUT;
}

struct PixelShaderInput
{
    float4 Color : COLOR;
};

float4 PS(PixelShaderInput IN) : SV_Target
{
    return IN.Color;
}

float4 PS_Outline(PixelShaderInput IN) : SV_Target
{
    return float4(1.0f, 0.0f, 0.0f, 1.0f);
}
