#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <cassert>

void loadShaders(sf::Shader& texturedShader, sf::Shader& untexturedShader) {
    {
        const char* vertexShader = R"(#version 120
			uniform vec4 u_colour; // Colour uniform, used for new Sprite implementation
			void main(void)
			{
				gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
				gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
				gl_FrontColor = gl_Color * u_colour;
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

        bool result = texturedShader.loadFromMemory(vertexShader, fragmentShader);
        assert(result);
        texturedShader.setUniform("u_tex", sf::Shader::CurrentTexture);
    }

    {
        const char* vertexShader = R"(#version 120
			uniform vec4 u_colour; // Colour uniform, used for new Sprite implementation
			void main(void)
			{
				gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
				gl_FrontColor = gl_Color * u_colour;
			}
			)";

        const char* fragmentShader = R"(#version 120
			void main(void)
			{
				gl_FragColor = gl_Color;
			}
			)";

        bool result = untexturedShader.loadFromMemory(vertexShader, fragmentShader);
        assert(result);
    }
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(640, 480), "SFML test");

    sf::RenderTexture offscreen;
    offscreen.create(512, 512);	
    offscreen.resetGLStates(); // NB: Need to do this to initialise the glStates

    sf::Shader texturedShader, untexturedShader;
    loadShaders(texturedShader, untexturedShader);

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

    sf::RectangleShape shape(sf::Vector2f(image.getSize().x, image.getSize().y));
    shape.setScale(6, 6);

    sf::Font font;
    bool fontLoaded = font.loadFromFile("resources/sansation.ttf");
    assert(fontLoaded);
    sf::Text text("[:D]", font, 32);
    text.setFillColor(sf::Color::White);
    text.setOutlineColor(sf::Color::White);

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
        sf::Shader::bind(&texturedShader);
        for (int i = 0; i < 10; ++i) {
            sprite.setPosition(i * 60, 100);
            sprite.setColor(sf::Color(255, 255, 100 + 15 * i));

            sf::RenderStates states;
            states.shader = &texturedShader;
            states.shaderIsBound = true;
            offscreen.draw(sprite, states);
        }
        sf::Shader::bind(NULL);

        sf::Shader::bind(&untexturedShader);
        for (int i = 0; i < 10; ++i) {
            shape.setPosition(i * 60, 200);
            shape.setFillColor(sf::Color(255, 255, 100 + 15 * i));
            sf::RenderStates states;
            states.shader = &untexturedShader;
            states.shaderIsBound = true;
        	offscreen.draw(shape, states);
        }
        sf::Shader::bind(NULL);
		
        for (int i = 0; i < 10; ++i) {
            text.setPosition(i * 60, 300);
            text.setFillColor(sf::Color(255, 255, 100 + 15 * i));
            sf::RenderStates states;
            states.shader = &texturedShader;
            // states.color = sf::Color::White;
            offscreen.draw(text, states);
        }
        offscreen.setActive(false);
				
        window.clear();
        window.setView(sf::View(sf::FloatRect(0, 0, offscreen.getSize().x, offscreen.getSize().y)));
        sf::Sprite offscreenSprite(offscreen.getTexture());
        offscreenSprite.setScale(1, -1);
        offscreenSprite.setPosition(0, offscreen.getSize().y);
        window.draw(offscreenSprite, &texturedShader);
        window.display();
    }

    return EXIT_SUCCESS;
}
