//
//  main.cpp
//  OpenGLTest
//
//  Created by 김성민 on 2020/03/07.
//  Copyright © 2020 Sung Min Kim. All rights reserved.
//

#define GL_SILENCE_DEPRECATION

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <stdbool.h>
#include <GL/glew.h>
#include "/Users/sungminkim/Desktop/OpenGLTest/GLUT.framework/Headers/glut.h"
#include "/Users/sungminkim/Desktop/OpenGLTest/OpenGLTest/LoadShaders.h"
#include "/Users/sungminkim/Desktop/OpenGLTest/OpenGLTest/loadobj.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <time.h>

#define STB_IMAGE_IMPLEMENTATION
#include "/Users/sungminkim/Desktop/OpenGLTest/OpenGLTest/stb_image.h"

#define UVAR(name) glGetUniformLocation(program, name)
#define MAP_FIND(map_obj, item)\
    ((map_obj).find(item) != (map_obj).end())

#define IS_MIPMAP(flag)\
    ((flag) == GL_LINEAR_MIPMAP_LINEAR || \
     (flag) == GL_LINEAR_MIPMAP_NEAREST || \
     (flag) == GL_NEAREST_MIPMAP_LINEAR || \
     (flag) == GL_NEAREST_MIPMAP_NEAREST)


enum {CAR, PATRICK, CITY, NUM_OF_MODELS};
enum {R, G, B};
enum {GOD_CAM, MY_CAM} ;

time_t start_time, finish_time;
typedef std::vector<GLfloat> GLvec;
using namespace glm;
using namespace std;
using namespace tinyobj;

GLuint program;
int shading_mode = 1;
bool isSportsCar = true ; // 나중에 바꿔줘
bool is_End = false;
GLfloat acceleration_rate = 0.0f ;

mat4 Car_M(1.0f);
mat4 Player_M(1.0f);

/* City, Patrick, Audi */
const char* model_files [NUM_OF_MODELS] = {
    "/Users/sungminkim/Desktop/OpenGLTest/OpenGLTest/project_obj/Car/LEGO_CAR_B2.obj",
    "/Users/sungminkim/Desktop/OpenGLTest/OpenGLTest/project_obj/Patrick/Patrick.obj",
    "/Users/sungminkim/Desktop/OpenGLTest/OpenGLTest/project_obj/City/serpentine city.obj",
    
};

const char* basedir [NUM_OF_MODELS] = {
    "/Users/sungminkim/Desktop/OpenGLTest/OpenGLTest/project_obj/Car/",
    "/Users/sungminkim/Desktop/OpenGLTest/OpenGLTest/project_obj/Patrick/",
    "/Users/sungminkim/Desktop/OpenGLTest/OpenGLTest/project_obj/City/",
};

float model_scales[NUM_OF_MODELS] = {1.0f, 0.3f, 100.0f}; //car : 1.0 patric 0.3 city 50.0

vector<real_t> vertices[NUM_OF_MODELS];
vector<real_t> normals[NUM_OF_MODELS];
vector<real_t> colors[NUM_OF_MODELS];
vector<vector<size_t>> vertex_map[NUM_OF_MODELS];
vector<vector<size_t>> material_map[NUM_OF_MODELS];
vector<shape_t> shapes[NUM_OF_MODELS];
vector<material_t> materials[NUM_OF_MODELS];
vector<real_t> texcoords[NUM_OF_MODELS];
map<string, size_t> texmap[NUM_OF_MODELS];

GLuint vao[NUM_OF_MODELS];
GLuint vbo[NUM_OF_MODELS][4];
mat3 world_coord(1.0f);

bool is_obj_valid = false;
bool is_tex_valid = false;

// Direction Key offset for the object
GLfloat car_speed = 0.0f ;
GLfloat theta = 0.0f ;
GLfloat accel = 1.0f;
//GLfloat deltacp = 0.0f;
//GLfloat deltath = 0.0f;
//GLfloat deltay = 0.0f;

// Track the car position
vec3 car_pos(1.0f) ;
vec3 cam_mag(0.5f) ;

bool is_forward_pressed = false ;
bool is_back_pressed = false ;
bool is_left_pressed = false ;
bool is_right_pressed = false ;
bool is_booster_pressed = false ;
int cnt = 0 ;
int car_num = 0;

int button_pressed[3] = {GLUT_UP, GLUT_UP, GLUT_UP};
int mouse_pos[2] = {0, 0};
void init();
void display();
int build_program();
void bind_buffer(GLint buffer, GLvec& vec, int program, const GLchar* attri_name, GLint attri_size);

/* Callback Functions */
void motion(int x, int y) ;
void mouse(int button, int state, int x, int y) ;
void special_key(int key, int x, int y) ;
void special_up(int key, int x, int y) ;
void keyboard(unsigned char key, int x, int y) ;

static bool has_file(const char* filepath);
bool load_tex(const char* basedir, vector<real_t>& texcoords_out, map<string, size_t>& texmap_out,
              const vector<real_t>& texcoords, const vector<shape_t>& shapes, const vector<material_t>& materials,
              GLint min_filter, GLint mag_filter);
void render(int color_mode);
void draw_obj_model(int model_idx, int color_mode, int object_code);
GLuint generate_tex( const char* tex_file_path, GLint min_filter, GLint mag_filter);

glm::mat4 parallel(double r, double aspect, double n, double f);

#define FPUSH_VTX3(p, vx, vy, vz)\
do{ \
    p.push_back(vx); \
    p.push_back(vy); \
    p.push_back(vz); \
} while(0)

#define FSET_VTX3(vx, vy, vz, valx, valy, valz)\
do{ \
    vx = (float)(valx); \
    vy = (float)(valy); \
    vz = (float)(valz); \
} while(0)

#define FPUSH_VTX3_AT(p,i,vx,vy,vz)\
do{ \
    GLuint i3 = 3*(i); \
    p[i3+0] = (float)(vx); \
    p[i3+1] = (float)(vy); \
    p[i3+2] = (float)(vz); \
} while(0)



struct Camera {
    enum { ORTHOGRAPHIC, PERSPECTIVE};
    glm::vec3 eye;
    glm::vec3 center;
    glm::vec3 up;
    float zoom_factor;
    int projection_mode;
    float z_near;
    float z_far;
    float fovy;
    float x_right;
    
    Camera() :eye(0, 0, 8), center(0, 0, 0), up(0, 1, 0), zoom_factor(1.0f), projection_mode(ORTHOGRAPHIC), z_near(0.01f),
                z_far(100.0f), fovy((float)(M_PI/180.0*(30.0))), x_right(1.2)
                {}
    
    //lookat() requires a position, target and up vector respectively
    glm::mat4 get_viewing() { return glm::lookAt(eye, center, up); }
    glm::mat4 get_projection(float aspect){
        glm::mat4 P;
        switch (projection_mode) {
            case ORTHOGRAPHIC:
                P = parallel(zoom_factor*x_right, aspect, z_near, z_far);
                break;
                
            case PERSPECTIVE:
                P = glm::perspective(zoom_factor*fovy, aspect, z_near, z_far);
                break;
        }
        return P;
    }
};


int toggle_cam = MY_CAM ;
Camera camera[2];

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA |GLUT_DOUBLE| GLUT_DEPTH |GLUT_3_2_CORE_PROFILE);
    glutInitWindowSize(1024, 1024);
    glutCreateWindow(argv[0]);

    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK){
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        exit(-1);
    }
    
    init();
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special_key);
    glutSpecialUpFunc(special_up);
    glutIgnoreKeyRepeat(1);
    glutMainLoop();
}

void Manual(){
    printf("\n########## Key Info ##########\n") ;
    printf("UP KEY    : straight\n");
    printf("DOWN KEY  : back\n");
    printf("RIGHT KEY : turn right\n");
    printf("LEFT KEY  : turn left\n");
    printf("1         : gouraud shading\n");
    printf("2         : phong shading\n");
    printf("z KEY     : booster\n");
    printf("c KEY     : toggle camera\n");
}

void init(){
    printf("choose the car \n (1) red (2) yellow (3) ghost\n");
    printf("Your input: ");
    scanf("%d", &car_num);
    Manual();
    
    program = build_program();
     
    for(size_t k = 0; k<NUM_OF_MODELS; ++k){
        attrib_t attrib;
        if(k==CAR){
            /* "/Users/dodo4.0/Projects/OpenGL/CG_Term_Project/OpenGLTest/project_obj/Car/LEGO_CAR_A2.obj", */
            if(car_num==1){
                is_obj_valid = load_obj("/Users/sungminkim/Desktop/OpenGLTest/OpenGLTest/project_obj/Car/LEGO_CAR_B2.obj", basedir[k], vertices[k], normals[k], vertex_map[k], material_map[k], attrib, shapes[k], materials[k], model_scales[k]) ;
            }else if(car_num==2){
                is_obj_valid = load_obj("/Users/sungminkim/Desktop/OpenGLTest/OpenGLTest/project_obj/Car/LEGO_CAR_A2.obj", basedir[k], vertices[k], normals[k], vertex_map[k], material_map[k], attrib, shapes[k], materials[k], model_scales[k]) ;
            }else if(car_num==3){
                is_obj_valid = load_obj("/Users/sungminkim/Desktop/OpenGLTest/OpenGLTest/project_obj/Car/LEGO_CAR_A1.obj", basedir[k], vertices[k], normals[k], vertex_map[k], material_map[k], attrib, shapes[k], materials[k], model_scales[k]) ;
            }else{
                cout << "car num error \n " << endl;
            }
        }else{
        is_obj_valid = load_obj(model_files[k], basedir[k], vertices[k], normals[k], vertex_map[k], material_map[k], attrib, shapes[k], materials[k], model_scales[k]) ;
        }

        glActiveTexture(GL_TEXTURE0);
        is_tex_valid = load_tex(basedir[k], texcoords[k], texmap[k], attrib.texcoords, shapes[k], materials[k], GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR);


        if(!is_obj_valid)
            cout << ">>> Can't load the obj" << endl ;
        if(!is_tex_valid)
            cout << ">>> Can't load the tex" << endl ;



        colors[k].resize(vertices[k].size());
        for(int i=0; i<colors[k].size(); ++i){
            colors[k][i] = 1.0f;
        }

//        printf("Loading %zu...\n", k);
        glGenVertexArrays(1, &vao[k]);
        glBindVertexArray(vao[k]);
        glGenBuffers(3, vbo[k]);
        bind_buffer(vbo[k][0], vertices[k], program, "vPosition", 3);
        bind_buffer(vbo[k][1], normals[k], program, "vNormal", 3);
        bind_buffer(vbo[k][2], texcoords[k], program, "vTexcoord", 2);
//        printf("END!!\n");
        
        start_time = time(NULL);
        
    }
    
    
    /* Set the initial camera position */
//    vec3 normalized_vec = (camera.center + vec3(0.0f, 0.0f, 1.0f)) - camera.center ;
//    normalized_vec = normalize(normalized_vec) ;
    
    camera[GOD_CAM].eye = camera[GOD_CAM].center + vec3(0.0f, 0.0f, 1.0f) ;
    camera[GOD_CAM].eye += vec3(0.0f, 1.0f, 8.0f) ;
    
    camera[MY_CAM].eye = camera[MY_CAM].center + vec3(0.0f, 0.0f, 1.0f) ;
    camera[MY_CAM].eye += vec3(0.0f, 1.0f, 8.0f) ;


    Car_M = rotate(Car_M, 3.0f, vec3(0.f, 1.f, 0.f)) ;
    Player_M = rotate(Player_M, 3.0f, vec3(0.f, 1.f, 0.f)) ;
    Player_M = translate(Player_M, vec3(0.1, 0.2, 0.4f));
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
}

GLfloat pre_speed = 0.0f;

void render(int color_mode){
    using namespace glm;
    using namespace std;
    using namespace tinyobj;
    GLuint location, mode_location;
    
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPolygonOffset(1,1);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    int width = glutGet(GLUT_WINDOW_WIDTH);
    int height = glutGet(GLUT_WINDOW_HEIGHT);
    double aspect = 1.0 * width/height;

    mode_location = glGetUniformLocation(program, "mode");
    glUniform1i(mode_location, shading_mode);
    mat4 V = camera[toggle_cam].get_viewing();
    mat4 T(1.0f);
    mat4 P = camera[toggle_cam].get_projection(aspect);
    
    
    location = glGetUniformLocation(program, "V");
    glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(V));
    location = glGetUniformLocation(program, "P");
    glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(P));
    location = glGetUniformLocation(program, "T");
    glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(T));
    
    
    GLfloat speed = 0.0f ;
    
    if(is_forward_pressed){
        acceleration_rate = 2.0f;
        speed = acceleration_rate * 0.01;
        car_speed += speed ;
//        cout << "speed: " << speed << endl ;

//        if(is_left_pressed){
//            theta += 0.003 ;
//         }else if(is_right_pressed){
//            theta -= 0.003 ;
//        }
        
        if(is_booster_pressed == true){
            int time = 2000;
//            cout << "cnt : "<<cnt<<'\n';
            if(cnt > time){
                is_booster_pressed = false;
                accel = 1.0f;
            }
            else if(cnt > time/2){
                accel -= 0.02f;
            }
            else{
                accel += 0.02f;
            }
            cnt += 3;
            speed += 0.01f * accel;
        }

        
    }
    else if(is_back_pressed){
        speed = - acceleration_rate * 0.01;
        car_speed -= 0.01 ;
//        if(is_left_pressed){
//            theta -= 0.003 ;
//        }else if(is_right_pressed){
//            theta += 0.003 ;
//        }
    }
    else{
        if(acceleration_rate > 0){
            if(pre_speed > 0){
                acceleration_rate -= 0.01;
                if(acceleration_rate<0){
                    acceleration_rate = 0.0f;
                }
                speed = acceleration_rate * 0.001;
            }
            else if(pre_speed < 0){
                acceleration_rate -= 0.01;
                if(acceleration_rate<0){
                    acceleration_rate = 0.0f;
                }
                speed = - acceleration_rate * 0.001;
            }
            car_speed += speed ;
        }
    }
    
    pre_speed = speed;
    
    
    if(car_speed!=0.0f){
        vec3 normalized_vec = (camera[toggle_cam].center + vec3(4.0f, 0.0f, -5.0f)) - camera[toggle_cam].center ;
        normalized_vec = normalize(normalized_vec) ;
        
        camera[toggle_cam].center += vec3(0.0f, 0.0f, 5.0f) ;
        camera[toggle_cam].eye = camera[toggle_cam].center + normalized_vec ;
        
        
        if(toggle_cam == GOD_CAM)
            camera[toggle_cam].eye += vec3(0.0f, 10.0f, 0.0f) ;
        else{
            camera[toggle_cam].eye += vec3(0.0f, 2.0f, 0.0f) ;
        }
            
        
        V = camera[toggle_cam].get_viewing() ;
    }

    if(is_obj_valid){
        for(int i=0; i<NUM_OF_MODELS; ++i){
            glBindVertexArray(vao[i]);
            
            location = glGetUniformLocation(program, "M");
            mat4 M(1.0f);
            mat4 R(1.0f);
            
            if(i == PATRICK){ // Patrick
                glUniform1i(UVAR("isSportsCar"), 0);
                
                M = translate(M, vec3(-0.1, -0.2, -0.4f));
                
                if(is_left_pressed){
                    M = rotate(M, 0.01f, vec3(0.f, 1.f, 0.f)) ;
                }
                else if(is_right_pressed){
                    M = rotate(M, -0.01f, vec3(0.f, 1.f, 0.f)) ;
                }
                    
                if(is_forward_pressed){
                    R = rotate(R, -0.90f, vec3(1.f, 0.f, 0.f));
                }else if(is_back_pressed){
                    R = rotate(R, 0.45f, vec3(1.f, 0.f, 0.f));
                }
                
                M = translate(M, vec3(0.0f , 0.0f, speed)) ;
                M = translate(M, vec3(0.1, 0.2, 0.4f));
                
                
                
                Player_M = Player_M * M * R ;
                
                glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(Player_M));
                
            }else if(i == CAR){ // Car
                glUniform1i(UVAR("isSportsCar"), 1);
                if(is_left_pressed){
                    M = rotate(M, 0.01f, vec3(0.f, 1.f, 0.f)) ;
                }
                else if(is_right_pressed){
                    M = rotate(M, -0.01f, vec3(0.f, 1.f, 0.f)) ;
                }
                M = translate(M, vec3(0.f,0.f,speed)) ;
                
                
                for(int x=0; x<3; x++){
                    for(int y=0; y<3; y++){
                        if(M[3][x] == 0){
                            world_coord[y][x]  = 0;
                        }
                        else{
                            world_coord[y][x] = M[y][x]/M[3][x];
                        }
                    }
                }
                
//                cout <<"camera : "<< camera[toggle_cam].center[0] <<" "<< camera[toggle_cam].center[1] <<" "<< camera[toggle_cam].center[2]<<'\n';
                
                if(camera[toggle_cam].center[0]>-75.0 && camera[toggle_cam].center[0]<-73.0 &&
                   camera[toggle_cam].center[2]>-21.0 && camera[toggle_cam].center[0]<-17.0){
                    is_End = true;
                    printf("**END***\n");

                }
                
                Car_M*=M;
                
                // Track the car's coordinates
                vec4 pos = vec4(1.0f, 0.0f, 1.0f, 1.0f) ;
                pos = Car_M * pos ; // apply transformation matrix to pos
                car_pos = vec3(pos.x, pos.y, pos.z) ;
                camera[toggle_cam].center = car_pos ;
            
                glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(Car_M));
                
            }else if(i == CITY){ // City
                glUniform1i(UVAR("isSportsCar"), 0);

                M = rotate(M, 183.0f, vec3(0.f, 1.f, 0.f)) ;
                M = translate(M, vec3(-16.0f, 0.0f, -76.0f)) ;//주석
                
                glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(M));
            }
            
            draw_obj_model(i, color_mode, i+1);
        }
    }
    
    
    glutSwapBuffers();
}

/* event handling functions */
void special_key(int key, int x, int y){
    if(key == GLUT_KEY_LEFT){
//        cout << "<--" << endl ;
        is_left_pressed = true ;
    }
    
    if(key == GLUT_KEY_RIGHT){
//        cout << "-->" << endl ;
        is_right_pressed = true ;
    }
    
    if(key == GLUT_KEY_UP){
//        cout << "FORWARD" << endl ;
        is_forward_pressed = true ;
    }
    
    if(key == GLUT_KEY_DOWN){
//        cout << "BACK" << endl ;
        is_back_pressed = true ;
    }

}

void special_up(int key, int x, int y){
    if(key == GLUT_KEY_LEFT){
        is_left_pressed = false ;
    }
    
    if(key == GLUT_KEY_RIGHT){
        is_right_pressed = false ;
    }
    
    if(key == GLUT_KEY_UP){
        is_forward_pressed = false ;
    }
    
    if(key == GLUT_KEY_DOWN){
        is_back_pressed = false ;
    }
}




void draw_obj_model(int model_idx, int color_mode, int object_code){
    glUniform1i(UVAR("ColorMode"), color_mode);
    glUniform1i(UVAR("ObjectCode"), object_code);
    
    glBindVertexArray(vao[model_idx]);
    using namespace glm;
    using namespace tinyobj;
    
    auto& _shapes = shapes[model_idx];
    auto& _materials = materials[model_idx];
    auto& _vertex_map = vertex_map[model_idx];
    auto& _material_map = material_map[model_idx];
    auto& _texmap = texmap[model_idx];
    
    for(size_t i = 0; i<_shapes.size(); ++i){
        for(size_t j = 0; j<_material_map[i].size(); ++j){
            int m_id = _material_map[i][j];

            if(m_id < 0){
                glUniform1f(UVAR("n"), 10.0f);
                glUniform3f(UVAR("Ka"), 0.3f, 0.3f, 0.3f);
                glUniform3f(UVAR("Kd"), 1.0f, 1.0f, 1.0f);
                glUniform3f(UVAR("Ks"), 0.8f, 0.8f, 0.8f);
            }
            else{
                glUniform1f(UVAR("n"), _materials[m_id].shininess);
                glUniform3fv(UVAR("Ka"), 1, _materials[m_id].ambient);
                glUniform3fv(UVAR("Kd"), 1, _materials[m_id].diffuse);
                glUniform3fv(UVAR("Ks"), 1, _materials[m_id].specular);
                
                auto texitem = _texmap.find(_materials[m_id].diffuse_texname);
                if(texitem != _texmap.end()){
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, texitem->second);
                    glUniform1i(UVAR("sampler"), 0);
                }
            }
            glDrawArrays(GL_TRIANGLES, _vertex_map[i][j], _vertex_map[i][j+1]-_vertex_map[i][j]);
            
        }
    }
    
}

void display(){
    if(is_End == false){
        render(0);
        glFlush();
        glutPostRedisplay();
    }
    else{
        finish_time = time(NULL);
        double result = (double) (finish_time - start_time);
        cout << "Your score : " <<  200 - result<<'\n';
        if(result < 40){
            printf("SUPER CAR\n");
        }
        else if(result < 80){
            printf("GOOD CAR\n");
        }
        else if(result < 120){
            printf("POOR CAR\n");
        }
        else{
            printf("TOO SlOW!\n");
        }
        return;
    }
//    glutSwapBuffers();
}


void bind_buffer(GLint buffer, GLvec& vec, int program, const GLchar* attri_name, GLint attri_size){
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vec.size(), vec.data(), GL_STATIC_DRAW);
    GLuint location = glGetAttribLocation(program, attri_name);
    glVertexAttribPointer(location, attri_size, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(location);
}

glm::mat4 parallel(double r, double aspect, double n, double f){
    double l = -r;
    double width = 2*r;
    double height = width / aspect;
    double t = height/2;
    double b = -t;
    return glm::ortho(l, r, b, t, n, f);
}

int build_program(){
    ShaderInfo shaders[] = {
        {GL_VERTEX_SHADER, "/Users/sungminkim/Desktop/OpenGLTest/OpenGLTest/cart.vert"},
        {GL_FRAGMENT_SHADER, "/Users/sungminkim/Desktop/OpenGLTest/OpenGLTest/cart.frag"},
        {GL_NONE, NULL}
    };

    GLuint program = LoadShaders(shaders);
    glUseProgram(program);
    return program;
}

void mouse (int button, int state, int x, int y){
    if(state == GLUT_UP){
        unsigned char res[4] ;
        int height = glutGet(GLUT_WINDOW_HEIGHT) ;
        glReadPixels(x, height-y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &res) ;
        
//        switch(res[0]){
//            case 1: printf("The earth is clicked!\n"); break ;
//            case 2: printf("The moon is clicked\n"); break ;
//        }
    }
    
    button_pressed[button] = state ;
    mouse_pos[0] = x ;
    mouse_pos[1] = y ;
}

static bool has_file(const char* filepath){
    FILE * fp ;
    fp = fopen(filepath, "rb") ;
//    cout << ">>> has_file filepath: " << filepath << " , " << fp << endl ;
    
    if(fp == NULL){
        fclose(fp) ;
        return false ;
    }
    
    return true ;
}


GLuint generate_tex( const char* tex_file_path, GLint min_filter, GLint mag_filter){
    int width, height, num_of_components;
    unsigned char* image = stbi_load(
                                     tex_file_path, &width, &height,
                                     &num_of_components, STBI_default);

    if(!image){
        fprintf(stderr, "Failed to open %s\n", tex_file_path);
        return false;
    }
    GLuint texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);

    bool is_supported = true;

    switch (num_of_components) {
        case 3:
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
            break;

        case 4:
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
            break;

        default:
            is_supported = false;
            break;
    }

    if(IS_MIPMAP(min_filter) || IS_MIPMAP(mag_filter))
        glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(image);
    glBindTexture(GL_TEXTURE_2D, 0);

    if(!is_supported){
        fprintf(stderr, "Unsupported image format : %d components\n",
                num_of_components);
        glDeleteTextures(1, &texture_id);
        texture_id = -1;
    }

    return texture_id;
}

bool load_tex(const char* basedir, vector<real_t>& texcoords_out, map<string, size_t>& texmap_out,
                const vector<real_t>& texcoords, const vector<shape_t>& shapes, const vector<material_t>& materials,
                GLint min_filter, GLint mag_filter){
    using namespace std;
    using namespace tinyobj;

    size_t total_num_of_vertices = 0;
    size_t num_of_shapes = shapes.size();

    for(size_t s = 0; s<num_of_shapes; ++s){
        total_num_of_vertices += shapes[s].mesh.indices.size();
    }
    texcoords_out.resize(total_num_of_vertices*2);

    real_t* texcoords_dst_ptr = texcoords_out.data();
    const real_t* texcoords_src_ptr = texcoords.size() > 0 ? texcoords.data() : NULL;

    for(size_t s = 0; s<num_of_shapes; ++s){
        const mesh_t& mesh = shapes[s].mesh;
        size_t num_of_faces = mesh.indices.size() / 3;
        for(size_t f = 0; f<num_of_faces; ++f){
            int idx[3] = {
                mesh.indices[3*f+0].texcoord_index,
                mesh.indices[3*f+1].texcoord_index,
                mesh.indices[3*f+2].texcoord_index
            };
            real_t tc[3][2];
            if(texcoords_src_ptr != NULL){
                if(idx[0] < 0 || idx[1] < 0 || idx[2] < 0){
                    fprintf(stderr, "Invalid texture coordinate index\n");
                    return false;
                }
                for(size_t i = 0; i<3; ++i){
                    memcpy(tc[i], texcoords_src_ptr + idx[i] * 2, sizeof(real_t)*2);
                    tc[i][1] = 1.0f - tc[i][1];
                }
            }
            else{
                tc[0][0] = tc[0][1] = 0;
                tc[1][0] = tc[1][1] = 0;
                tc[2][0] = tc[2][1] = 0;
            }

            memcpy(texcoords_dst_ptr, tc, sizeof(real_t) * 6);
            texcoords_dst_ptr +=6;
        }
    }
    
    
    //2. make a texture
    GLuint texture_id;
    size_t num_of_materials = materials.size();

    for(size_t m = 0; m<num_of_materials; ++m){
        const material_t& mat = materials[m];
        const string& texname = mat.diffuse_texname;
        if(texname.empty()) continue;
        if (MAP_FIND(texmap_out, texname)) continue;

        string full_texpath = texname;
        
        if(!has_file(full_texpath.c_str())){
            full_texpath = basedir + texname;
            
            if(!has_file(full_texpath.c_str())){
                cout << ">>> full_path: " << full_texpath << endl ;
                fprintf(stderr, "Failed to find %s\n", texname.c_str()) ;
                return false ;
            }
        }

        texture_id = generate_tex(full_texpath.c_str(), min_filter, mag_filter);
        if(texture_id < 0)
            return false;

        texmap_out[texname] = texture_id;
    }
    return true;
}


void keyboard(unsigned char key, int x, int y){
    switch(key){
        case '1':
            shading_mode = 1;
            printf("gouraud shading\n");
            glutPostRedisplay();
            break;
        case '2':
            shading_mode = 2;
            printf("phong shading\n");
            glutPostRedisplay();
            break;
        case 'z':
            is_booster_pressed = true ;
            cnt = 0;
            break;
            
        case 'c': case 'C':
            if(toggle_cam == GOD_CAM){
                toggle_cam = MY_CAM ;
            }else{
                toggle_cam = GOD_CAM ;
            }
//        case 32:
//            car_speed = 0 ;
//            theta = 0 ;
//            is_back_pressed = false ;
//            is_forward_pressed = false ;
//            is_right_pressed = false ;
//            is_left_pressed = false ;
//            break ;
    }
}

void motion(int x, int y){ // called back when the user move a mouse pointer within the window
    int modifiers = glutGetModifiers() ;
    int is_alt_active = modifiers & GLUT_ACTIVE_ALT ;
    int is_command_active = modifiers & GLUT_ACTIVE_COMMAND ;
    int is_shift_active = modifiers & GLUT_ACTIVE_SHIFT ;
    
    int w = glutGet(GLUT_WINDOW_WIDTH) ;
    int h = glutGet(GLUT_WINDOW_HEIGHT) ;
    GLfloat dx = 1.f*(x - mouse_pos[0]) / w ;
    GLfloat dy = -1.f*(y - mouse_pos[1]) / h ;
    
    
    
    /* 설명
     Mouse Middle Button은 Mouse Left Button + Shift로 설정하였습니다.
     
     Tumble Tool: Alt + Mouse Left Button + Drag
     Track Tool: Shift + Alt + Mouse Left Button + Drag
     Dolly Tool: Shift + Command + Mouse Left Button + Drag
     Zoom Tool: Command + Mouse Left Button + Drag

     */
    
    
    
    
    if(button_pressed[GLUT_LEFT_BUTTON] == GLUT_DOWN && is_alt_active && is_shift_active){ /* Track Tool */
        mat4 VT(1.0f) ;
        VT = transpose(camera[toggle_cam].get_viewing()) ;
        camera[toggle_cam].eye += vec3(-dx*VT[0] + -dy*VT[1]) ;
        camera[toggle_cam].center += vec3(-dx*VT[0] + -dy*VT[1]) ;
    }else if(button_pressed[GLUT_LEFT_BUTTON] == GLUT_DOWN && is_shift_active && is_command_active){ /* Dolly Tool */
        // Mouse Middle Button + Command + Drag
        vec3 disp = camera[toggle_cam].eye - camera[toggle_cam].center ;
        if(dy > 0){
            camera[toggle_cam].eye = camera[toggle_cam].center + 0.95f * disp ;
        }else{
            camera[toggle_cam].eye = camera[toggle_cam].center + 1.05f * disp ;
        }
    }else if(button_pressed[GLUT_LEFT_BUTTON] == GLUT_DOWN && is_command_active){ /* Zoom Tool */
        // Mouse Left Button + Command + Drag
        if(dy > 0){
            camera[toggle_cam].zoom_factor *= 0.95f ;
        }else{
            camera[toggle_cam].zoom_factor *= 1.05f ;
        }
    }else if(button_pressed[GLUT_LEFT_BUTTON] == GLUT_DOWN && is_alt_active){ /* Tumble Tool */
        vec4 disp(camera[toggle_cam].eye - camera[toggle_cam].center, 1) ;
        
        GLfloat alpha = 2.0f;
        mat4 V(1.0f), Rx(1.0f), Ry(1.0f), R(1.0f) ;
        
        
        V = camera[toggle_cam].get_viewing() ;
        Rx = rotate(mat4(1.0f), alpha*dy, vec3(transpose(V)[0])) ;
        Ry = rotate(mat4(1.0f), -alpha*dx, vec3(0,1,0)) ;
        R = Ry * Rx ;
        camera[toggle_cam].eye = camera[toggle_cam].center + vec3(R*disp) ;
        camera[toggle_cam].up = mat3(R) * camera[toggle_cam].up ;
    }
    
    mouse_pos[0] = x ;
    mouse_pos[1] = y ;
//    glutPostRedisplay() ;
}
