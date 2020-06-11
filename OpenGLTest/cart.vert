#version 410

in vec4 vPosition;
in vec4 vNormal; // main.cpp의 normal vector 연결
in vec4 vColor;
in vec2 vTexcoord;

out vec4 fColor;
out vec4 fColorFront;
out vec4 fColorBack;
out vec2 fTexcoord;
out vec4 fNormal;
out vec4 fPosition;

uniform mat4 T;    // Additional transformation performed before modeling transformation
uniform int mode;
uniform mat4 M;
uniform mat4 P;
uniform mat4 V;
uniform mat4 U;

uniform vec3 Ia = vec3(1.0,1.0,1.0);
uniform vec3 Il = vec3(1.0); //intensity of light source
uniform vec3 Ka /*= 0.3*/;
uniform vec3 Ks /*= 0.5*/;
uniform vec3 Kd /*= 0.8*/;
uniform float c[3] = float[3](0.01, 0.001, 0.0); //constant, linear, quadratic value를 갖는다.
uniform float n /*= 10.0*/;
uniform vec4 LightPos_wc = vec4(10, 10, 3, 1); // light source position

vec4 shading(vec3 LightPos_ec, vec3 vPosition_ec, vec3 vNormal_ec) //evaluate phong reflection model. 파라미터는 eye coord를 represent한다.
{
    vec3 N = normalize(vNormal_ec);
    vec3 L = LightPos_ec - vPosition_ec;
    float d = length(L); L = L/d; // L is unit vector
    vec3 V = normalize(vec3(0.0) - vPosition_ec); //sample position to eye position
    vec3 R = reflect(-L, N);

    float fatt = min(1.0 / (c[0] + c[1]*d + c[2]*d*d), 1.0);

    float cos_theta = max(dot(N,L),0); //cos이 음수면 back부분 이기 때문에
    float cos_alpha = max(dot(V,R),0);

    vec3 I = Ia * Ka + fatt * Il * (Kd * cos_theta + Ks * pow(cos_alpha, n))/**vec3(vColor)*/; //compution intensity

    return vec4(I,1);
}

void default_shading() //for default shading
{
    gl_Position = P*V*M*T * vPosition;
    fColor = vColor;
}

void gouraud_shading() // for gouraud shading
{
    mat4 VMT = V*M*T;
    mat4 U = transpose(inverse(VMT));
    vec3 vNormal_ec = vec3(normalize(U*vNormal)); //transform normal vector
    vec3 vPosition_ec = vec3(VMT * vPosition); //eye coord 구하기
    vec3 LightPos_ec = vec3(V * LightPos_wc); //world coord to eye coord (Light)

    gl_Position = P * vec4(vPosition_ec, 1);
    
    fColorFront = shading(LightPos_ec, vPosition_ec, vNormal_ec); //Front와 Back 나눠서 색깔지정  back은 -Normal vector 를 사용한다.
    fColorBack = shading(LightPos_ec, vPosition_ec, -vNormal_ec);
    fTexcoord = vTexcoord;
}

void phong_shading() //for phong shading
{
    mat4 VMT = V*M*T;
    mat4 U = transpose(inverse(VMT));
    vec3 vNormal_ec = vec3(normalize(U*vNormal)); //eye coord 계산
    vec3 vPosition_ec = vec3(VMT * vPosition);
    vec3 LightPos_ec = vec3(V * LightPos_wc);
    
    gl_Position = P * vec4(vPosition_ec, 1);
    fNormal = vec4(vNormal_ec, 1); //fragment에 들어갈 fNormal, fPosition을 각각 vNormal_ec, vPosition_ec으로 준다.
    fPosition = vec4(vPosition_ec, 1);
    fTexcoord = vTexcoord;
}


void main(){
    
    switch(mode)
    {
//        case 0:    // use vertex color data
        case 3:    // use constant user color uColor
            default_shading();
            break;
        case 1:
            gouraud_shading();
            break;
        case 2:
            phong_shading();
            break;
    }
}
