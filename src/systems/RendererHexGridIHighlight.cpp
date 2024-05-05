/*
Copyright (c) 2022 James Dean Mathias

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "RendererHexGridIHighlight.hpp"

#include "components/Property.hpp"
#include "entities/Factory.hpp"
#include "services/ConfigurationPath.hpp"
#include "services/ContentKey.hpp"

namespace systems
{
    RendererHexGridIHighlight::RendererHexGridIHighlight(std::shared_ptr<Level> level, std::function<void(entities::EntityPtr)> addEntity) :
        RendererHexGridHighlight(level, addEntity)
    {
        // We make an entity that has the animated sprite that is rendered over every Property::I object.
        m_highlight = std::make_shared<entities::Entity>();
        m_highlight->addComponent(entities::createAnimatedSprite(config::DOM_IMAGES_ANIMATED, "i-am-highlight-512", content::KEY_IMAGE_I_AM_HIGHLIGHT_512));
        m_texture = m_highlight->getComponent<components::AnimatedSprite>()->getSprite()->getTexture();

        m_addEntity(m_highlight);
    }

    // --------------------------------------------------------------
    //
    // Have to additionally check the Property component to see if
    // it is "I".
    //
    // --------------------------------------------------------------
    bool RendererHexGridIHighlight::isInterested(const entities::EntityPtr& entity)
    {
        return System::isInterested(entity) &&
               entity->getComponent<components::Property>()->has(components::PropertyType::I);
    }

} // namespace systems
