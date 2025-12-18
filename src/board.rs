use crate::ffi;
use crate::texture::*;
use crate::vertex::*;

use cxx::UniquePtr;
use wgpu::util::DeviceExt;

pub struct Board {
    pawn: Texture,
    knight: Texture,
    bishop: Texture,
    rook: Texture,
    queen: Texture,
    king: Texture,
    mask: Texture,
    board_texture: Texture,

    pub vertex_buffer: wgpu::Buffer,
    pub index_buffer: wgpu::Buffer,
    pub instance_buffer: wgpu::Buffer,
    pub instances: [[Instance; 8]; 8],
    pub board_instance: Instance,

    state: Vec<ffi::Piece>,

    internal: UniquePtr<ffi::RustBoard>,
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
        let instance_buffer = device.create_buffer_init(&wgpu::util::BufferInitDescriptor {
            label: Some("Instance buffer"),
            contents: bytemuck::cast_slice(&INITIAL_BOARD.map(|row| row.map(|x| x.to_raw()))),
            usage: wgpu::BufferUsages::VERTEX,
        });
        let board_instance = Instance {
            position: [0.0, 0.0, 0.0],
            scale: 8.0,
            id: InstanceType::BOARD,
        };
        Board {
            pawn: Texture::from_bytes(
                device,
                queue,
                include_bytes!("../assets/textures/pawn.png"),
                "pawn.png",
            )
            .unwrap(),
            knight: Texture::from_bytes(
                device,
                queue,
                include_bytes!("../assets/textures/knight.png"),
                "knight.png",
            )
            .unwrap(),
            bishop: Texture::from_bytes(
                device,
                queue,
                include_bytes!("../assets/textures/bishop.png"),
                "bishop.png",
            )
            .unwrap(),
            rook: Texture::from_bytes(
                device,
                queue,
                include_bytes!("../assets/textures/rook.png"),
                "rook.png",
            )
            .unwrap(),
            queen: Texture::from_bytes(
                device,
                queue,
                include_bytes!("../assets/textures/queen.png"),
                "queen.png",
            )
            .unwrap(),
            king: Texture::from_bytes(
                device,
                queue,
                include_bytes!("../assets/textures/king.png"),
                "king.png",
            )
            .unwrap(),
            mask: Texture::from_bytes(
                device,
                queue,
                include_bytes!("../assets/textures/mask.png"),
                "mask.png",
            )
            .unwrap(),
            board_texture: Texture::from_bytes(
                device,
                queue,
                include_bytes!("../assets/textures/board.png"),
                "board.png",
            )
            .unwrap(),

            vertex_buffer,
            index_buffer,
            instance_buffer,
            instances: INITIAL_BOARD,
            board_instance,

            state: vec![],
            internal: ffi::make_board(),
        }
    }
}
