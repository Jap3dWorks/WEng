// import GlobalUBO.GlobalUBO;
import Postprocess.Postprocess;

//
public struct PostprocessParamsUBO {
    public float4 param1;
    public float4 param2;
    public float4 param3;
    public float4 param4;
};

[[vk::binding(0,1)]] // binding 0, set 1
public uniform ConstantBuffer<PostprocessParamsUBO> ppcess_params_ubo;


[shader("vertex")]
VSPostprocessOutput vsMain(VSPostprocessInput input)
{
    VSPostprocessOutput output;

    output.position = float4(input.position, 0.f, 1.f);
    output.tex_coord = input.tex_coord;

    return output;
}

[shader("fragment")]
float4 fsMain(VSPostprocessOutput vertIn) : SV_TARGET
{
    float2 uv = vertIn.tex_coord;

    if (uv.x < 0.5) {
        if (uv.y <0.5) {
            return previous.Sample(uv);
        }
        else {
            return mrAO.Sample(uv).g;
        }
    }
    else {
        if (uv.y < 0.5) {
            return normal.Sample(uv);
        }
        else {
            return depth.Sample(uv) * 0.1;
        }
    }
}



