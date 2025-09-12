#include"geometry_util.h"
#include"math_util.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

Mesh *mesh_create(size_t tris_num){
    Mesh *m = malloc(sizeof(Mesh));
    if(!m){
        fprintf(stderr, "Error creating mesh");
        exit(1);
    }
    m->tris = malloc(sizeof(Triangle) * tris_num);
    m->tris_num = tris_num;
    return m;
}

void mesh_init_tris(Mesh *m){
    // TODO implement full init of tris
    vec3 arr_v[] = {{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f}}; // v[3]
                                                                         
    Triangle **arrTri = malloc(sizeof(Triangle) * m->tris_num);
    for(size_t i=0; i<m->tris_num; i++){
        Triangle t;
        for(int j=0; j<3; j++){
            t.p[j] = arr_v[i];
        }
        arrTri[i] = &t;
    }
    m->tris= arrTri;
}

void mesh_init_tris_SampleBox(Mesh *m){
    vec3 arr_v[] = {
        // SOUTH
        { 0.0f, 0.0f, 0.0f},    {0.0f, 1.0f, 0.0f},    {1.0f, 1.0f, 0.0f},
        { 0.0f, 0.0f, 0.0f},    {1.0f, 1.0f, 0.0f},    {1.0f, 0.0f, 0.0f},

        // EAST                                                      
        { 1.0f, 0.0f, 0.0f},    {1.0f, 1.0f, 0.0f},    {1.0f, 1.0f, 1.0f},
        { 1.0f, 0.0f, 0.0f},    {1.0f, 1.0f, 1.0f},    {1.0f, 0.0f, 1.0f},

        // NORTH                                                     
        { 1.0f, 0.0f, 1.0f},    {1.0f, 1.0f, 1.0f},    {0.0f, 1.0f, 1.0f},
        { 1.0f, 0.0f, 1.0f},    {0.0f, 1.0f, 1.0f},    {0.0f, 0.0f, 1.0f},

        // WEST                                                      
        { 0.0f, 0.0f, 1.0f},    {0.0f, 1.0f, 1.0f},    {0.0f, 1.0f, 0.0f},
        { 0.0f, 0.0f, 1.0f},    {0.0f, 1.0f, 0.0f},    {0.0f, 0.0f, 0.0f},

        // TOP                                                       
        { 0.0f, 1.0f, 0.0f},    {0.0f, 1.0f, 1.0f},    {1.0f, 1.0f, 1.0f},
        { 0.0f, 1.0f, 0.0f},    {1.0f, 1.0f, 1.0f},    {1.0f, 1.0f, 0.0f},

        // BOTTOM                                                    
        { 1.0f, 0.0f, 1.0f},    {0.0f, 0.0f, 1.0f},    {0.0f, 0.0f, 0.0f},
        { 1.0f, 0.0f, 1.0f},    {0.0f, 0.0f, 0.0f},    {1.0f, 0.0f, 0.0f},    
    }; 
                                                                         
    Triangle **arrTri = malloc(sizeof(Triangle*) * m->tris_num); //sizeof Triangle pointer for double pointer
    for(size_t i=0; i<m->tris_num; i++){
        arrTri[i] = (Triangle*)malloc(sizeof(Triangle));
        for(int j=0; j<3; j++){
            arrTri[i]->p[j] = arr_v[i*3 + j];
        }
    }
    m->tris= arrTri;
}

void mesh_free(Mesh *mesh){
    if(mesh->tris!=NULL){
        free(mesh->tris);
    }
    free(mesh);
}


Mesh *mesh_create_loadfromObj(){
    Mesh *mesh = malloc(sizeof(Mesh));

    FILE *fptr = NULL;
    char myString[100];
    fptr = fopen("blender_legacyobj.obj", "r");
    if(fptr!=NULL){
        size_t tris_num = 0;
        while(fgets(myString, 100, fptr)){
            if(myString[0]=='f' && myString[1]==' ')
                tris_num++;
        }
        mesh = mesh_create(tris_num);
    }
    fclose(fptr);

    FILE *fptr2 = NULL;
    fptr2 = fopen("blender_legacyobj.obj", "r");
    if(fptr2!=NULL){
        size_t vertices_num = 0;
        while(fgets(myString, 100, fptr2)){
            if(myString[0]=='v' && myString[1]==' ')
                vertices_num++;
        }
        mesh->vert_num=vertices_num;
    }
    fclose(fptr2);

    return mesh;
}

int mesh_loadfrom_Obj(Mesh *mesh){
    
    // FILE *fptr = NULL;
    FILE *fptr2 = NULL;
    vec3 arr_f[mesh->tris_num];
    vec3 arr_v[mesh->vert_num];
    int index_arr_v = 0;
    int index_arr_f = 0;
    char myString[100];
    char *token;

    // fptr = fopen("blender_legacyobj.obj", "r");
    // if(fptr!=NULL){
    //     size_t tris_num = 0;
    //     while(fgets(myString, 100, fptr)){
    //         if(myString[0]=='v' && myString[1]==' ')
    //             tris_num++;
    //     }
    //     mesh = mesh_create(tris_num);
    //     arr_v = malloc(sizeof(vec3) * tris_num);
    // }
    // fclose(fptr);

    fptr2 = fopen("blender_legacyobj.obj", "r");
    if(fptr2!=NULL){
        while(fgets(myString, 100, fptr2)){
            float v_x, v_y, v_z;
            int index_face_v1, index_face_v2, index_face_v3;
            int iTriangleVec = 0;
            const char delimiter[] = " "; // Space character as the delimiter
            // VERTICES //
            // VERTICES //
            if(myString[0]=='v' && myString[1]==' '){
                token = strtok(myString, delimiter);
                while (token != NULL) {
                    if(token[0]=='v'){
                        token = strtok(NULL, delimiter); // Pass NULL for subsequent calls
                    }
                    iTriangleVec++;
                    switch(iTriangleVec){
                        case 1:
                            v_x = atof(token);
                            break;
                        case 2:
                            v_y = atof(token);
                            break;
                        case 3:
                            v_z = atof(token);
                            break;
                    }
                    token = strtok(NULL, delimiter); // Pass NULL for subsequent calls
                }
                vec3 v3 = (vec3){v_x,v_y,v_z}; //{v_x,v_y,v_z} errors should have casted if direct
                arr_v[index_arr_v] = v3;
                index_arr_v++;
                // printf("\n");
                // printf("\n");
                // printf("%f\t", v_x);
                // printf("%f\t", v_y);
                // printf("%f\t", v_z);
            }
            // FACES //
            // FACES //
            if(myString[0]=='f' && myString[1]==' '){
                token = strtok(myString, delimiter);
                while (token != NULL) {
                    if(token[0]=='f'){
                        token = strtok(NULL, delimiter); // Pass NULL for subsequent calls
                    }
                    iTriangleVec++;
                    switch(iTriangleVec){
                        case 1:
                            index_face_v1 = atoi(token);
                            break;
                        case 2:
                            index_face_v2 = atoi(token);
                            break;
                        case 3:
                            index_face_v3 = atoi(token);
                            break;
                    }
                    token = strtok(NULL, delimiter); // Pass NULL for subsequent calls
                }
                arr_f[index_arr_f] = (vec3){index_face_v1, index_face_v2, index_face_v3};
                    index_arr_f++;
                // printf("\n");
                // printf("\n");
                // printf("%i\t", index_face_v1);
                // printf("%i\t", index_face_v2);
                // printf("%i\t", index_face_v3);
            }
        }
    }else{
        printf("Not able to open the file.");
        return 1;
    }
    fclose(fptr2);
    free(token);

    //Create triangles from array
    Triangle **arrTri = malloc(sizeof(Triangle*) * mesh->tris_num); //sizeof Triangle pointer for double pointer
    for(size_t i=0; i<mesh->tris_num; i++){
        arrTri[i] = (Triangle*)malloc(sizeof(Triangle));
        vec3 obj_tri = (vec3)arr_f[i];
        arrTri[i]->p[0] = arr_v[(int)(obj_tri.x)-1];  //x here in obj_tri is first vertex in triangle face    // -1 because of vert face starts at 1 -> while array starts at 0
        arrTri[i]->p[1] = arr_v[(int)(obj_tri.y)-1];  //y here in obj_tri is second vertex in triangle face   // -1 because of vert face starts at 1 -> while array starts at 0
        arrTri[i]->p[2] = arr_v[(int)(obj_tri.z)-1];  //z here in obj_tri is third vertex in triangle face    // -1 because of vert face starts at 1 -> while array starts at 0
    }
    mesh->tris = arrTri;

    return 0;
}











