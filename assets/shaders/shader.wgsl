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
};

struct VertexOutput {
    // this is pixel coords
    @builtin(position) clip_position: vec4<f32>,
    @location(0) tex_coords: vec2<f32>,
    @location(1) id: u32,
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
var mask_t: texture_2d<f32>;
@group(0) @binding(7)
var board_t: texture_2d<f32>;
@group(0) @binding(8)
var texture_sampler: sampler;


@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4<f32> {
    switch (in.id) {
        case 0 {
            discard;
        }
        case 1 {
            return textureSample(pawn_t, texture_sampler, in.tex_coords);
        }
        case 2 {
            return textureSample(knight_t, texture_sampler, in.tex_coords);
        }
        case 3 {
            return textureSample(bishop_t, texture_sampler, in.tex_coords);
        }
        case 4 {
            return textureSample(rook_t, texture_sampler, in.tex_coords);
        }
        case 5 {
            return textureSample(queen_t, texture_sampler, in.tex_coords);
        }
        case 6 {
            return textureSample(king_t, texture_sampler, in.tex_coords);
        }
        case 7 {
            return textureSample(mask_t, texture_sampler, in.tex_coords);
        }
        case 8 {
            return textureSample(board_t, texture_sampler, in.tex_coords);
        }
        default {
            discard;
        }
    }
}
