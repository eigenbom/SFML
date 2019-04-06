////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2019 Laurent Gomila (laurent@sfml-dev.org)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Err.hpp>
#include <cstdlib>
#include <cassert>

namespace sf
{
////////////////////////////////////////////////////////////
Sprite::Sprite() :
m_texture       (NULL),
m_textureRect   (),
m_color(Color::White)
{
}


////////////////////////////////////////////////////////////
Sprite::Sprite(const Texture& texture) :
m_texture       (NULL),
m_textureRect   (),
m_color(Color::White)
{
    setTexture(texture);
}


////////////////////////////////////////////////////////////
Sprite::Sprite(const Texture& texture, const IntRect& rectangle) :
m_texture       (&texture),
m_textureRect   (rectangle),
m_color(Color::White)
{
    updatePositions();
    updateTexCoords();
}


////////////////////////////////////////////////////////////
void Sprite::setTexture(const Texture& texture, bool resetRect)
{
	const Texture* oldTexture = m_texture;

	// Assign the new texture
	m_texture = &texture;

    // Recompute the texture area if requested, or if there was no valid texture & rect before
    if (resetRect || (!oldTexture && (m_textureRect == sf::IntRect())))
        setTextureRect(IntRect(0, 0, texture.getSize().x, texture.getSize().y));
}


////////////////////////////////////////////////////////////
void Sprite::setTextureRect(const IntRect& rectangle)
{
    if (rectangle != m_textureRect)
    {
        m_textureRect = rectangle;
        updatePositions();
        updateTexCoords();
    }
}


////////////////////////////////////////////////////////////
void Sprite::setColor(const Color& color)
{
    m_color = color;
}


////////////////////////////////////////////////////////////
const Texture* Sprite::getTexture() const
{
    return m_texture;
}


////////////////////////////////////////////////////////////
const IntRect& Sprite::getTextureRect() const
{
    return m_textureRect;
}


////////////////////////////////////////////////////////////
const Color& Sprite::getColor() const
{
	return m_color;
}


////////////////////////////////////////////////////////////
FloatRect Sprite::getLocalBounds() const
{
    float width = static_cast<float>(std::abs(m_textureRect.width));
    float height = static_cast<float>(std::abs(m_textureRect.height));

    return FloatRect(0.f, 0.f, width, height);
}


////////////////////////////////////////////////////////////
FloatRect Sprite::getGlobalBounds() const
{
    return getTransform().transformRect(getLocalBounds());
}


////////////////////////////////////////////////////////////
void Sprite::draw(RenderTarget& target, RenderStates states) const
{
    if (m_texture)
    {
		if (states.shader == NULL) {
            sf::err() << "Sprite::draw() requires a shader!" << std::endl;
            assert(false);
		}

        states.transform *= getTransform() * m_vertexTransform;
        states.texture = m_texture;
        states.textureTransform = &m_textureTransform;
		states.color = m_color;
		target.draw(target.getSpriteVBO(), states);
    }
}


////////////////////////////////////////////////////////////
void Sprite::updatePositions()
{
    FloatRect bounds = getLocalBounds();
	m_vertexTransform = Transform(
		bounds.width, 0.0f, 0.0f,
		0.0f, bounds.height, 0.0f,
		0.0f, 0.0f, 1.0f
    );
}


////////////////////////////////////////////////////////////
void Sprite::updateTexCoords()
{
    float left   = static_cast<float>(m_textureRect.left);
    float right  = left + m_textureRect.width;
    float top    = static_cast<float>(m_textureRect.top);
    float bottom = top + m_textureRect.height;

	Vector2u actualSize = m_texture->getActualSize();
	float xscale = (right - left) / (float)actualSize.x;
	float yscale = (bottom - top) / (float)actualSize.y;
	float xorigin = left / (float)actualSize.x;
	float yorigin = top / (float)actualSize.y;

	if (m_texture->m_pixelsFlipped)
	{
		yscale *= -1.0f;
		Vector2u size = m_texture->getSize();
		yorigin += size.y / (float)actualSize.y;
	}

	m_textureTransform = Transform(
		xscale, 0.0f, xorigin,
		0.0f, yscale, yorigin,
		0.0f, 0.0f, 1.0f
	);
}

} // namespace sf
