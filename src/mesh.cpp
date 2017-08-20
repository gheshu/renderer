
#define _CRT_SECURE_NO_WARNINGS

#include "myglheaders.h"
#include "mesh.h"
#include "debugmacro.h"
#include "vertexbuffer.h"
#include "glm/glm.hpp"
#include "loadfile.h"
#include "hashstring.h"

template<typename T>
void mesh_layout(int location);

static size_t offset_loc = 0;
static size_t stride = 0;

template<typename T>
void begin_mesh_layout(){
    offset_loc = 0;
    stride = sizeof(T);
}

template<>
inline void mesh_layout<glm::vec2>(int location){
    glEnableVertexAttribArray(location); DebugGL();;
    glVertexAttribPointer(location, 2, GL_FLOAT, GL_FALSE, (int)stride, (void*)offset_loc); DebugGL();;
    offset_loc += sizeof(glm::vec2);
}
template<>
inline void mesh_layout<glm::vec4>(int location){
    glEnableVertexAttribArray(location); DebugGL();;
    glVertexAttribPointer(location, 4, GL_FLOAT, GL_FALSE, (int)stride, (void*)offset_loc); DebugGL();;
    offset_loc += sizeof(glm::vec4);
}
template<>
inline void mesh_layout<half4>(int location){
    glEnableVertexAttribArray(location); DebugGL();;
    glVertexAttribPointer(location, 4, GL_HALF_FLOAT, GL_FALSE, (int)stride, (void*)offset_loc); DebugGL();;
    offset_loc += sizeof(half4);
}

void Mesh::init(){
    num_vertices = 0;
    glGenVertexArrays(1, &vao); DebugGL();;
    glGenBuffers(1, &vbo); DebugGL();;

    glBindVertexArray(vao); DebugGL();;
    glBindBuffer(GL_ARRAY_BUFFER, vbo); DebugGL();;

    begin_mesh_layout<Vertex>();
    mesh_layout<glm::vec4>(0);
    mesh_layout<glm::vec4>(1);
    mesh_layout<glm::vec4>(2);

}

void Mesh::deinit(){
    glDeleteBuffers(1, &vbo); DebugGL();;
    glDeleteVertexArrays(1, &vao); DebugGL();;
    DebugGL();
}

void Mesh::upload(const VertexBuffer& vb){
    glBindVertexArray(vao); DebugGL();;
    glBindBuffer(GL_ARRAY_BUFFER, vbo); DebugGL();;
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vb.size(), vb.data(), GL_STATIC_DRAW); DebugGL();;
    num_vertices = unsigned(vb.size());
}

void Mesh::draw(){
    if(!num_vertices){return;}
    glBindVertexArray(vao); DebugGL();;
    glDrawArrays(GL_TRIANGLES, 0, num_vertices); DebugGL();;
}

void parse_obj(VertexBuffer& out, const char* text){
    const char* p = text;

    struct Face{
        int v1, vt1, vn1, v2, vt2, vn2, v3, vt3, vn3;
    };

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
    std::vector<Face> faces;

    while(*p && p[1]){
        switch(*p){
            case 'v':
            {
                switch(p[1]){
                    case ' ':
                    {
                        glm::vec3 pos;
                        assert(sscanf(p, "v %f %f %f", &pos.x, &pos.y, &pos.z));
                        {
                            positions.push_back(pos);
                        }
                    }
                    break;
                    case 't':
                    {
                        glm::vec2 uv;
                        assert(sscanf(p, "vt %f %f", &uv.x, &uv.y));
                        {
                            uvs.push_back(uv);
                        }
                    }
                    break;
                    case 'n':
                    {
                        glm::vec3 norm;
                        assert(sscanf(p, "vn %f %f %f", &norm.x, &norm.y, &norm.z));
                        {
                            norm = glm::normalize(norm);
                            normals.push_back(norm);
                        }
                    }
                    break;
                }
            }
            break;
            case 'f':
            {
                Face f;
                assert(sscanf(p, "f %i/%i/%i %i/%i/%i %i/%i/%i", &f.v1, &f.vt1, &f.vn1,
                    &f.v2, &f.vt2, &f.vn2,
                    &f.v3, &f.vt3, &f.vn3));
                {
                    faces.push_back(f);
                }

                
            }
        }
        p = nextline(p);
    }

    out.clear();
    for(Face& face : faces){
        const glm::vec3& pa = positions[face.v1 - 1];
        const glm::vec3& pb = positions[face.v2 - 1];
        const glm::vec3& pc = positions[face.v3 - 1];

        const glm::vec3& na = normals[face.vn1 - 1];
        const glm::vec3& nb = normals[face.vn2 - 1];
        const glm::vec3& nc = normals[face.vn3 - 1];

        const glm::vec2& ua = uvs[face.vt1 - 1];
        const glm::vec2& ub = uvs[face.vt2 - 1];
        const glm::vec2& uc = uvs[face.vt3 - 1];

        const glm::vec3 e1 = pb - pa;
        const glm::vec3 e2 = pc - pa;
        const glm::vec2 duv1 = ub - ua;
        const glm::vec2 duv2 = uc - ua;

        glm::vec3 t;
        float f = 1.0f / (duv1.x * duv2.y - duv2.x * duv1.y);

        t.x = f * (duv2.y * e1.x - duv1.y * e2.x);
        t.y = f * (duv2.y * e1.y - duv1.y * e2.y);
        t.z = f * (duv2.y * e1.z - duv1.y * e2.z);
        t = glm::normalize(t);

        int mat = 0;

        out.push_back({
                {pa.x, pa.y, pa.z, ua.x},
                {na.x, na.y, na.z, ua.y},
                {t.x, t.y, t.z, float(mat)}
            });
        out.push_back({
                {pb.x, pb.y, pb.z, ub.x},
                {nb.x, nb.y, nb.z, ub.y},
                {t.x, t.y, t.z, float(mat)}
            });
        out.push_back({
                {pc.x, pc.y, pc.z, uc.x},
                {nc.x, nc.y, nc.z, uc.y},
                {t.x, t.y, t.z, float(mat)}
            });
    }

    assert(out.size() == 3 * faces.size());
}

void MeshStore::load_mesh(Mesh& mesh, unsigned name){
    VertexBuffer* vb = m_vbs[name];

    if(vb){
        mesh.upload(*vb);
        return;
    }

    if(m_vbs.full()){
        vb = m_vbs.reuse_near(name);
    }
    else{
        m_vbs.insert(name, {});
        vb = m_vbs[name];
    }

    const char* filename = HashString(name);
    assert(filename);
    const char* contents = load_file(filename);
    parse_obj(*vb, contents); 
    release_file(contents);
    mesh.upload(*vb);

    printf("[mesh] loaded %s\n", filename);
}

MeshStore g_MeshStore;

HashString::operator Mesh*() const{
    return g_MeshStore[m_hash];
}