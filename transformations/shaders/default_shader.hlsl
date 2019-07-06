//struct ModelViewProjection
//{
//    matrix MVP;
//};
//ConstantBuffer<ModelViewProjection> ModelViewProjectionCB : register(b0);

cbuffer ViewProjectionCB : register(b0)
{
    // view matrix changes when the camera moves/rotates
    // projection matrix changes when the window is resized
    // only one total exist
    matrix View;
    matrix Projection;
}

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

struct PixelShaderInput
{
	float4 Color : COLOR;
};

float4 PS( PixelShaderInput IN ) : SV_Target
{
    return IN.Color;
}
