#[repr(C)]
#[derive(Copy, Clone, Debug, bytemuck::Pod, bytemuck::Zeroable)]
pub struct Vertex {
    position: [f32; 3],
    tex_coords: [f32; 2],
}
impl Vertex {
    const ATTRIBS: [wgpu::VertexAttribute; 2] =
        wgpu::vertex_attr_array![0 => Float32x3, 1 => Float32x2];

    pub fn desc() -> wgpu::VertexBufferLayout<'static> {
        use std::mem;
        wgpu::VertexBufferLayout {
            array_stride: mem::size_of::<Self>() as wgpu::BufferAddress,
            step_mode: wgpu::VertexStepMode::Vertex,
            attributes: &Self::ATTRIBS,
        }
    }
}
pub const VERTICES: &[Vertex] = &[
    Vertex {
        position: [0.0, 0.0, 0.0],
        tex_coords: [1.0, 1.0],
    },
    Vertex {
        position: [0.1, 0.0, 0.0],
        tex_coords: [0.0, 1.0],
    },
    Vertex {
        position: [0.0, 0.1, 0.0],
        tex_coords: [1.0, 0.0],
    },
    Vertex {
        position: [0.1, 0.1, 0.0],
        tex_coords: [0.0, 0.0],
    },
];

pub const INDICES: &[u16] = &[0, 1, 2, 2, 1, 3];

#[derive(Copy, Clone)]
pub enum InstanceType {
    NONE = 0,
    PAWN = 1,
    KNIGHT = 2,
    BISHOP = 3,
    ROOK = 4,
    QUEEN = 5,
    KING = 6,
    MASK = 7,
    BOARD = 8,
}

pub struct Instance {
    pub position: [f32; 3],
    pub scale: f32,
    pub id: InstanceType,
}

#[repr(C)]
#[derive(Copy, Clone, bytemuck::Pod, bytemuck::Zeroable)]
pub struct InstanceRaw {
    pub model: [[f32; 4]; 4],
    pub id: u32,
}

impl Instance {
    pub fn to_raw(&self) -> InstanceRaw {
        InstanceRaw {
            model: (cgmath::Matrix4::from_translation(self.position.into())
                * cgmath::Matrix4::from_scale(self.scale))
            .into(),
            id: self.id as u32,
        }
    }
}
impl InstanceRaw {
    const ATTRIBS: [wgpu::VertexAttribute; 5] = wgpu::vertex_attr_array![
        5 => Float32x4,
        6 => Float32x4,
        7 => Float32x4,
        8 => Float32x4,
        9 => Uint32
    ];
    pub fn desc() -> wgpu::VertexBufferLayout<'static> {
        use std::mem;
        wgpu::VertexBufferLayout {
            array_stride: mem::size_of::<InstanceRaw>() as wgpu::BufferAddress,
            step_mode: wgpu::VertexStepMode::Instance,
            attributes: &Self::ATTRIBS,
        }
    }
}

use InstanceType::*;
macro_rules! board {
    ([$([$($cell:expr),+]),+]) => {
        {
        let mut y = -1;
        let board = [$({
                let mut x = -1;
                y += 1;
                let row = [$({
                    x += 1;
                    let instance = Instance {
                        position: [x as f32, y as f32, 0.0],
                        scale: 1.0,
                        id: $cell,
                    };
                    instance
                }),+];
                row
            }),+];

        board
        }
    };
}
pub const INITIAL_BOARD: [[Instance; 8]; 8] = board!([
    [ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK],
    [PAWN, PAWN, PAWN, PAWN, PAWN, PAWN, PAWN, PAWN],
    [NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE],
    [NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE],
    [NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE],
    [NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE],
    [PAWN, PAWN, PAWN, PAWN, PAWN, PAWN, PAWN, PAWN],
    [ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK]
]);
