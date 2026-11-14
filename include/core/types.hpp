#pragma once

#include <bit>
#include <cassert>
#include <cstdint>

namespace ace {

using Bitboard = uint64_t;
using Key = uint64_t;

constexpr int MAX_PLY = 128;
constexpr int MAX_MOVES = 256;

enum Colour : uint8_t {
    WHITE = 0,
    BLACK = 1,
    COLOUR_NB = 2
};

constexpr Colour operator-(Colour c) noexcept {
    return static_cast<Colour>(c ^ 1);
}

enum PieceType : uint8_t {
    NO_PIECE_TYPE = 0,
    PAWN = 1,
    KNIGHT = 2,
    BISHOP = 3,
    ROOK = 4,
    QUEEN = 5,
    KING = 6,
    PIECE_TYPE_NB = 7
};

enum Piece : uint8_t {
    NO_PIECE = 0,
    W_PAWN = 1,
    W_KNIGHT = 2,
    W_BISHOP = 3,
    W_ROOK = 4,
    W_QUEEN = 5,
    W_KING = 6,
    B_PABN = 9,
    B_KNIGHT = 10,
    B_BISHOP = 11,
    B_ROOK = 12,
    B_QUEEN = 13,
    B_KING = 14,
    PIECE_NB = 16
};

constexpr Piece make_piece(Colour c, PieceType t) noexcept {
    return static_cast<Piece>((c << 3) | t);
}

constexpr PieceType type_of(Piece p) noexcept {
    return static_cast<PieceType>(p & 0b0111);
}

constexpr Colour colour_of(Piece p) noexcept {
    assert(p != NO_PIECE);
    return static_cast<Colour>(p >> 3);
}

enum Square : uint8_t {
    SQ_A1,
    SQ_B1,
    SQ_C1,
    SQ_D1,
    SQ_E1,
    SQ_F1,
    SQ_G1,
    SQ_H1,
    SQ_A2,
    SQ_B2,
    SQ_C2,
    SQ_D2,
    SQ_E2,
    SQ_F2,
    SQ_G2,
    SQ_H2,
    SQ_A3,
    SQ_B3,
    SQ_C3,
    SQ_D3,
    SQ_E3,
    SQ_F3,
    SQ_G3,
    SQ_H3,
    SQ_A4,
    SQ_B4,
    SQ_C4,
    SQ_D4,
    SQ_E4,
    SQ_F4,
    SQ_G4,
    SQ_H4,
    SQ_A5,
    SQ_B5,
    SQ_C5,
    SQ_D5,
    SQ_E5,
    SQ_F5,
    SQ_G5,
    SQ_H5,
    SQ_A6,
    SQ_B6,
    SQ_C6,
    SQ_D6,
    SQ_E6,
    SQ_F6,
    SQ_G6,
    SQ_H6,
    SQ_A7,
    SQ_B7,
    SQ_C7,
    SQ_D7,
    SQ_E7,
    SQ_F7,
    SQ_G7,
    SQ_H7,
    SQ_A8,
    SQ_B8,
    SQ_C8,
    SQ_D8,
    SQ_E8,
    SQ_F8,
    SQ_G8,
    SQ_H8,
    SQ_NONE = 64,
    SQ_NB = 64
};

enum File : uint8_t {
    FILE_A,
    FILE_B,
    FILE_C,
    FILE_D,
    FILE_E,
    FILE_F,
    FILE_G,
    FILE_H,
    FILE_NB
};

enum Rank : uint8_t {
    RANK_1,
    RANK_2,
    RANK_3,
    RANK_4,
    RANK_5,
    RANK_6,
    RANK_7,
    RANK_8,
    RANK_NB
};

enum Direction : int8_t {
    NORTH = 8,
    EAST = 1,
    SOUTH = -8,
    WEST = -1,
    NORTH_EAST = NORTH + EAST,
    SOUTH_EAST = SOUTH + EAST,
    SOUTH_WEST = SOUTH + WEST,
    NORTH_WEST = NORTH + WEST
};

enum CastlingRights : uint8_t {
    NO_CASTLING = 0,
    WHITE_OO = 1 << 0,
    WHITE_OOO = 1 << 1,
    BLACK_OO = 1 << 2,
    BLACK_OOO = 1 << 3,

    KING_SIDE = WHITE_OO | BLACK_OO,
    QUEEN_SIDE = WHITE_OOO | BLACK_OOO,
    WHITE_CASTLING = WHITE_OO | WHITE_OOO,
    BLACK_CASTLING = BLACK_OO | BLACK_OOO,
    ALL_CASTLING = WHITE_CASTLING | BLACK_CASTLING,
    CASTLING_RIGHT_NB = 16
};

constexpr Square make_square(File f, Rank r) noexcept {
    return static_cast<Square>((r << 3) | f);
}

constexpr File file_of(Square s) noexcept {
    return static_cast<File>(s & 7);
}

constexpr Rank rank_of(Square s) noexcept {
    return static_cast<Rank>(s >> 3);
}

constexpr Rank relative_rank(Colour c, Rank r) noexcept {
    return static_cast<Rank>(r ^ (c * 7));
}

constexpr Rank relative_rank(Colour c, Square s) noexcept {
    return relative_rank(c, rank_of(s));
}

constexpr Square flip_rank(Square s) noexcept {
    return static_cast<Square>(s ^ SQ_A8);
}

constexpr Square flip_file(Square s) noexcept {
    return static_cast<Square>(s ^ SQ_H1);
}

constexpr Direction pawn_push(Colour c) noexcept {
    return c == WHITE ? NORTH : SOUTH;
}

constexpr Bitboard sq_bb(Square s) noexcept {
    assert(s < SQ_NB);
    return 1ULL << s;
}

constexpr int popcount(Bitboard b) noexcept {
    return std::popcount(b);
}

constexpr Square lsb(Bitboard b) noexcept {
    assert(b != 0);
    return static_cast<Square>(std::countr_zero(b));
}

constexpr Square msb(Bitboard b) noexcept {
    assert(b != 0);
    return static_cast<Square>(63 ^ std::countl_zero(b));
}

constexpr Square pop_lsb(Bitboard& b) noexcept {
    assert(b != 0);
    const Square s = lsb(b);
    b &= b - 1; // Compiles to BLSR
    return s;
}

constexpr bool more_than_one(Bitboard b) noexcept {
    return (b & (b - 1)) != 0;
}

// DirtyPiece records board mutations per ply for Lazy NNUE Accumulator updates
struct DirtyPiece {
    int dirty_num = 0;
    Piece piece[3]{NO_PIECE, NO_PIECE, NO_PIECE};
    Square from[3]{SQ_NONE, SQ_NONE, SQ_NONE};
    Square to[3]{SQ_NONE, SQ_NONE, SQ_NONE};
};

} // namespace ace
