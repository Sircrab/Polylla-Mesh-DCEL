// https://jerryyin.info/geometry-processing-algorithms/half-edge/
// https://doc.cgal.org/latest/Arrangement_on_surface_2/classCGAL_1_1Arrangement__2_1_1Halfedge.html
// https://threejs.org/docs/#examples/en/math/convexhull/HalfEdge.vertex


// half-edge triangulation
/*
Basic operations
    incident_face(e): return the face incident to e
    twin(e): return the twin halfedge of e
    next(e): return the next halfedge of e
    prev(e): return the previous halfedge of e
    origin(e): return the first vertex of halfedge e
    target(e): return the second vertex of halfedge e
Others
    CCW_edge_to_vertex(e): return the next CCW edge incident to v after e
    edge_of_vertex(v): return A edge incident to v
    is_border_face(e): return true if the incent face of e is a border face
    is_interior(e): return true if the incent face of e is an interior face
    is_border_vertex(e): return true if the vertex v is part of the boundary
    faces(): return number of faces
    halfEdges(): Return number of halfedges
    vertices(): Return number of vertices
    get_Triangles(): bitvector of triangles where true if the halfege generate a unique face, false if the face is generated by another halfedge
    get_PointX(int i): return the i-th x coordinate of the triangulation
    get_PointY(int i): return the i-th y coordinate of the triangulation

TODO:
    edge_iterator;
    face_iterator;
    vertex_iterator;
    copy constructor;
    constructor indepent of triangle
*/

#ifndef TRIANGULATION_HPP
#define TRIANGULATION_HPP

#include <array>
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
#include <sstream>
#include <unordered_map>
#include <map>

struct vertex{
    double x;
    double y;
    bool is_border = false; // if the vertex is on the boundary
    int incident_halfedge; // halfedge incident to the vertex, vertex is the origin of the halfedge
};



struct halfEdge {
    int origin; //tail of edge
    int target; //head of edge
    int twin; //opposite halfedge
    int next; //next halfedge of the same face
    int prev; //previous halfedge of the same face
    int face = -1; //face index incident to the halfedge
    int is_border; //1 if the halfedge is on the boundary, 0 otherwise
};

class Triangulation 
{

private:

    typedef std::array<int,3> _triangle; 
    int n_halfedges = 0; //number of halfedges
    int n_faces = 0; //number of faces
    int n_vertices = 0; //number of vertices
    std::vector<vertex> Vertices;
    std::vector<halfEdge> HalfEdges; //list of edges
    //std::vector<char> triangle_flags; //list of edges that generate a unique triangles, 
    std::vector<int> triangle_list; //list of edges that generate a unique triangles, 
    typedef std::pair<int,int> _edge;

    //Read node file in .node format and nodes in point vector
    void read_nodes_from_file(std::string name){
        std::string line;
        std::ifstream nodefile(name);
        double a1, a2, a3, a4;
        
        //std::cout<<"Node file"<<std::endl;
        if (nodefile.is_open())
        {
            nodefile >> n_vertices;
            //std::cout<<pnumber<<std::endl;
            Vertices.reserve(n_vertices);
            std::getline(nodefile, line); //skip the first line
            while (std::getline(nodefile, line))
            {
                std::istringstream(line) >> a1 >> a2 >> a3 >> a4;
                vertex ve;
                ve.x =  a2;
                ve.y =  a3;
                ve.is_border = (a4 == 1) ? true : false;
                Vertices.push_back(ve);
            }  
        }
        else 
            std::cout << "Unable to open node file"; 
        nodefile.close();
    }

    //Read triangle file in .ele format and stores it in faces vector
    std::vector<int> read_triangles_from_file(std::string name){
        std::vector<int> faces;
        std::string line;
        std::ifstream elefile(name);
        int a1, a2, a3, a4;
        
        //std::cout<<"Node file"<<std::endl;
        if (elefile.is_open())
        {
            elefile >> n_faces;
            //std::cout<<pnumber<<std::endl;
            faces.reserve(3*n_faces);
            std::getline(elefile, line); //skip the first line
            while (std::getline(elefile, line))
            {
                std::istringstream(line) >> a1 >> a2 >> a3 >> a4;
                faces.push_back(a2);
                faces.push_back(a3);
                faces.push_back(a4);
                //std::cout<<"Vertex "<<a1<<" "<<v.x<<" "<<v.y<<" "<<v.is_border<<std::endl;
            }
        }
        else 
            std::cout << "Unable to open node file"; 
        elefile.close();

        return faces;
    }

    //Read node file in .node format and nodes in point vector
    std::vector<int>  read_neigh_from_file(std::string name){
        std::vector<int> neighs;
        std::string line;
        std::ifstream neighfile(name);
        int a1, a2, a3, a4;
        
        //std::cout<<"Node file"<<std::endl;
        if (neighfile.is_open())
        {
            neighfile >> n_faces;
            //std::cout<<pnumber<<std::endl;
            neighs.reserve(3*n_faces);
            std::getline(neighfile, line); //skip the first line
            while (std::getline(neighfile, line))
            {
                std::istringstream(line) >> a1 >> a2 >> a3 >> a4;
                neighs.push_back(a2);
                neighs.push_back(a3);
                neighs.push_back(a4);
                
            }
        }
        else 
            std::cout << "Unable to open node file"; 
        neighfile.close();
        return neighs;
    }

    //Generate interior halfedges using faces and neigh vectors
    //also associate each vertex with an incident halfedge
    void construct_interior_halfEdges_from_faces_and_neighs(std::vector<int> faces, std::vector<int> neighs){
        for(std::size_t i = 0; i < n_faces; i++){
            halfEdge he0, he1, he2;
            int index_he0 = i*3+0;
            int index_he1 = i*3+1;
            int index_he2 = i*3+2;
            int v0 = faces.at(3*i+0);
            int v1 = faces.at(3*i+1);
            int v2 = faces.at(3*i+2);
            int n0 = neighs.at(3*i+0);
            int n1 = neighs.at(3*i+1);
            int n2 = neighs.at(3*i+2);
            
            he0.origin = v0;
            he0.target = v1;
            he0.next = index_he1;
            he0.prev = index_he2;
            he0.face = i;
            he0.is_border = (n2 == -1);
            Vertices.at(v0).incident_halfedge = index_he0;
            if(n2 != -1){
                for (std::size_t j = 0; j < 3; j++)
                {
                    if(faces.at(3*n2 + j) == v1 && faces.at(3*n2 + (j + 1)%3) == v0){
                        he0.twin = 3*n2 + j;
                        break;
                    }
                }
            }else
                he0.twin = -1;

            HalfEdges.push_back(he0);
            
            he1.origin = v1;
            he1.target = v2;
            he1.next = index_he2;
            he1.prev = index_he0;
            he1.face = i;
            he1.is_border = (n0 == -1);
            
            Vertices.at(v1).incident_halfedge = index_he1;
            

            if(n0 != -1){
                for (std::size_t j = 0; j < 3; j++)
                {
                    if(faces.at(3*n0 + j) == v2 && faces.at(3*n0 + (j + 1)%3) == v1){
                        he1.twin = 3*n0 + j;
                        break;
                    }
                }
            }else
                he1.twin = -1;
            HalfEdges.push_back(he1);

            he2.origin = v2;
            he2.target = v0;
            he2.next = index_he0;
            he2.prev = index_he1;
            he2.face = i;
            he2.is_border = (n1 == -1);
            Vertices.at(v2).incident_halfedge = index_he2;

            if(n1 != -1)
                for (std::size_t j = 0; j < 3; j++)
                {
                    if(faces.at(3*n1 + j) == v0 && faces.at(3*n1 + (j + 1)%3) == v2){
                        he2.twin = 3*n1 + j;
                        break;
                    }
                }
            else
                he2.twin = -1;
            
            HalfEdges.push_back(he2);
        }

        this->n_halfedges = HalfEdges.size();
    }

    //Generate exterior halfedges
    //Literally calculates the convex hull
    //this takes O(n + k*k), with n the number of interior halfedges and k the number of exterior halfedges
    //optimize to a version n + k
    void construct_exterior_halfEdges(){
        //search interior edges labed as border, generates exterior edges
        //with the origin and target inverted and add at the of HalfEdges vector
        halfEdge he_aux;
        for(std::size_t i = 0; i < this->n_halfedges; i++)
            if(HalfEdges.at(i).is_border){
                he_aux.target = HalfEdges.at(i).origin;
                he_aux.origin = HalfEdges.at(i).target;
                he_aux.is_border = true;
                HalfEdges.at(i).is_border = false;
                he_aux.twin = i;
                HalfEdges.push_back(he_aux);
                HalfEdges.at(i).twin = HalfEdges.size() - 1 ;
            }    
               
        
        //traverse the exterior edges and search their next prev halfedge
        int v_origin, v_target;
        for(std::size_t i = n_halfedges; i < HalfEdges.size(); i++){
            if(HalfEdges.at(i).is_border){
                //search prev of the halfedge
                for(std::size_t j = n_halfedges; j < HalfEdges.size(); j++)
                    if(HalfEdges.at(j).origin == HalfEdges.at(i).target){
                        HalfEdges.at(j).prev = i;
                        break;
                    }
                //search next of each exterior edge   
                for(std::size_t j = n_halfedges; j < HalfEdges.size(); j++)
                    if(HalfEdges.at(i).target == HalfEdges.at(j).origin){
                            HalfEdges.at(i).next = j;
                            break;
                    }
            }
        }

        this->n_halfedges = HalfEdges.size();
    }


    //Generate interior halfedges using a a vector with the faces of the triangulation
    //if an interior half-edge is border, it is mark as border-edge
    //mark border-edges
    void construct_interior_halfEdges_from_faces(std::vector<int> faces){
        auto hash_for_pair = [](const std::pair<int, int>& p) {
            return std::hash<int>{}(p.first) ^ std::hash<int>{}(p.second);
        };
        std::unordered_map<_edge, int, decltype(hash_for_pair)> map_edges(3*this->n_faces, hash_for_pair); //set of edges to calculate the boundary and twin edges
        for(std::size_t i = 0; i < n_faces; i++){
            halfEdge he0, he1, he2;
            int index_he0 = i*3+0;
            int index_he1 = i*3+1;
            int index_he2 = i*3+2;
            int v0 = faces.at(3*i+0);
            int v1 = faces.at(3*i+1);
            int v2 = faces.at(3*i+2);
            
            he0.origin = v0;
            he0.target = v1;
            he0.next = index_he1;
            he0.prev = index_he2;
            he0.face = i;
            he0.is_border = false;
            he0.twin = -1;
            //falta twin
            Vertices.at(v0).incident_halfedge = index_he0;
            
            map_edges[std::make_pair(v0, v1)] = index_he0;
            HalfEdges.push_back(he0);
            
            he1.origin = v1;
            he1.target = v2;
            he1.next = index_he2;
            he1.prev = index_he0;
            he1.face = i;
            he1.is_border = false;
            he1.twin = -1;
            Vertices.at(v1).incident_halfedge = index_he1;
            
            map_edges[std::make_pair(v1, v2)] = index_he1;
            HalfEdges.push_back(he1);

            he2.origin = v2;
            he2.target = v0;
            he2.next = index_he0;
            he2.prev = index_he1;
            he2.face = i;
            he2.is_border = false;
            he2.twin = -1;
            Vertices.at(v2).incident_halfedge = index_he2;

            map_edges[std::make_pair(v2, v0)] = index_he2;            
            HalfEdges.push_back(he2);
        }
        this->n_halfedges = HalfEdges.size();

        //Calculate twin halfedge and boundary halfedges from set_edges
        std::unordered_map<_edge,int, decltype(hash_for_pair)>::iterator it;
        for(std::size_t i = 0; i < HalfEdges.size(); i++){
            //if halfedge has no twin
            if(HalfEdges.at(i).twin == -1){
                _edge twin = std::make_pair(HalfEdges.at(i).target,HalfEdges.at(i).origin);
                it=map_edges.find(twin);
                //if twin is found
                if(it!=map_edges.end()){
                    int index_twin = it->second;
                    HalfEdges.at(i).twin = index_twin;
                    HalfEdges.at(index_twin).twin = i;
                }else{ //if twin is not found and halfedge is on the boundary
                    HalfEdges.at(i).is_border = true;
                    Vertices.at(HalfEdges.at(i).origin).is_border = true;
                    Vertices.at(HalfEdges.at(i).target).is_border = true;
                }
            }
        }
    }

    //Read the mesh from a file in OFF format
    std::vector<int> read_OFFfile(std::string name){
        //Read the OFF file
        std::vector<int> faces;
		std::string line;
		std::ifstream offfile(name);
		double a1, a2, a3;
		std::string tmp;
		if (offfile.is_open())
		{
            //Check first line is a OFF file
			while (std::getline(offfile, line)){ //add check boundary vertices flag
				std::istringstream(line) >> tmp;
				if (tmp[0] != '#' ) //check if first element is a comentary
				{
					if(tmp[0] == 'O' && tmp[1] == 'F' && tmp[2] == 'F') //Check if the format is OFF
                        break;
                    else{
                        std::cout<<"The file is not an OFF file"<<std::endl;
                        exit(0);
                    }
				}
			}

            //Read the number of vertices and faces
            
            while (std::getline(offfile, line)){ //add check boundary vertices flag
				std::istringstream(line) >> tmp;
				if (tmp[0] != '#' ) //check if first element is a comentary
				{
						std::istringstream(line) >> this->n_vertices >> this->n_faces;
			            this->Vertices.reserve(this->n_vertices);
                        faces.reserve(3*this->n_faces);
						break;
				}
			}

            //Read vertices
            int index = 0;
			while (index < n_vertices && std::getline(offfile, line) )
			{
				std::istringstream(line) >> tmp;
				if (tmp[0] != '#' ) //check if first element is a comentary
				{
					std::istringstream(line) >> a1 >> a2 >> a3;
					vertex ve;
                    ve.x =  a1;
                    ve.y =  a2;
                    this->Vertices.push_back(ve);
                    index++;
				}
			}
            //Read faces
            
            int lenght, t1, t2, t3;
            index = 0;
			while (index < n_faces && std::getline(offfile, line) )
			{
				std::istringstream(line) >> tmp;
				if (tmp[0] != '#' ) //check if first element is a comentary
				{
                    std::istringstream(line) >> lenght >> t1 >> t2 >> t3;
                    faces.push_back(t1);
                    faces.push_back(t2);
                    faces.push_back(t3);
                    index++;
				}
			}

		}
		else 
				std::cout << "Unable to open node file"; 
		offfile.close();
        return faces;
    }


public:

    //default constructor
    Triangulation() {}

    //Constructor from file
    Triangulation(std::string node_file, std::string ele_file, std::string neigh_file) {
        std::vector<int> faces;
        std::vector<int> neighs;
        std::cout<<"Reading node file"<<std::endl;
        read_nodes_from_file(node_file);
        //fusionar estos dos métodos
        std::cout<<"Reading ele file"<<std::endl;
        faces = read_triangles_from_file(ele_file);
        std::cout<<"Reading neigh file"<<std::endl;
        neighs = read_neigh_from_file(neigh_file);
        //std::cout<<"Constructing interior halfedges"<<std::endl;
        construct_interior_halfEdges_from_faces_and_neighs(faces, neighs);
        //std::cout<<"Constructing exterior halfedges"<<std::endl;
        construct_exterior_halfEdges();
        //std::cout<<"Constructing triangles"<<std::endl;

        triangle_list.reserve(n_faces);
        for(std::size_t i = 0; i < n_faces; i++)
            triangle_list.push_back(3*i);

    }

    Triangulation(std::string OFF_file){
        std::cout<<"Reading OFF file "<<OFF_file<<std::endl;
        std::vector<int> faces = read_OFFfile(OFF_file);
        construct_interior_halfEdges_from_faces(faces);
        construct_exterior_halfEdges();

        triangle_list.reserve(n_faces);
        for(std::size_t i = 0; i < n_faces; i++)
            triangle_list.push_back(3*i);
    }

    //print the triangulation in pg file format
    void print_pg(std::string file_name){
        std::ofstream file;
        file.open(file_name);
        file<< n_vertices <<"\n";
        file<< n_halfedges <<"\n";
        for(std::size_t i = 0; i < n_vertices; i++){
            vertex v = Vertices.at(i);
            int incident = v.incident_halfedge;
            int curr = incident;
            int twin = HalfEdges.at(curr).twin;
            //search border edges with v as origin
            if(v.is_border){
                while(!HalfEdges.at(twin).is_border){
                    curr = CCW_edge_to_vertex(curr);
                    twin = HalfEdges.at(curr).twin;
                }
            }
            file<<origin(curr)<<" "<<target(curr)<<"\n";
            int nxt = CCW_edge_to_vertex(curr);
            while(nxt != curr){
                file<<origin(nxt)<<" "<<target(nxt)<<"\n";
                nxt = CCW_edge_to_vertex(nxt);
            }
        }
        file.close();
    }

    // Calculates the distante of edge e
    double distance(int e){
        double x1 = Vertices.at(origin(e)).x;
        double y1 = Vertices.at(origin(e)).y;
        double x2 = Vertices.at(target(e)).x;
        double y2 = Vertices.at(target(e)).y;
        return sqrt(pow(x1-x2,2) + pow(y1-y2,2));
    }


    int face_index(int i){
        return HalfEdges.at(i).face;
    }

    //Return triangle of the face incident to edge e
    //Input: e is the edge
    //output: array with the vertices of the triangle
    _triangle incident_face(int e)
    {   
        _triangle face;  
        int nxt = e;
        int init_vertex = origin(nxt);
        int curr_vertex = -1;
        int i = 0;
        while ( curr_vertex != init_vertex )
        {
            nxt = next(nxt);            
            curr_vertex = origin(nxt);
            face.at(i) = curr_vertex;
            i++;
        }
        return face;
    }
    
    //function to check if a triangle is counterclockwise
    //Input: array with the vertices of the triangle
    //Output: true if the triangle is counterclockwise, false otherwise
    bool is_counterclockwise(_triangle tr)
    {
        int v0 = tr.at(0);
        int v1 = tr.at(1);
        int v2 = tr.at(2);
        double area = 0.0;
            //int val = (p2.y - p1.y) * (p3.x - p2.x) - (p2.x - p1.x) * (p3.y - p2.y);
        area = (Vertices.at(v2).x - Vertices.at(v1).x) * (Vertices.at(v1).y - Vertices.at(v0).y) - (Vertices.at(v2).y - Vertices.at(v1).y) * (Vertices.at(v1).x - Vertices.at(v0).x);
        if(area < 0)
            return true;
        else
            return false;
    }

    //Given a edge with vertex origin v, return the next coutnerclockwise edge of v with v as origin
    //Input: e is the edge
    //Output: the next counterclockwise edge of v
    int CCW_edge_to_vertex(int e)
    {
        int twn, nxt;
        if(is_border_face(e)){
            nxt = HalfEdges.at(e).prev;
            twn = HalfEdges.at(nxt).twin;
            return twn;
        }
        nxt = HalfEdges.at(e).next;
        nxt = HalfEdges.at(nxt).next;
        twn = HalfEdges.at(nxt).twin;
        return twn;
    }    

    //Given a edge with vertex origin v, return the prev clockwise edge of v with v as origin
    //Input: e is the edge
    //Output: the prev clockwise edge of v
    int CW_edge_to_vertex(int e)
    {
        int twn, prv, nxt;
        twn = HalfEdges.at(e).twin;
        nxt = HalfEdges.at(twn).next;
        return nxt;
    }    

    //return number of faces
    int faces(){
        return n_faces;
    }

    //Return number of halfedges
    int halfEdges(){
        return n_halfedges;
    }

    //Return number of vertices
    int vertices(){
        return n_vertices;
    }

    //list of triangles where true if the halfege generate a unique face, false if the face is generated by another halfedge
    //Replace by a triangle iterator
    std::vector<int> get_Triangles(){
        return triangle_list;
    }

    double get_PointX(int i){
        return Vertices.at(i).x;
    }

    double get_PointY(int i){
        return Vertices.at(i).y;
    }

    //Calculates the next edge of the face incident to edge e
    //Input: e is the edge
    //Output: the next edge of the face incident to e
    int next(int e){
        return HalfEdges.at(e).next;
    }

    //Calculates the tail vertex of the edge e
    //Input: e is the edge
    //Output: the tail vertex v of the edge e
    int origin(int e)
    {
        return HalfEdges.at(e).origin;
    }


    //Calculates the head vertex of the edge e
    //Input: e is the edge
    //Output: the head vertex v of the edge e
    int target(int e)
    {
        return HalfEdges.at(e).target;
    }

    //Return the twin edge of the edge e
    //Input: e is the edge
    //Output: the twin edge of e
    int twin(int e)
    {
        return HalfEdges.at(e).twin;
    }

    //Return the twin edge of the edge e
    //Input: e is the edge
    //Output: the twin edge of e
    int prev(int e)
    {
        return HalfEdges.at(e).prev;
    }



    //return a edge associate to the node v
    //Input: v is the node
    //Output: the edge associate to the node v
    int edge_of_vertex(int v)
    {
        return Vertices.at(v).incident_halfedge;
    }

    //Input: edge e
    //Output: true if is the face of e is border face
    //        false otherwise
    bool is_border_face(int e)
    {
        return HalfEdges.at(e).is_border;
    }

    // Input: edge e of compressTriangulation
    // Output: true if the edge is an interior face a
    //         false otherwise
    bool is_interior_face(int e)
    {
       return !this->is_border_face(e);
    }

    //Input:vertex v
    //Output: the edge incident to v, wiht v as origin
    bool is_border_vertex(int v)
    {
        return Vertices.at(v).is_border;
    }

};

#endif