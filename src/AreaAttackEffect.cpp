#include "AreaAttackEffect.h"


AreaAttackEffect::AreaAttackEffect(const sf::Vector2f& center, float maxRadius)
    : m_maxRadius(maxRadius) {

    m_circle.setRadius(0.f);
    m_circle.setFillColor(sf::Color::Transparent);
    m_circle.setOutlineColor(sf::Color(255, 100, 0, 150)); // Naranja semitransparente
    m_circle.setOutlineThickness(2.f);

    // Ajuste del origen del círculo al centro
    m_circle.setOrigin(sf::Vector2f(0.f, 0.f));

    // Posicionar el círculo en el centro del área de expansión
    m_circle.setPosition(center);
}

bool AreaAttackEffect::isComplete() const {
    return m_currentTime >= m_duration;
}

void AreaAttackEffect::update(float deltaTime) {
    m_currentTime += deltaTime;
    float progress = m_currentTime / m_duration;

    // Interpolación suave para la expansión
    m_currentRadius = m_maxRadius * progress;
    m_circle.setRadius(m_currentRadius);

    // Ajustar origen para mantener el círculo centrado
    m_circle.setOrigin(sf::Vector2f(m_currentRadius, m_currentRadius));

    // Efecto de desvanecimiento
    sf::Color currentColor = m_circle.getOutlineColor();
    currentColor.a = static_cast<std::uint8_t>(150 * (1.f - progress));
    m_circle.setOutlineColor(currentColor);
}

void AreaAttackEffect::draw(sf::RenderTarget& target) const {
    if (!isComplete()) {
        target.draw(m_circle);
    }
}