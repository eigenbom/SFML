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
				gl_FrontColor = gl_Color;
			}
			)";

    const char* fragmentShader = R"(#version 120
			void main(void)
			{
				gl_FragColor = gl_Color;
			}
			)";

    sf::Shader shader;
    bool result = shader.loadFromMemory(vertexShader, fragmentShader);
    assert(result);

    sf::RectangleShape shape { sf::Vector2f(50, 50) };
    shape.setOrigin(25, 25);

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
            shape.setPosition(i * 60, 100);
            sf::RenderStates states;
            states.shader = &shader;
            states.shaderIsBound = true;
            offscreen.draw(shape, states);
        }
        sf::Shader::bind(NULL);
        offscreen.setActive(false);
				
        window.clear();
        window.draw(sf::Sprite(offscreen.getTexture()));
        window.display();
    }

    return EXIT_SUCCESS;
}
