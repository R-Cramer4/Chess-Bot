use std::num::NonZeroU8;

use crate::ffi;
use crate::texture::*;
use crate::vertex::*;

use cxx::UniquePtr;
use wgpu::util::DeviceExt;

macro_rules! make_texture_bind_group_layout_entry {
    ($x:expr) => {
        wgpu::BindGroupLayoutEntry {
            binding: $x,
            visibility: wgpu::ShaderStages::FRAGMENT,
            ty: wgpu::BindingType::Texture {
                sample_type: wgpu::TextureSampleType::Float { filterable: true },
                view_dimension: wgpu::TextureViewDimension::D2,
                multisampled: false,
            },
            count: None,
        }
    };
}
macro_rules! make_texture_bind_group_entry {
    ($x:expr, $y:expr) => {
        wgpu::BindGroupEntry {
            binding: $x,
            resource: wgpu::BindingResource::TextureView(&$y.view),
        }
    };
}

pub struct Board {
    pawn: Texture,
    knight: Texture,
    bishop: Texture,
    rook: Texture,
    queen: Texture,
    king: Texture,
    board_texture: Texture,
    pub texture_bind_group: wgpu::BindGroup,
    pub texture_bind_group_layout: wgpu::BindGroupLayout,

    pub vertex_buffer: wgpu::Buffer,
    pub index_buffer: wgpu::Buffer,
    pub instance_buffer: wgpu::Buffer,
    pub instances: Vec<Instance>,

    state: Vec<ffi::Piece>,

    internal: UniquePtr<ffi::RustBoard>,
    mouse_position: (f64, f64),
}

impl Board {
    pub fn new(device: &wgpu::Device, queue: &wgpu::Queue) -> Self {
        let vertex_buffer = device.create_buffer_init(&wgpu::util::BufferInitDescriptor {
            label: Some("Vertex buffer"),
            contents: bytemuck::cast_slice(VERTICES),
            usage: wgpu::BufferUsages::VERTEX,
        });
        let index_buffer = device.create_buffer_init(&wgpu::util::BufferInitDescriptor {
            label: Some("Index buffer"),
            contents: bytemuck::cast_slice(INDICES),
            usage: wgpu::BufferUsages::INDEX,
        });
        let mut instances: Vec<Instance> = vec![Instance {
            position: [0.0, 0.0, 0.0],
            scale: 20.0,
            id: InstanceType::BOARD,
            color: 1,
            mask: 0,
        }];
        for b in INITIAL_BOARD {
            for i in b {
                instances.push(i);
            }
        }
        let instance_buffer = device.create_buffer_init(&wgpu::util::BufferInitDescriptor {
            label: Some("Instance buffer"),
            contents: bytemuck::cast_slice(
                instances
                    .iter()
                    .map(|x| x.to_raw())
                    .collect::<Vec<_>>()
                    .as_slice(),
            ),
            usage: wgpu::BufferUsages::VERTEX,
        });

        let pawn = Texture::from_bytes(
            device,
            queue,
            include_bytes!("../assets/textures/pawn.png"),
            "pawn.png",
        )
        .unwrap();
        let knight = Texture::from_bytes(
            device,
            queue,
            include_bytes!("../assets/textures/knight.png"),
            "knight.png",
        )
        .unwrap();
        let bishop = Texture::from_bytes(
            device,
            queue,
            include_bytes!("../assets/textures/bishop.png"),
            "bishop.png",
        )
        .unwrap();
        let rook = Texture::from_bytes(
            device,
            queue,
            include_bytes!("../assets/textures/rook.png"),
            "rook.png",
        )
        .unwrap();
        let queen = Texture::from_bytes(
            device,
            queue,
            include_bytes!("../assets/textures/queen.png"),
            "queen.png",
        )
        .unwrap();
        let king = Texture::from_bytes(
            device,
            queue,
            include_bytes!("../assets/textures/king.png"),
            "king.png",
        )
        .unwrap();
        let board_texture = Texture::from_bytes(
            device,
            queue,
            include_bytes!("../assets/textures/board.png"),
            "board.png",
        )
        .unwrap();

        let texture_bind_group_layout =
            device.create_bind_group_layout(&wgpu::BindGroupLayoutDescriptor {
                entries: &[
                    make_texture_bind_group_layout_entry!(0),
                    make_texture_bind_group_layout_entry!(1),
                    make_texture_bind_group_layout_entry!(2),
                    make_texture_bind_group_layout_entry!(3),
                    make_texture_bind_group_layout_entry!(4),
                    make_texture_bind_group_layout_entry!(5),
                    make_texture_bind_group_layout_entry!(6),
                    wgpu::BindGroupLayoutEntry {
                        binding: 7,
                        visibility: wgpu::ShaderStages::FRAGMENT,
                        ty: wgpu::BindingType::Sampler(wgpu::SamplerBindingType::Filtering),
                        count: None,
                    },
                ],
                label: Some("Texture bind group layout"),
            });
        let texture_bind_group = device.create_bind_group(&wgpu::BindGroupDescriptor {
            layout: &texture_bind_group_layout,
            entries: &[
                make_texture_bind_group_entry!(0, pawn),
                make_texture_bind_group_entry!(1, knight),
                make_texture_bind_group_entry!(2, bishop),
                make_texture_bind_group_entry!(3, rook),
                make_texture_bind_group_entry!(4, queen),
                make_texture_bind_group_entry!(5, king),
                make_texture_bind_group_entry!(6, board_texture),
                wgpu::BindGroupEntry {
                    binding: 7,
                    resource: wgpu::BindingResource::Sampler(&pawn.sampler),
                },
            ],
            label: Some("Texture bind group"),
        });

        Board {
            pawn,
            knight,
            bishop,
            rook,
            queen,
            king,
            board_texture,
            texture_bind_group,
            texture_bind_group_layout,

            vertex_buffer,
            index_buffer,
            instance_buffer,
            instances,

            state: vec![],
            internal: ffi::make_board(),
            mouse_position: (0.0, 0.0),
        }
    }

    pub fn update_board(&self) {}

    pub fn mouse_clicked(&self) {
        // To check what object we clicked
        // write object id to frame buffer
        // map buffer to cpu
        // lookup mouse position in frame buffer to get object id

        // because the camera never moves, we only need to do that mapping once
        // and then reference the static buffer
        // because this is cpu limited (thanks chess engine)
        // we can write to an unused buffer and it should be fine
    }

    pub fn update_mouse_position(&mut self, x: f64, y: f64) {
        self.mouse_position = (x, y);
    }
}
