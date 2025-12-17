project(assets)

set(ASSETS
    shaders/fragment.frag
    shaders/vertex.vert

    textures/bishop.png
    textures/board.png
    textures/king.png
    textures/knight.png
    textures/mask.png
    textures/pawn.png
    textures/pawn_promo.png
    textures/queen.png
    textures/rook.png
)

add_custom_target(chess_assets ALL)

foreach(asset ${ASSETS})
add_custom_command(
        TARGET chess_assets
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/${asset} ${CMAKE_CURRENT_BINARY_DIR}/${asset}
    )

endforeach(asset)
