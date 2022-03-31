/* Polygon mesh metrics functions*/
#ifndef METRICS_HPP
#define METRICS_HPP

#include "triangulation.hpp"
#include "utils.hpp"
#include <vector>
#include <climits>
#include <algorithm>

//Frontier edges, terminal edges, border terminal edge, area, perimetro, ratio, arista minima promedio, circunferencia circunscrita (voronoi)
//Cantidad de triags por poligono (min, max, avg), 
//json for output


class Metrics 
{
    private:
        double max_area = 0, min_area = __DBL_MAX__, avg_area = 0;
        double max_perimeter = 0, min_perimeter = __DBL_MAX__, avg_perimeter = 0;
        double max_apr = 0, min_apr = __DBL_MAX__, avg_apr = 0;
        double min_angle = 360.0, max_angle = 0;
        double min_pointDistance = __DBL_MAX__, max_pointDistance = 0;
        double min_radius = __DBL_MAX__, max_radius = 0, avg_radius = 0;


        void calc_stats(const Triangulation& mesh) {
            double areaSum = 0.0;
            double perimSum = 0.0;
            double aprSum = 0.0;
            for(auto& triIdx: mesh.triangle_list) {
                const halfEdge firstEdge = mesh.HalfEdges[triIdx];
                const halfEdge secondEdge = mesh.HalfEdges[firstEdge.next];
                const halfEdge thirdEdge = mesh.HalfEdges[secondEdge.next];
                const double area = calc_area(firstEdge, secondEdge, mesh);
                const double perimeter = calc_perimeter(firstEdge, secondEdge, thirdEdge, mesh);
                areaSum += area;
                perimSum += perimeter;
                double apr = (2*M_PI*area)/(perimeter * perimeter));
                max_apr = std::max(max_apr, apr);
                min_apr = std::min(min_apr, apr);
                max_area = std::max(max_area, area);
                min_area = std::max(min_area, area);
                max_perimeter = std::max(max_perimeter, perimeter);
                min_perimeter = std::min(min_perimeter, perimeter);
            }
            avg_area = areaSum / mesh.n_faces() * 1.0;
            avg_apr = aprSum / mesh.n_faces() * 1.0;
            avg_perimeter = perimSum / (mesh.n_halfedges * 1.0 / 2.0); //twice as many half edges as edges
        }

        double calc_area(const halfEdge& firstEdge, const halfEdge& secondEdge, const Triangulation& mesh) {
            const halfEdge twinEdge = mesh.HalfEdges[firstEdge.twin];
            const double v1x = mesh.Vertices[twinEdge.target].x - mesh.Vertices[twinEdge.origin].x;
            const double v1y = mesh.Vertices[twinEdge.target].y - mesh.Vertices[twinEdge.origin].y;
            const double v2x = mesh.Vertices[secondEdge.target].x - mesh.Vertices[secondEdge.origin].x;
            const double v2y = mesh.Vertices[secondEdge.target].y - mesh.Vertices[secondEdge.origin].y;
            return abs(cross(v1x, v1y, v2x, v2y) / 2.0);
        }

        void calc_perim(const halfEdge& firstEdge, const halfEdge& secondEdge, const halfEdge& thirdEdge, const Triangulation& mesh) {
                double perimiter = 0.0;
                perimiter += module(
                    mesh.Vertices[firstEdge.target].x - mesh.Vertices[firstEdge.origin].x,
                    mesh.Vertices[firstEdge.target].y - mesh.Vertices[firstEdge.origin].y
                );
                perimiter += module(
                    mesh.Vertices[secondEdge.target].x - mesh.Vertices[secondEdge.origin].x,
                    mesh.Vertices[secondEdge.target].y - mesh.Vertices[secondEdge.origin].y
                );
                perimiter += module(
                    mesh.Vertices[thirdEdge.target].x - mesh.Vertices[thirdEdge.origin].x,
                    mesh.Vertices[thirdEdge.target].y - mesh.Vertices[thirdEdge.origin].y
                );
                return perimeter;
            }
            
            
        }

    public:
        Metrics () {};
        Metrics(const Triangulation &mesh) {
            calc_stats(mesh);
        };
};

#endif