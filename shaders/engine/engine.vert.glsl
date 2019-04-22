#version 330 core

struct VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    mat3 TBN;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
};