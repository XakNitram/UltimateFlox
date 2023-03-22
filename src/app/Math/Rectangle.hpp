// Copyright (c) 2023 Tate "Xak Nitram" Mioton

#pragma once

class Rectangle {
public:
    Rectangle() = default;

    Rectangle(Vector center, Vector size);

    explicit Rectangle(Vector size);

    Rectangle(Rectangle const &) = default;

    Rectangle(Rectangle &&) = default;

    Rectangle &operator=(Rectangle const &) = default;

    Rectangle &operator=(Rectangle &&) = default;

    Rectangle operator*(float) const;

    [[nodiscard]] bool contains(Vector point) const;

    [[nodiscard]] bool has_intersection_with(const Rectangle &other) const;
    [[nodiscard]] bool no_intersection_with(const Rectangle &other) const;

    [[nodiscard]] int quadrant(Vector point) const;

    Vector center{0, 0};
    Vector size{0, 0};
};

std::ostream& operator<<(std::ostream& os, Rectangle const&);
