//struct ModelViewProjection
//{
//    matrix MVP;
//};
//ConstantBuffer<ModelViewProjection> ModelViewProjectionCB : register(b0);

cbuffer ModelViewProjectionCB : register(b0)
{
    matrix MVP;
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

    OUT.Position = mul(MVP, float4(IN.Position, 1.0f));
    OUT.Color = float4(IN.Color, 1.0f);

    return OUT;
}
