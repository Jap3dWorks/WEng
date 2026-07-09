module GlobalUBO;

// Camera stuffs
public struct CameraUBO {
  public float4x4 proj;
  public float4x4 view;
  public float px;
  public float py;
  public float pz;
  public float pitch;
  public float yaw;
  public float roll;
  public float sx;
  public float sy;
  public float sz;
  public float field_of_view;
  public float near_clipping;
  public float far_clipping;
};

// Lighting Types
// --------------

public struct PointLight {
    public float4 color_radius; // xyz -> color | w -> radius
    public float4 position;
};

public struct DirectionalLight {
    public float4 color;
    public float4 direction;
};

public struct AmbientLight {
    public float4 color;
};

// WLightingUBO
// ------------------

public struct LightingUBO {
    public PointLight point_lights[64];
    public DirectionalLight directional_lights[16];
    public AmbientLight ambient_light;
    public uint point_lights_count;
    public uint directional_lights_count;
};

// Global bindings
// ---------------

[[vk::binding(0, 0)]] // binding, set
public ConstantBuffer<CameraUBO> camera_ubo;

[[vk::binding(1,0)]]
public ConstantBuffer<LightingUBO> lighting_ubo;

