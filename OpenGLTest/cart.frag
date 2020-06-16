#version 410

out vec4 FragColor;
in vec4 fColorFront;
in vec4 fColorBack;
in vec4 fColor;
in vec4 fPosition;
in vec4 fNormal;
in vec2 fTexcoord;

uniform mat4 T;
uniform int mode;
uniform int ColorMode;
uniform int ObjectCode;
uniform sampler2D sampler;
uniform vec4 uColor;

uniform int isSportsCar;

uniform mat4 M;
uniform mat4 P;
uniform mat4 V;
uniform mat4 U;

uniform vec3 Ia = vec3(1.0,1.0,1.0);
uniform vec3 Il = vec3(1.0);
uniform vec3 Ka /*= 0.3 */ ;
uniform vec3 Ks /*= 0.5 */;
uniform vec3 Kd /*= 0.8*/ ;
uniform float c[3] = float[3](0.01, 0.001, 0.0);
uniform float n  = 10.0;
uniform vec4 LightPos_wc = vec4(10, 10, 3, 1); //light world coord

vec4 shading(vec3 LightPos_ec, vec3 vPosition_ec, vec3 vNormal_ec)
{
    vec3 N = normalize(vNormal_ec);
    vec3 L = LightPos_ec - vPosition_ec;
    float d = length(L); L = L/d;
    vec3 V = normalize(vec3(0.0) - vPosition_ec); //sample position to eye position
    vec3 R = reflect(-L, N);

    float fatt = min(1.0 / (c[0] + c[1]*d + c[2]*d*d), 1.0);

    float cos_theta = max(dot(N,L),0); //cos이 음수면 back부분 이기 때문에
    float cos_alpha = max(dot(V,R),0);

    vec3 I = Ia * Ka + fatt * Il * (Kd * cos_theta + Ks * pow(cos_alpha, n)) /**vec3(fColor)*/; //compution intensity
    return vec4(I,1);
}

void default_shading() //for default shading
{
    FragColor = fColor;
}

void gouraud_shading() //for gouraud shading
{
    if(ColorMode == 1){ //color mode가 1이면 파란색을 FragColor로 지정해주고 반환한다.
        FragColor = vec4(0,0,1,1);
    }
    else if(ColorMode == 2){
        float nc = ObjectCode / 255.0;
        FragColor = vec4(nc, nc, nc, 1);
    }
    else{
        if(gl_FrontFacing)          //front와 back을 나누어서 FragColor를 지정한다.
            FragColor = fColorFront;
        else
            FragColor = fColorBack;
        
        if(isSportsCar == 0)
            FragColor *= texture(sampler, fTexcoord);
    }
}

void phong_shading() //for phong shading
{
    if(ColorMode == 1){
           FragColor = vec4(0,0,1,1);
           return;
    }
    else if(ColorMode == 2){
        float nc = ObjectCode / 255.0;
        FragColor = vec4(nc, nc, nc, 1);
    }
    else{
        vec3 LightPos_ec = vec3(V * LightPos_wc);
        if(gl_FrontFacing)          //front와 back을 나누어서 FragColor를 지정한다.
            FragColor = shading(LightPos_ec, vec3(fPosition), vec3(fNormal));
        else
            FragColor = shading(LightPos_ec, vec3(fPosition), vec3(-fNormal));
        
        if(isSportsCar == 0)
            FragColor *= texture(sampler, fTexcoord);
        
            
    }
}

void main(){
    
    switch(mode){
       case 0:
           default_shading();
           break;
       case 1:
           gouraud_shading();
           break;
       case 2:
           phong_shading();
           break;
       case 3:
           FragColor = uColor;
           break;
       }
       
}

