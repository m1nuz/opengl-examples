#define CUBE_VERTICES_NUM       24
#define CUBE_INDICES_NUM        36

typedef struct v3t2n3_t {
    float position[3];
    float texcoord[2];
    float normal[3];
} v3t2n3_t;

const v3t2n3_t cube_vertices[CUBE_VERTICES_NUM] = {
    // front
    {{-1.f, 1.f, 1.f}, {0.f, 1.f}, { 0.f, 0.f, 1.f}},
    {{ 1.f, 1.f, 1.f}, {1.f, 1.f}, { 0.f, 0.f, 1.f}},
    {{ 1.f,-1.f, 1.f}, {1.f, 0.f}, { 0.f, 0.f, 1.f}},
    {{-1.f,-1.f, 1.f}, {0.f, 0.f}, { 0.f, 0.f, 1.f}},
    // back
    {{ 1.f, 1.f,-1.f}, {0.f, 1.f}, { 0.f, 0.f,-1.f}},
    {{-1.f, 1.f,-1.f}, {1.f, 1.f}, { 0.f, 0.f,-1.f}},
    {{-1.f,-1.f,-1.f}, {1.f, 0.f}, { 0.f, 0.f,-1.f}},
    {{ 1.f,-1.f,-1.f}, {0.f, 0.f}, { 0.f, 0.f,-1.f}},
    // top
    {{-1.f, 1.f,-1.f}, {0.f, 1.f}, { 0.f, 1.f, 0.f}},
    {{ 1.f, 1.f,-1.f}, {1.f, 1.f}, { 0.f, 1.f, 0.f}},
    {{ 1.f, 1.f, 1.f}, {1.f, 0.f}, { 0.f, 1.f, 0.f}},
    {{-1.f, 1.f, 1.f}, {0.f, 0.f}, { 0.f, 1.f, 0.f}},
    // bottom
    {{ 1.f,-1.f,-1.f}, {0.f, 1.f}, { 0.f,-1.f, 0.f}},
    {{-1.f,-1.f,-1.f}, {1.f, 1.f}, { 0.f,-1.f, 0.f}},
    {{-1.f,-1.f, 1.f}, {1.f, 0.f}, { 0.f,-1.f, 0.f}},
    {{ 1.f,-1.f, 1.f}, {0.f, 0.f}, { 0.f,-1.f, 0.f}},
    // left
    {{-1.f, 1.f,-1.f}, {0.f, 1.f}, {-1.f, 0.f, 0.f}},
    {{-1.f, 1.f, 1.f}, {1.f, 1.f}, {-1.f, 0.f, 0.f}},
    {{-1.f,-1.f, 1.f}, {1.f, 0.f}, {-1.f, 0.f, 0.f}},
    {{-1.f,-1.f,-1.f}, {0.f, 0.f}, {-1.f, 0.f, 0.f}},
    // right
    {{ 1.f, 1.f, 1.f}, {0.f, 1.f}, { 1.f, 0.f, 0.f}},
    {{ 1.f, 1.f,-1.f}, {1.f, 1.f}, { 1.f, 0.f, 0.f}},
    {{ 1.f,-1.f,-1.f}, {1.f, 0.f}, { 1.f, 0.f, 0.f}},
    {{ 1.f,-1.f, 1.f}, {0.f, 0.f}, { 1.f, 0.f, 0.f}}
};

const unsigned short cube_indices[CUBE_INDICES_NUM] = {
    0, 3, 1,  1, 3, 2,  // front
    4, 7, 5,  5, 7, 6,  // back
    8,11, 9,  9,11,10,  // top
    12,15,13, 13,15,14, // bottom
    16,19,17, 17,19,18, // left
    20,23,21, 21,23,22  // right
};
