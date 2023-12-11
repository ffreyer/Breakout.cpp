#version 330 core

layout (location = 0) in ivec3 v_idx;

flat out ivec3 g_idx;

void main(){
    g_idx = v_idx;
}

