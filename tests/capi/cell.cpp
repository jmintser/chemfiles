// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include "catch.hpp"
#include "helpers.hpp"
#include "chemfiles.h"
#include <cmath>

static bool approx_eq(double A[3][3], double B[3][3]) {
    double eps = 1e-10;
    return
        (fabs(A[0][0] - B[0][0]) < eps) && (fabs(A[0][1] - B[0][1]) < eps) && (fabs(A[0][2] - B[0][2]) < eps) &&
        (fabs(A[1][0] - B[1][0]) < eps) && (fabs(A[1][1] - B[1][1]) < eps) && (fabs(A[1][2] - B[1][2]) < eps) &&
        (fabs(A[2][0] - B[2][0]) < eps) && (fabs(A[2][1] - B[2][1]) < eps) && (fabs(A[2][2] - B[2][2]) < eps);
}

TEST_CASE("chfl_cell") {
    SECTION("Constructors") {
        chfl_vector3d lengths = {2, 3, 4};
        CHFL_CELL* cell = chfl_cell(lengths);
        REQUIRE(cell);

        chfl_vector3d data = {0};
        CHECK_STATUS(chfl_cell_lengths(cell, data));
        CHECK(data[0] == 2);
        CHECK(data[1] == 3);
        CHECK(data[2] == 4);

        CHECK_STATUS(chfl_cell_angles(cell, data));
        CHECK(data[0] == 90);
        CHECK(data[1] == 90);
        CHECK(data[2] == 90);

        chfl_free(cell);

        lengths[0] = 20; lengths[1] = 21; lengths[2] = 22;
        chfl_vector3d angles = {90, 100, 120};
        cell = chfl_cell_triclinic(lengths, angles);
        REQUIRE(cell);

        CHECK_STATUS(chfl_cell_lengths(cell, data));
        CHECK(data[0] == 20);
        CHECK(data[1] == 21);
        CHECK(data[2] == 22);

        CHECK_STATUS(chfl_cell_angles(cell, data));
        CHECK(data[0] == 90);
        CHECK(data[1] == 100);
        CHECK(data[2] == 120);

        chfl_free(cell);

        // Check that a call to chfl_cell_triclinic always gives a triclinic
        // cell, even with all angles equal to 90°
        angles[0] = 90; angles[1] = 90; angles[2] = 90;
        cell = chfl_cell_triclinic(lengths, angles);
        REQUIRE(cell);

        chfl_cellshape shape;
        CHECK_STATUS(chfl_cell_shape(cell, &shape));
        CHECK(shape == CHFL_CELL_TRICLINIC);

        chfl_free(cell);
    }

    SECTION("Constructors errors") {
        chfl_vector3d dummy = {0, 0, 0};
        fail_next_allocation();
        CHECK(chfl_cell(dummy) == nullptr);

        fail_next_allocation();
        CHECK(chfl_cell_triclinic(dummy, dummy) == nullptr);

        CHFL_CELL* cell = chfl_cell(dummy);
        REQUIRE(cell);

        fail_next_allocation();
        CHECK(chfl_cell_copy(cell) == nullptr);

        CHFL_FRAME* frame = chfl_frame();
        REQUIRE(frame);

        fail_next_allocation();
        CHECK(chfl_cell_from_frame(frame) == nullptr);

        chfl_free(cell);
        chfl_free(frame);
    }

    SECTION("copy") {
        chfl_vector3d lengths = {2, 2, 2};
        CHFL_CELL* cell = chfl_cell(lengths);
        REQUIRE(cell);

        CHFL_CELL* copy = chfl_cell_copy(cell);
        REQUIRE(copy);

        double volume = 0;
        CHECK_STATUS(chfl_cell_volume(cell, &volume));
        CHECK(volume == 8);

        CHECK_STATUS(chfl_cell_volume(copy, &volume));
        CHECK(volume == 8);

        chfl_vector3d new_lengths = {3, 3, 3};
        CHECK_STATUS(chfl_cell_set_lengths(cell, new_lengths));

        CHECK_STATUS(chfl_cell_volume(cell, &volume));
        CHECK(volume == 27);

        CHECK_STATUS(chfl_cell_volume(copy, &volume));
        CHECK(volume == 8);

        chfl_free(copy);
        chfl_free(cell);
    }

    SECTION("Length") {
        chfl_vector3d lengths = {2, 3, 4};
        CHFL_CELL* cell = chfl_cell(lengths);
        REQUIRE(cell);

        chfl_vector3d data = {0};
        CHECK_STATUS(chfl_cell_lengths(cell, data));
        CHECK(data[0] == 2);
        CHECK(data[1] == 3);
        CHECK(data[2] == 4);

        lengths[0] = 10; lengths[1] = 20; lengths[2] = 30;
        CHECK_STATUS(chfl_cell_set_lengths(cell, lengths));
        CHECK_STATUS(chfl_cell_lengths(cell, data));
        CHECK(data[0] == 10);
        CHECK(data[1] == 20);
        CHECK(data[2] == 30);

        chfl_free(cell);
    }

    SECTION("Angles") {
        chfl_vector3d lengths = {2, 3, 4};
        CHFL_CELL* cell = chfl_cell(lengths);
        REQUIRE(cell);

        chfl_vector3d data = {0};
        CHECK_STATUS(chfl_cell_angles(cell, data));
        CHECK(data[0] == 90);
        CHECK(data[1] == 90);
        CHECK(data[2] == 90);

        chfl_vector3d angles = {80, 89, 100};
        // Setting an orthorhombic cell angles is an error
        CHECK(chfl_cell_set_angles(cell, angles) != CHFL_SUCCESS);

        CHECK_STATUS(chfl_cell_set_shape(cell, CHFL_CELL_TRICLINIC));

        CHECK_STATUS(chfl_cell_set_angles(cell, angles));
        CHECK_STATUS(chfl_cell_angles(cell, data));
        CHECK(data[0] == 80);
        CHECK(data[1] == 89);
        CHECK(data[2] == 100);

        chfl_free(cell);
    }

    SECTION("Volume") {
        chfl_vector3d lengths = {2, 3, 4};
        CHFL_CELL* cell = chfl_cell(lengths);
        REQUIRE(cell);

        double volume = 0;
        CHECK_STATUS(chfl_cell_volume(cell, &volume));
        CHECK(volume == 2 * 3 * 4);

        chfl_free(cell);
    }

    SECTION("Matrix") {
        chfl_vector3d lengths = {10, 20, 30};
        CHFL_CELL* cell = chfl_cell(lengths);
        REQUIRE(cell);

        chfl_vector3d expected[3] = {{10, 0, 0}, {0, 20, 0}, {0, 0, 30}};
        chfl_vector3d matrix[3];
        CHECK_STATUS(chfl_cell_matrix(cell, matrix));
        CHECK(approx_eq(expected, matrix));

        chfl_free(cell);
    }

    SECTION("Shape") {
        chfl_vector3d lengths = {2, 3, 4};
        CHFL_CELL* cell = chfl_cell(lengths);
        REQUIRE(cell);

        chfl_cellshape shape;
        CHECK_STATUS(chfl_cell_shape(cell, &shape));
        CHECK(shape == CHFL_CELL_ORTHORHOMBIC);

        CHECK_STATUS(chfl_cell_set_shape(cell, CHFL_CELL_TRICLINIC));
        CHECK_STATUS(chfl_cell_shape(cell, &shape));
        CHECK(shape == CHFL_CELL_TRICLINIC);

        lengths[0] = 0; lengths[1] = 0; lengths[2] = 0;
        CHECK_STATUS(chfl_cell_set_lengths(cell, lengths));
        CHECK_STATUS(chfl_cell_set_shape(cell, CHFL_CELL_INFINITE));
        CHECK_STATUS(chfl_cell_shape(cell, &shape));
        CHECK(shape == CHFL_CELL_INFINITE);

        chfl_free(cell);
    }

    SECTION("Wrap") {
        chfl_vector3d lengths = {2, 3, 4};
        CHFL_CELL* cell = chfl_cell(lengths);
        REQUIRE(cell);

        chfl_vector3d vector = {0.8, 1.7, -6};
        CHECK_STATUS(chfl_cell_wrap(cell, vector));
        CHECK(vector[0] == 0.8);
        CHECK(vector[1] == -1.3);
        CHECK(vector[2] == 2);

        chfl_free(cell);
    }
}
