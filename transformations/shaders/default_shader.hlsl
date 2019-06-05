//struct ModelViewProjection
//{
//    matrix MVP;
//};
//ConstantBuffer<ModelViewProjection> ModelViewProjectionCB : register(b0);

cbuffer ModelViewProjectionCB : register(b0)
{
    // combined world (model), view, projection matrices used to transform a point from local space to homogeneous clip space

    // world matrix of an object changes when it moves/rotates/scales
    // view matrix changes when the camera moves/rotates
    // projection matrix changes when the window is resized
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

struct PixelShaderInput
{
	float4 Color    : COLOR;
};

float4 PS( PixelShaderInput IN ) : SV_Target
{
    return IN.Color;
}
