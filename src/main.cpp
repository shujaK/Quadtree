#include <SFML/Graphics.hpp>
#include <SFML/Window/Mouse.hpp>
#include <vector>
#include <stdlib.h>
#include "Quadtree.h"

void drawQuadTree(sf::RenderWindow& window, Quadtree<float>* root)
{
    sf::Vector2f size(root->boundary.size * 2, root->boundary.size * 2); // Full width and height
    sf::RectangleShape rect(size);

    rect.setOrigin(size / 2.0f);
    rect.setPosition(root->boundary.center.x, root->boundary.center.y);
    rect.setFillColor(sf::Color::Black);
    rect.setOutlineColor(sf::Color::White);
    rect.setOutlineThickness(1);

    window.draw(rect);

    if (!root->isLeaf())
    {
        if (root->tr != nullptr) drawQuadTree(window, root->tr);
        if (root->tl != nullptr) drawQuadTree(window, root->tl);
        if (root->br != nullptr) drawQuadTree(window, root->br);
        if (root->bl != nullptr) drawQuadTree(window, root->bl);
    }
}

template <typename T>
void printQuadtree(Quadtree<T>* qt, int level = 0)
{
    if (qt == nullptr)
    {
        return;
    }

    // Indentation based on the level of the node
    std::string indent(level * 2, ' ');
    std::cout << "---------------------------------------------" << std::endl;
    std::cout << indent << "Boundary: (" << qt->boundary.center.x << ", " << qt->boundary.center.y << "), size: " << qt->boundary.size << std::endl;
    std::cout << indent << "Points: ";
    for (int i = 0; i < qt->numPoints; ++i)
    {
        std::cout << "(" << qt->points[i].x << ", " << qt->points[i].y << ") ";
    }
    std::cout << std::endl;

    if (!qt->isLeaf())
    {
        std::cout << indent << "Top Left:" << std::endl;
        printQuadtree(qt->tl, level + 1);

        std::cout << indent << "Top Right:" << std::endl;
        printQuadtree(qt->tr, level + 1);

        std::cout << indent << "Bottom Left:" << std::endl;
        printQuadtree(qt->bl, level + 1);

        std::cout << indent << "Bottom Right:" << std::endl;
        printQuadtree(qt->br, level + 1);
    }
    std::cout << "---------------------------------------------" << std::endl;
}

int main()
{
    std::vector<sf::CircleShape> points;

    quad<float> b(vec2<float>(500, 500), 500);
    Quadtree<float>* root = new Quadtree<float>(b, 1);

    for (int i = 0; i < 1000; i++)
    {
        vec2<float> p(rand() % 1000, rand() % 1000);
        root->insert(p);

        float rad = 2.0f;
        sf::CircleShape pp(rad, 16);
        pp.setOrigin({ rad / 2.0f, rad / 2.0f });
        pp.setPosition({ p.x, p.y });
        points.push_back(pp);
    }

    auto window = sf::RenderWindow{ { 1000, 1000 }, "Quadtree" };
    window.setFramerateLimit(144);

    sf::Rect windowRect(0, 0, 1000, 1000);

    std::vector<sf::CircleShape> queryPointDisplay;

    sf::Vector2f queryRectSize = { 200.0f, 200.0f };
    sf::RectangleShape queryRect(queryRectSize);
    queryRect.setOrigin(queryRectSize / 2.0f);
    queryRect.setFillColor(sf::Color(0, 0, 0, 0));
    queryRect.setOutlineColor(sf::Color::Green);
    queryRect.setOutlineThickness(1);

    sf::CircleShape queryCircle(100.0f);
    queryCircle.setOrigin({ 100.0f, 100.0f });
    queryCircle.setFillColor(sf::Color(0, 0, 0, 0));
    queryCircle.setOutlineColor(sf::Color::Green);
    queryCircle.setOutlineThickness(1);

    bool justClicked = false;
    while (window.isOpen())
    {
        for (auto event = sf::Event{}; window.pollEvent(event);)
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }
        window.clear();

        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        queryCircle.setPosition((sf::Vector2f)mousePos);

        if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
        {
            points.clear();
            root = new Quadtree<float>(b, 1);
        }

        if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            if (windowRect.contains(mousePos) && !justClicked)
            {
                float rad = 2.0f;
                sf::CircleShape p(rad, 16);
                p.setOrigin({ rad/2.0f, rad/2.0f });
                p.setPosition((sf::Vector2f) mousePos);
                points.push_back(p);
                // justClicked = true;

                vec2<float> qtp((float)mousePos.x, (float) mousePos.y);
                root->insert(qtp);
            }
        }
        else
        {
            justClicked = false;
        }

        drawQuadTree(window, root);

        window.draw(queryCircle);

        for (const auto& point : points)
        {
            window.draw(point);
        }

        // quad<float> queryQuad(vec2<float>(queryRect.getPosition().x, queryRect.getPosition().y), 100.0f);
        circle<float> cqueryCircle(vec2<float>(queryCircle.getPosition().x, queryCircle.getPosition().y), 100.0f);
        


        queryPointDisplay.clear();

        auto pointsv = root->queryCircle(cqueryCircle);

        for (const auto& vec2 : pointsv)
        {
            sf::CircleShape p(2.0f, 16);
            p.setOrigin({ 1.0f, 1.0f });
            p.setFillColor(sf::Color::Green);
            p.setPosition({vec2.x, vec2.y});
            queryPointDisplay.push_back(p);
        }

        for (const auto& vec2 : queryPointDisplay)
        {
            window.draw(vec2);
        }
        window.display();
    }
}
