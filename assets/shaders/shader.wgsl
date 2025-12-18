struct CameraUniform {
    view_proj: mat4x4<f32>
};
@group(1) @binding(0)
var<uniform> camera: CameraUniform;


struct VertexInput {
    @location(0) position: vec3<f32>,
    @location(1) tex_coords: vec2<f32>,
};
struct InstanceInput {
    @location(5) model_matrix0: vec4<f32>,
    @location(6) model_matrix1: vec4<f32>,
    @location(7) model_matrix2: vec4<f32>,
    @location(8) model_matrix3: vec4<f32>,
    @location(9) id: u32,
    @location(10) color: u32,
    @location(11) mask: u32,
};

struct VertexOutput {
    // this is pixel coords
    @builtin(position) clip_position: vec4<f32>,
    @location(0) tex_coords: vec2<f32>,
    @location(1) id: u32,
    @location(2) color: u32,
    @location(3) mask: u32,
};

@vertex
fn vs_main(
    model: VertexInput,
    instance: InstanceInput,
) -> VertexOutput {
    let model_matrix= mat4x4<f32>(
        instance.model_matrix0,
        instance.model_matrix1,
        instance.model_matrix2,
        instance.model_matrix3,
    );
    var out: VertexOutput;
    out.tex_coords = model.tex_coords;
    out.clip_position = camera.view_proj * model_matrix * vec4<f32>(model.position, 1.0);
    out.id = instance.id;
    out.color = instance.color;
    out.mask = instance.mask;
    return out;
}


@group(0) @binding(0)
var pawn_t: texture_2d<f32>;
@group(0) @binding(1)
var knight_t: texture_2d<f32>;
@group(0) @binding(2)
var bishop_t: texture_2d<f32>;
@group(0) @binding(3)
var rook_t: texture_2d<f32>;
@group(0) @binding(4)
var queen_t: texture_2d<f32>;
@group(0) @binding(5)
var king_t: texture_2d<f32>;
@group(0) @binding(6)
var board_t: texture_2d<f32>;
@group(0) @binding(7)
var texture_sampler: sampler;


@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4<f32> {
    var frag_color: vec4<f32> = vec4<f32>(0.0);
    switch (in.id) {
        case 0 {
            break;
        }
        case 1 {
            frag_color = textureSample(pawn_t, texture_sampler, in.tex_coords);
            break;
        }
        case 2 {
            frag_color = textureSample(knight_t, texture_sampler, in.tex_coords);
            break;
        }
        case 3 {
            frag_color = textureSample(bishop_t, texture_sampler, in.tex_coords);
            break;
        }
        case 4 {
            frag_color = textureSample(rook_t, texture_sampler, in.tex_coords);
            break;
        }
        case 5 {
            frag_color = textureSample(queen_t, texture_sampler, in.tex_coords);
            break;
        }
        case 6 {
            frag_color = textureSample(king_t, texture_sampler, in.tex_coords);
            break;
        }
        case 7 {
            frag_color = textureSample(board_t, texture_sampler, in.tex_coords);
            break;
        }
        default {
            discard;
        }
    }
    if (in.color == 0) {
        frag_color = vec4<f32>(1.0 - frag_color.x, 1.0 - frag_color.y, 1.0 - frag_color.z, frag_color.w);
    }
    if (in.mask == 1) {
        let mask_color: vec4<f32> = vec4<f32>(0.188, 0.110, 0.545, 1.0);
        frag_color = mix(mask_color, frag_color, 0.5);
    }
    return frag_color;
}
