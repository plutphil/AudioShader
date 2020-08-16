
#include <SFML/Window.hpp>
#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include <iostream>
#include <SFML/Audio.hpp>
int main()
{

    
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 4;
    settings.majorVersion = 3;
    settings.minorVersion = 0;
        // create the window
        sf::Window window(sf::VideoMode(800, 600), "OpenGL", sf::Style::Default, sf::ContextSettings(40));
        glewExperimental = true;
        glewInit();
        
        window.setVerticalSyncEnabled(true);

        // activate the window
        window.setActive(true);

        std::cout << "depth bits:" << settings.depthBits << std::endl;
        std::cout << "stencil bits:" << settings.stencilBits << std::endl;
        std::cout << "antialiasing level:" << settings.antialiasingLevel << std::endl;
        std::cout << "version:" << settings.majorVersion << "." << settings.minorVersion << std::endl;
        // load resources, initialize the OpenGL states, ...
        //Init:
        GLuint pbo;
        //glGenBuffers(1, &pbo);
        //glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo);
        
        int  width = 1024;
        int height = 1;
        //glBufferData(GL_PIXEL_PACK_BUFFER, width * height * 4, NULL, GL_DYNAMIC_READ);
        // run the main loop
        bool running = true;
        char* pixeldata= (char*)malloc(width * height * 4);
        

        const char* srcvert = R"(   
#version 120        
attribute vec4 position; 
varying vec4 vColor;     
void main() {
gl_Position = position;
vColor=position;
}
            
            )";
        const char* srcfrag = R"(
#version 120
precision mediump float;
varying vec4 vColor;
uniform float off=0;

float fm(float time,float freq){
return sin(sin(sin(time/freq)*20.0f));
}
float main2(float time){
    float l=10;
    float add=0.0;
    for(float i=0;i<l;i+=3.3)
    {
        add+=sin(time/(200.0f+i));
    }
    return add/l;
}

void main() {          
gl_FragColor=vec4(
main2(
(
(vColor.x-0.5)
/2.*1024.0
)
+off)
,0.,0.,1.);//vec4(1.0,1.0,0.0,1.0);//vColor;//vec4(vColor.xyz,1.0);
};
            
            )";
        GLuint vertexshader=0;
        vertexshader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexshader, 1, &srcvert, NULL);
        glCompileShader(vertexshader);
        GLint compiled;
        glGetShaderiv(vertexshader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLint infoLen = 0;
            glGetShaderiv(vertexshader, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen > 1) { 
                char* infoLog = new char[sizeof(char) * infoLen];         
                glGetShaderInfoLog(vertexshader, infoLen, NULL, infoLog);         
                printf("Error compiling shader:\n%s\n", infoLog);         
                free(infoLog); 
            }
        }
        else {
            printf("succesfully compiled!\n");
        }
        
        GLuint fragmentshader=0;
        fragmentshader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentshader, 1, &srcfrag, NULL);
        glCompileShader(fragmentshader);
        glGetShaderiv(fragmentshader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLint infoLen = 0;
            glGetShaderiv(fragmentshader, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen > 1) {
                char* infoLog = new char[sizeof(char) * infoLen];
                glGetShaderInfoLog(fragmentshader, infoLen, NULL, infoLog);
                printf("Error compiling shader:\n%s\n", infoLog);
                free(infoLog);
            }
        }
        else {
            printf("succesfully compiled!\n");
        }

        GLuint program;
        program=glCreateProgram();
        glAttachShader(program, vertexshader);
        glAttachShader(program,fragmentshader);
        glBindAttribLocation(program, 0, "position");
        GLuint timeoffsetlocation =glGetUniformLocation(program, "off");
        glLinkProgram(program);
        GLint linked;
        glGetProgramiv(program, GL_LINK_STATUS, &linked);   
        if (linked) {
            printf("program linked!\n");
        }
        else {
            GLint infoLen = 0;      
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLen);      
            if (infoLen > 1) { 
                char* infoLog = new char[(sizeof(char) * infoLen)];         
                glGetProgramInfoLog(program, infoLen, NULL, infoLog);         
                printf("Error linking program:\n%s\n", infoLog);         
                
            }      
        
        }
        GLfloat vVertices[] = { 
            -1.0,-1.0,
            -1.0,1.0,
            1.0,-1.0,
            
            1.0,1.0,
            1.0,-1.0,
            -1.0,1.0,
        };
        float timeoffset=0.0f;
        sf::Sound sound;
        int multiples = 8;
        bool isready = false;
        bool played = false;
        sf::SoundBuffer buffer;
        while (running)
        {
            // handle events
            sf::Event event;
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                {
                    // end the program
                    running = false;
                }
                else if (event.type == sf::Event::Resized)
                {
                    // adjust the viewport when the window is resized
                    //glViewport(0, 0, event.size.width, event.size.height);
                }
            }
           
            if (sound.getStatus() != sf::SoundSource::Playing) {
                
                
                
                
                if (isready) {
                    printf("pl\n");




                    sound.setBuffer(buffer);

                    //sound.setLoop(true);
                    sound.play();
                    if (sound.getStatus() == sound.Playing) {
                        printf("playing");
                    }
                    isready = false;
                }
            }
           
            if (!isready) {
                std::vector<sf::Int16> samples(width * multiples);
                for (size_t m = 0; m < multiples; m++)
                {
                    // clear the buffers
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                    // draw...

                    glViewport(0, 0, width, height);
                    glUseProgram(program);
                    glUniform1f(timeoffsetlocation, timeoffset);
                    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, vVertices);
                    glEnableVertexAttribArray(0);
                    glDrawArrays(GL_TRIANGLES, 0, 6);
                    std::vector<std::uint8_t> data(width * height * 4);
                    glReadBuffer(GL_FRONT);
                    glReadPixels(0, 0, width, height, GL_BGRA, GL_UNSIGNED_BYTE, &data[0]);
                    for (size_t i = 0; i < width; i++) {
                        //printf("%i,", data[i*4+2]);
                        samples[i + m * width] = data[i * 4 + 2] * 256;
                        //printf("%i,", samples[i]);
                    }
                    timeoffset += 1024.0f;
                }
                buffer.loadFromSamples(&samples[0], samples.size(), 2, 44100);
                isready = true;
            }
            
            

            //glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo);
            //glReadPixels(0, 0, width, height, GL_BGRA, GL_UNSIGNED_BYTE, 0);


           
            //after drawing
            // Return to onscreen rendering:
            //glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo); //Might not be necessary...
            //auto pixeldata =(char*) glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);

            
            
            // end the current frame (internally swaps the front and back buffers)
            window.display();
        }

        // release resources...

        return 0;
    
    std::cout << "Hello World!\n";


    
}

