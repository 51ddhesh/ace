#pragma once

#include <core/types.hpp>
#include <cstdint>
#include <string>

namespace ace {

enum MoveFlags : uint16_t {
    QUIET = 0b0000,
    DOUBLE_PAWN_PUSH = 0b0001,
    KING_CASTLE = 0b0010,
    QUEEN_CASTLE = 0b0011,
    CAPTURE = 0b0100,
    EN_PASSANT = 0b0101,
    PROMO_KNIGHT = 0b1000,
    PROMO_BISHOP = 0b1001,
    PROMO_ROOK = 0b1010,
    PROMO_QUEEN = 0b1011,
    PROMO_CAP_KNIGHT = 0b1100,
    PROMO_CAP_BISHOP = 0b1101,
    PROMO_CAP_ROOK = 0b1110,
    PROMO_CAP_QUEEN = 0b1111
};

class Move {
  private:
    uint16_t data_;

  public:
    constexpr Move() noexcept : data_(0) {}
    constexpr explicit Move(uint16_t d) noexcept : data_(d) {}
    constexpr Move(Square from, Square to, MoveFlags flags = QUIET) noexcept : data_(static_cast<uint16_t>(from | (to << 6) | (flags << 12))) {}

    static constexpr Move none() noexcept { return Move(0); }
    static constexpr Move null() noexcept { return Move(65); }

    [[nodiscard]] constexpr Square from() const noexcept {
        return static_cast<Square>(data_ & 0x3F);
    }

    [[nodiscard]] constexpr Square to() const noexcept {
        return static_cast<Square>((data_ >> 6) & 0x3F);
    }

    [[nodiscard]] constexpr MoveFlags flags() const noexcept {
        return static_cast<MoveFlags>(data_ >> 12);
    }

    [[nodiscard]] constexpr uint16_t raw() const noexcept { return data_; }
    [[nodiscard]] constexpr uint16_t from_to() const noexcept { return data_ & 0x0FFF; }

    [[nodiscard]] constexpr bool is_ok() const noexcept {
        return data_ != 0 and data_ != 65;
    }

    [[nodiscard]] constexpr bool is_capture() const noexcept {
        return (data_ & (CAPTURE << 12)) != 0;
    }

    [[nodiscard]] constexpr bool is_promotion() const noexcept {
        return (data_ & (PROMO_KNIGHT << 12)) != 0;
    }

    [[nodiscard]] constexpr bool is_en_passant() const noexcept {
        return flags() == EN_PASSANT;
    }

    [[nodiscard]] constexpr bool is_castling() const noexcept {
        const uint16_t f = flags();
        return f == KING_CASTLE or f == QUEEN_CASTLE;
    }

    [[nodiscard]] constexpr PieceType promotion_type() const noexcept {
        return static_cast<PieceType>(((data_ >> 12) & 0b0011) + KNIGHT);
    }

    constexpr bool operator==(const Move&) const noexcept = default;

    [[nodiscard]] std::string to_uci() const {
        if (!is_ok()) return "0000";
        std::string s;
        s += static_cast<char>('a' + file_of(from()));
        s += static_cast<char>('1' + rank_of(from()));
        s += static_cast<char>('a' + file_of(to()));
        s += static_cast<char>('1' + rank_of(to()));
        if (is_promotion()) {
            constexpr char promo_chars[] = " nbrq";
            s += promo_chars[promotion_type()];
        }
        return s;
    }
};

} // namespace ace
