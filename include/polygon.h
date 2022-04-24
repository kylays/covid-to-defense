#ifndef __POLYGON_H__
#define __POLYGON_H__

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "list.h"
#include "vector.h"

/**
 * Computes the area of a polygon.
 * See https://en.wikipedia.org/wiki/Shoelace_formula#Statement.
 *
 * @param polygon the list of vertices that make up the polygon,
 * listed in a counterclockwise direction. There is an edge between
 * each pair of consecutive vertices, plus one between the first and last.
 * @return the area of the polygon
 */
double polygon_area(list_t *polygon);

/**
 * Computes the center of mass of a polygon.
 * See https://en.wikipedia.org/wiki/Centroid#Of_a_polygon.
 *
 * @param polygon the list of vertices that make up the polygon,
 * listed in a counterclockwise direction. There is an edge between
 * each pair of consecutive vertices, plus one between the first and last.
 * @return the centroid of the polygon
 */
vector_t polygon_centroid(list_t *polygon);

/**
 * Translates all vertices in a polygon by a given vector.
 * Note: mutates the original polygon.
 *
 * @param polygon the list of vertices that make up the polygon
 * @param translation the vector to add to each vertex's position
 */
void polygon_translate(list_t *polygon, vector_t translation);

/**
 * Rotates vertices in a polygon by a given angle about a given point.
 * Note: mutates the original polygon.
 *
 * @param polygon the list of vertices that make up the polygon
 * @param angle the angle to rotate the polygon, in radians.
 * A positive angle means counterclockwise.
 * @param point the point to rotate around
 */
void polygon_rotate(list_t *polygon, double angle, vector_t point);

/**
 * @brief Calculates the longest distance from the centroid to a vertex.
 *
 * @param shape the polygon to get the max distance across
 * @return longest distance from the centroid to a vertex of the polygon
 */
double polygon_max_distance_across(list_t *shape);

/**
 * @brief Calculates the shortest distance from the centroid to a vertex.
 *
 * @param shape the polygon to get the max distance across
 * @return shortest distance from the centroid to a vertex of the polygon
 */
double polygon_min_distance_across(list_t *shape);

/**
 * @brief makes a circle shape list of vertices
 *
 * @param position center of circle
 * @param radius circle radius
 * @return list_t* circle shape vertices
 */
list_t *polygon_make_circle(vector_t position, double radius, size_t num_points);

/**
 * @brief Makes a rectangle shape list of vertices.
 * (x1, y1) lower left corner.
 * (x2, y2) upper right corner.
 *
 * @param x1 lower left x
 * @param y1 lower left y
 * @param x2 upper right x
 * @param y2 upper right y
 * @return list_t* rectangle shape vertices
 */
list_t *polygon_make_rectangle(double x1, double y1, double x2, double y2);

/**
 * @brief makes a square
 *
 * @param position center of square
 * @param radius radius of square (center to side distance)
 * @return list_t* square shape vertices
 */
list_t *polygon_make_square(vector_t position, double radius);

/**
 * @brief makes a triangle shape list
 *
 * @param v1 first triangle vertex
 * @param v2 second triangle vertex
 * @param v3 third triangle vertex
 * @return list_t* triangle shape vertices
 */
list_t *polygon_make_triangle(vector_t v1, vector_t v2, vector_t v3);

/**
 * @brief makes a star shape list
 *
 * @param center position
 * @param radius outer radius, inner radius is radius / 2
 * @param points num of points for a "points"-pointed star
 * @return list_t*
 */
list_t *polygon_make_star(vector_t center, double radius, size_t points);

#endif // #ifndef __POLYGON_H__
