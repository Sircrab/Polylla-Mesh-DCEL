/* Polygon mesh metrics functions*/
#ifndef METRICS_HPP
#define METRICS_HPP

#include "polylla.hpp"
#include "utils.hpp"
#include <vector>
#include <climits>
#include <algorithm>

class Metrics 
{
    private:
        double max_area = 0, min_area = __DBL_MAX__, avg_area = 0;
        double max_perimeter = 0, min_perimeter = __DBL_MAX__, avg_perimeter = 0;
        double max_apr = 0, min_apr = __DBL_MAX__, avg_apr = 0;
        double min_angle = 360.0, max_angle = 0;
        double min_pointDistance = __DBL_MAX__, max_pointDistance = 0;
        double min_radius = __DBL_MAX__, max_radius = 0, avg_radius = 0;

        void calc_areas(Triangulation &mesh) {
            double areaSum = 0.0;
            for(auto& triIdx : mesh.triangle_list) {
                const halfEdge triEdge = mesh.HalfEdges[triIdx];
                const halfEdge nextEdge = mesh.HalfEdges[triEdge.next];
                const halfEdge twinEdge = mesh.HalfEdges[triEdge.twin];
                const double v1x = mesh.Vertices[twinEdge.target].x - mesh.Vertices[twinEdge.origin].x;
                const double v1y = mesh.Vertices[twinEdge.target].y - mesh.Vertices[twinEdge.origin].y;
                const double v2x = mesh.Vertices[nextEdge.target].x - mesh.Vertices[nextEdge.origin].x;
                const double v2y = mesh.Vertices[nextEdge.target].x - mesh.Vertices[nextEdge.origin].x;
                const double triArea = cross(v1x, v1y, v2x, v2y) / 2.0;
                max_area = std::max(max_area, triArea);
                min_area = std::min(min_area, triArea);
                areaSum += triArea;
            }
            areaSum /= mesh.faces();
        }

    public:
        Metrics () {};
        Metrics(Triangulation &mesh) {
            calc_areas(mesh);
        };
};

#endif