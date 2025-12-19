use std::num::NonZeroU8;

use crate::ffi;
use crate::texture::*;
use crate::vertex::*;

use cgmath::SquareMatrix;
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
    screen_size: (f32, f32),
}

impl Board {
    pub fn new(device: &wgpu::Device, queue: &wgpu::Queue, screen_size: (f32, f32)) -> Self {
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
            position: [-4.0, -4.0, 0.0],
            scale: 80.0,
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
            usage: wgpu::BufferUsages::VERTEX | wgpu::BufferUsages::COPY_DST,
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
            screen_size,
        }
    }

    pub fn update_board(&mut self, queue: &wgpu::Queue) {
        self.state = self.internal.pin_mut().get_board();
        for i in 0..self.state.len() {
            self.instances[i + 1].mask = self.state[i].masked as u32;
            self.instances[i + 1].color = match self.state[i].color {
                ffi::Color::White => 1,
                ffi::Color::Black => 0,
                _ => panic!(),
            };
            self.instances[i + 1].id = match self.state[i].piece {
                ffi::PieceType::None => InstanceType::NONE,
                ffi::PieceType::Pawn => InstanceType::PAWN,
                ffi::PieceType::Knight => InstanceType::KNIGHT,
                ffi::PieceType::Bishop => InstanceType::BISHOP,
                ffi::PieceType::Rook => InstanceType::ROOK,
                ffi::PieceType::Queen => InstanceType::QUEEN,
                ffi::PieceType::King => InstanceType::KING,
                _ => panic!(),
            };
        }
        queue.write_buffer(
            &self.instance_buffer,
            0,
            bytemuck::cast_slice(
                self.instances
                    .iter()
                    .map(|x| x.to_raw())
                    .collect::<Vec<_>>()
                    .as_slice(),
            ),
        );
    }

    pub fn mouse_clicked(&mut self, view_proj: cgmath::Matrix4<f32>) {
        // honestly dont even need to do that, just convert mouse coords to (0, 1) and then use
        // inverse of the view projection matrix

        let mouse_coords = (
            self.mouse_position.0 as f32 / self.screen_size.0,
            self.mouse_position.1 as f32 / self.screen_size.1,
        ); // in range [0.0, 1.0]

        // transform to [-1.0, 1.0]
        let mouse_coords = (mouse_coords.0 * 2.0, mouse_coords.1 * 2.0);
        let mouse_coords = (mouse_coords.0 - 1.0, mouse_coords.1 - 1.0);

        // transform to object space
        let obj_coords = view_proj.invert().unwrap()
            * cgmath::Vector4::new(mouse_coords.0, mouse_coords.1, 1.0, 1.0);

        // find which piece was clicked
        let mut instance: &Instance = &self.instances[0];
        let mut loc = 0;
        for i in &self.instances {
            if i.id == InstanceType::BOARD {
                continue;
            }
            if i.position[0] <= obj_coords.x && obj_coords.x <= i.position[0] + 1.0 {
                if i.position[1] <= obj_coords.y && obj_coords.y <= i.position[1] + 1.0 {
                    instance = i;
                    break;
                }
            }
            loc += 1;
        }
        // board if clicked nothing, else it is a piece
        if instance.id != InstanceType::BOARD {
            if !self.internal.pin_mut().piece_clicked(loc) {
                panic!("game over");
            }
        }
    }

    pub fn update_mouse_position(&mut self, x: f64, y: f64) {
        self.mouse_position = (x, y);
    }
}
