#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <cassert>

int main()
{
    sf::RenderWindow window(sf::VideoMode(640, 480), "SFML test");

    sf::RenderTexture offscreen;
    offscreen.create(512, 512);	
    offscreen.resetGLStates(); // NB: Need to do this to initialise the glStates

    const char* vertexShader = R"(#version 120
			void main(void)
			{
				gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
				gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
				gl_FrontColor = gl_Color;
			}
			)";

    const char* fragmentShader = R"(#version 120
			uniform sampler2D u_tex;
			void main(void)
			{
				vec4 colour = texture2D(u_tex, gl_TexCoord[0].st);
				gl_FragColor = gl_Color * colour;
			}
			)";

    sf::Shader shader;
    bool result = shader.loadFromMemory(vertexShader, fragmentShader);
    assert(result);
    shader.setUniform("u_tex", sf::Shader::CurrentTexture);

    sf::Image image;
    image.create(8, 8);
	for (int i=0; i<image.getSize().x; ++i) {
        for (int j = 0; j < image.getSize().y; ++j) {
            image.setPixel(i, j, (i + j) % 2 == 0 ? sf::Color::White : sf::Color::Black);
        }		
	}
	
    sf::Texture texture;
    texture.loadFromImage(image);

    sf::Sprite sprite (texture);
    sprite.setOrigin(image.getSize().x / 2, image.getSize().y / 2);
    sprite.setScale(6, 6);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape))
                window.close();
        }

        if (!window.isOpen()) break;
				
        offscreen.setActive(true);
        offscreen.clear(sf::Color::Transparent); 
        sf::Shader::bind(&shader);
        for (int i = 0; i < 10; ++i) {
            sprite.setPosition(i * 60, 100);
            sf::RenderStates states;
            states.shader = &shader;
            states.shaderIsBound = true;
            offscreen.draw(sprite, states);
        }
        sf::Shader::bind(NULL);
        offscreen.setActive(false);
				
        window.clear();
        window.draw(sf::Sprite(offscreen.getTexture()));
        window.display();
    }

    return EXIT_SUCCESS;
}
