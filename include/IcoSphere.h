#pragma once
#include <math/vec3.h>

#include <map>
#include <vector>

class IcoSphere {
public:
    using Index = uint16_t;

    struct Triangle {
        Index vertex[3];
    };

    using TriangleList = std::vector<Triangle>;
    using VertexList = std::vector<filament::math::float3>;
    using IndexedMesh = std::pair<VertexList, TriangleList>;

    explicit IcoSphere(size_t subdivisions);

    IndexedMesh const& getMesh() const { return mMesh; }
    VertexList const& getVertices() const { return mMesh.first; }
    TriangleList const& getIndices() const { return mMesh.second; }

private:
    static const IcoSphere::VertexList sVertices;
    static const IcoSphere::TriangleList sTriangles;

    using Lookup = std::map<std::pair<Index, Index>, Index>;
    Index vertex_for_edge(Lookup& lookup, VertexList& vertices, Index first, Index second);
    TriangleList subdivide(VertexList& vertices, TriangleList const& triangles);
    IndexedMesh make_icosphere(int subdivisions);

    IndexedMesh mMesh;
};

