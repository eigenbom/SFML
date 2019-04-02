#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <cassert>
#include <iostream>

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
        texturedShader.setAutoBind(false);
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
        untexturedShader.setAutoBind(false);
        assert(result);
    }
}

void testAllDrawables() {
    sf::RenderWindow window(sf::VideoMode(640, 480), "SFML test");

    sf::RenderTexture offscreen;
    offscreen.create(512, 512);

    sf::Shader texturedShader, untexturedShader;
    loadShaders(texturedShader, untexturedShader);

    sf::Image image;
    image.create(8, 8);
    for (int i = 0; i < image.getSize().x; ++i) {
        for (int j = 0; j < image.getSize().y; ++j) {
            image.setPixel(i, j, (i + j) % 2 == 0 ? sf::Color::White : sf::Color::Black);
        }
    }

    sf::Texture texture;
    texture.loadFromImage(image);

    sf::Sprite sprite(texture);
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
		
        offscreen.setupGLStates();
        offscreen.clear(sf::Color::Transparent);

        for (int i = 0; i < 10; ++i) {
            text.setPosition(i * 60, 300);
            text.setFillColor(sf::Color(255, 255, 100 + 15 * i));
            offscreen.draw(text);
        }

        sf::Shader::bind(&texturedShader);
        for (int i = 0; i < 10; ++i) {
            text.setPosition(i * 60, 400);
            text.setFillColor(sf::Color(255, 255, 100 + 15 * i));
            sf::RenderStates states;
            states.shader = &texturedShader;
            states.shaderIsBound = true;
            // states.color = sf::Color::White;
            offscreen.draw(text, states);
        }
        sf::Shader::bind(NULL);

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

        window.setupGLStates();
        window.clear();
        window.setView(sf::View(sf::FloatRect(0, 0, offscreen.getSize().x, offscreen.getSize().y)));
        sf::Sprite offscreenSprite(offscreen.getTexture());
        offscreenSprite.setScale(1, -1);
        offscreenSprite.setPosition(0, offscreen.getSize().y);
        window.draw(offscreenSprite, &texturedShader);
        window.display();
    }
}

void traceSpritePerf() {
    sf::RenderWindow window(sf::VideoMode(512, 512), "SFML test");
    

    sf::Shader texturedShader, untexturedShader;
    loadShaders(texturedShader, untexturedShader);

    sf::Texture textureAB;
    textureAB.loadFromFile("resources/ab.png");

    sf::Texture textureA;
    textureA.loadFromFile("resources/ab.png", sf::IntRect(0, 0, 32, 32));

    sf::Texture textureB;
    textureB.loadFromFile("resources/ab.png", sf::IntRect(32, 0, 32, 32));
	
    sf::Sprite spriteA(textureAB, sf::IntRect(0, 0, 32, 32));
    spriteA.setOrigin(16, 16);
    spriteA.setScale(2, 2);

    sf::Sprite spriteB(textureAB, sf::IntRect(32, 0, 32, 32));
    spriteB.setOrigin(16, 16);
    spriteB.setScale(2, 2);

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

        window.setupGLStates();
        window.clear();
        sf::Shader::bind(&texturedShader);
        std::srand(0);
        for (int i = 0; i < 100; ++i){
            sf::Sprite& sprite = (std::rand() % 8 > 4) ? spriteA : spriteB;
            sprite.setPosition(std::rand() % 512, std::rand() % 512);
            sprite.setColor(sf::Color(std::rand()%256, std::rand() % 256, std::rand() % 256));
            sf::RenderStates states;
            states.shader = &texturedShader;
            states.shaderIsBound = true;
            window.draw(sprite, states);
        }
        sf::Shader::bind(NULL);
        window.display();
    }
}

void textureBindingBug() {
    sf::RenderWindow window(sf::VideoMode(512, 512), "SFML test");
    window.setupGLStates();
    window.display();

    sf::Shader shader;
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

    bool result = shader.loadFromMemory(vertexShader, fragmentShader);
    assert(result);
    shader.setAutoBind(false);

    sf::Texture textureA;
    textureA.loadFromFile("resources/ab.png", sf::IntRect(0, 0, 32, 32));

    sf::Texture textureB;
    textureB.loadFromFile("resources/ab.png", sf::IntRect(32, 0, 32, 32));

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

        window.setupGLStates();
        window.clear(sf::Color::Blue);

        std::srand(0);

        sf::Shader::bindProgram(&shader);
		for (int i=0; i<2; ++i){

            sf::Vector2f viewSize = (sf::Vector2f) window.getSize();

            // shader.setUniform("u_tex", (i == 0) ? textureA : textureB);
            shader.setUniform("u_tex", sf::Shader::CurrentTexture);

            sf::Color colourTop = sf::Color(255, 255, 255);
            sf::Color colourBottom = sf::Color(0, 0, 0);

            sf::Vertex v[4];

            v[0].position = sf::Vector2f(0, 0);
            v[1].position = sf::Vector2f(viewSize.x / 2, 0);
            v[2].position = sf::Vector2f(viewSize.x / 2, viewSize.y);
            v[3].position = sf::Vector2f(0, viewSize.y);

			if (i == 1) {
                for (int j = 0; j < 4; ++j) v[j].position.x += viewSize.x / 2;
			}

            v[0].color = colourTop;
            v[1].color = colourTop;
            v[2].color = colourBottom;
            v[3].color = colourBottom;

            v[0].texCoords = sf::Vector2f(0, 0);
            v[1].texCoords = sf::Vector2f(32, 0);
            v[2].texCoords = sf::Vector2f(32, 32);
            v[3].texCoords = sf::Vector2f(0, 32);
        	
        	sf::RenderStates states;
            states.shader = &shader;
            states.shaderIsBound = true;
            states.texture = (i == 0) ? &textureA : &textureB;			
            window.draw(v, 4, sf::PrimitiveType::Quads, states);
        }
        sf::Shader::bindProgram(NULL);

        window.display();
    }
}

void renderThingToTarget(sf::RenderTarget& target, sf::Shader& shader, sf::Texture& texture) {
    sf::Shader::bindProgram(&shader);
    sf::Vector2f viewSize = (sf::Vector2f) target.getSize();
    shader.setUniform("u_tex", sf::Shader::CurrentTexture);

    sf::Color colourTop = sf::Color(255, 255, 255);
    sf::Color colourBottom = sf::Color(0, 0, 0);

    sf::Vertex v[4];

    v[0].position = sf::Vector2f(0, 0);
    v[1].position = sf::Vector2f(viewSize.x, 0);
    v[2].position = sf::Vector2f(viewSize.x, viewSize.y);
    v[3].position = sf::Vector2f(0, viewSize.y);
	
    v[0].color = colourTop;
    v[1].color = colourTop;
    v[2].color = colourBottom;
    v[3].color = colourBottom;

    v[0].texCoords = sf::Vector2f(0, 0);
    v[1].texCoords = sf::Vector2f(32, 0);
    v[2].texCoords = sf::Vector2f(32, 32);
    v[3].texCoords = sf::Vector2f(0, 32);

    sf::RenderStates states;
    states.shader = &shader;
    states.shaderIsBound = true;
    states.texture = &texture;
    target.draw(v, 4, sf::PrimitiveType::Quads, states);

    sf::Shader::bindProgram(NULL);	
}

void traceContextRedundancy() {
    sf::RenderWindow window(sf::VideoMode(512, 512), "SFML test");
    window.setupGLStates();
    window.display();



    sf::RenderTexture targetA, targetB;
    targetA.create(512, 512);
    targetB.create(512, 512);

    sf::Shader shader;
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

    bool result = shader.loadFromMemory(vertexShader, fragmentShader);
    assert(result);
    shader.setAutoBind(false);

    sf::Texture texture;
    texture.loadFromFile("resources/ab.png", sf::IntRect(0, 0, 32, 32));

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

        targetA.setActive(true);
        targetA.setupGLStates();
        renderThingToTarget(targetA, shader, texture);
        renderThingToTarget(targetA, shader, texture);
        targetA.display();
        targetA.setActive(false);

        targetB.setActive(true);
    	targetB.setupGLStates();
        renderThingToTarget(targetB, shader, texture);
        targetB.display();
        targetB.setActive(false);

        window.setupGLStates();

        if (true){
            sf::RenderTexture& tex = targetA;
            window.setView(sf::View(sf::FloatRect(0, 0, tex.getSize().x, tex.getSize().y)));
            sf::Sprite sprite(tex.getTexture());
            sprite.setScale(1, 1);
            sf::Shader::bind(&shader);
            shader.setUniform("u_tex", sf::Shader::CurrentTexture);
            window.draw(sprite, &shader);
            sf::Shader::bind(NULL);
        }

        window.display();
    }
}

int main()
{
    // testAllDrawables();
    traceSpritePerf();
    // traceTextureBindingBug();
    // traceContextRedundancy();
    
    return EXIT_SUCCESS;
}
