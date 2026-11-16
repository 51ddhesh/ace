#pragma once

#include <array>
#include <core/types.hpp>
#include <immintrin.h>

namespace ace {

constexpr Bitboard FILE_ABB = 0x0101010101010101ULL;
constexpr Bitboard FILE_BBB = FILE_ABB << 1;
constexpr Bitboard FILE_CBB = FILE_ABB << 2;
constexpr Bitboard FILE_DBB = FILE_ABB << 3;
constexpr Bitboard FILE_EBB = FILE_ABB << 4;
constexpr Bitboard FILE_FBB = FILE_ABB << 5;
constexpr Bitboard FILE_GBB = FILE_ABB << 6;
constexpr Bitboard FILE_HBB = FILE_ABB << 7;

constexpr Bitboard RANK_1BB = 0x00000000000000FFULL;
constexpr Bitboard RANK_2BB = RANK_1BB << (8 * 1);
constexpr Bitboard RANK_3BB = RANK_1BB << (8 * 2);
constexpr Bitboard RANK_4BB = RANK_1BB << (8 * 3);
constexpr Bitboard RANK_5BB = RANK_1BB << (8 * 4);
constexpr Bitboard RANK_6BB = RANK_1BB << (8 * 5);
constexpr Bitboard RANK_7BB = RANK_1BB << (8 * 6);
constexpr Bitboard RANK_8BB = RANK_1BB << (8 * 7);

template <Direction D>
constexpr Bitboard shift(Bitboard b) noexcept {
    if constexpr (D == NORTH) return b << 8;
    else if constexpr (D == SOUTH)
        return b >> 8;
    else if constexpr (D == NORTH + NORTH)
        return b << 16;
    else if constexpr (D == SOUTH + SOUTH)
        return b >> 16;
    else if constexpr (D == EAST)
        return (b & ~FILE_HBB) << 1;
    else if constexpr (D == WEST)
        return (b & ~FILE_ABB) >> 1;
    else if constexpr (D == NORTH_EAST)
        return (b & ~FILE_HBB) << 9;
    else if constexpr (D == NORTH_WEST)
        return (b & ~FILE_ABB) << 7;
    else if constexpr (D == SOUTH_EAST)
        return (b & ~FILE_HBB) >> 7;
    else if constexpr (D == SOUTH_WEST)
        return (b & ~FILE_ABB) >> 9;
    else
        return 0ULL;
}

template <Colour C>
constexpr Bitboard pawn_attacks_bb(Bitboard b) noexcept {
    if constexpr (C == WHITE) {
        return shift<NORTH_WEST>(b) | shift<NORTH_EAST>(b);
    } else {
        return shift<SOUTH_WEST>(b) | shift<SOUTH_EAST>(b);
    }
}

namespace detail {

consteval auto generate_pawn_attacks() {
    std::array<std::array<Bitboard, SQ_NB>, COLOUR_NB> table{};
    for (int s = 0; s < SQ_NB; s++) {
        const Bitboard b = 1ULL << s;
        table[WHITE][s] = pawn_attacks_bb<WHITE>(b);
        table[BLACK][s] = pawn_attacks_bb<BLACK>(b);
    }
    return table;
}

consteval auto generate_knight_attacks() {
    std::array<Bitboard, SQ_NB> table{};
    for (int s = 0; s < SQ_NB; ++s) {
        const Bitboard b = 1ULL << s;
        Bitboard l1 = (b >> 1) & ~FILE_HBB;
        Bitboard l2 = (b >> 2) & ~(FILE_GBB | FILE_HBB);
        Bitboard r1 = (b << 1) & ~FILE_ABB;
        Bitboard r2 = (b << 2) & ~(FILE_ABB | FILE_BBB);
        Bitboard h1 = l1 | r1;
        Bitboard h2 = l2 | r2;
        table[s] = (h1 << 16) | (h1 >> 16) | (h2 << 8) | (h2 >> 8);
    }
    return table;
}

consteval auto generate_king_attacks() {
    std::array<Bitboard, SQ_NB> table{};
    for (int s = 0; s < SQ_NB; ++s) {
        const Bitboard b = 1ULL << s;
        Bitboard attacks = shift<EAST>(b) | shift<WEST>(b);
        Bitboard full = b | attacks;
        attacks |= shift<NORTH>(full) | shift<SOUTH>(full);
        table[s] = attacks;
    }
    return table;
}

} // namespace detail

inline constexpr auto PAWN_ATTACKS = detail::generate_pawn_attacks();
inline constexpr auto KNIGHT_ATTACKS = detail::generate_knight_attacks();
inline constexpr auto KING_ATTACKS = detail::generate_king_attacks();

struct PextEntry {
    const Bitboard* attacks;
    Bitboard mask;
};

} // namespace ace
