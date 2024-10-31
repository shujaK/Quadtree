#include <SFML/Graphics.hpp>
#include <SFML/Window/Mouse.hpp>
#include <vector>
#include <stdlib.h>
#include "StaticQuadtree.h"
#include "DynamicQuadtree.h"

void drawQuadTree(sf::RenderWindow& window, StaticQuadtree<float>* root)
{
    sf::Vector2f size(root->boundary.size * 2, root->boundary.size * 2); // Full width and height
    sf::RectangleShape rect(size);

    rect.setOrigin(size / 2.0f);
    rect.setPosition(root->boundary.center.x, root->boundary.center.y);
    rect.setFillColor(sf::Color(0, 0, 0, 0));
    rect.setOutlineColor(sf::Color::White);
    rect.setOutlineThickness(0.5f);

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
void printQuadtree(StaticQuadtree<T>* qt, int level = 0)
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

class particle {
public:
    vec2<float> pos;
    vec2<float> velocity;

    particle()
    {
        pos = vec2<float>(0.0f, 0.0f);
        velocity = vec2<float>(0.0f, 0.0f);
    }
    particle(vec2<float> p, vec2<float> v) : pos(p), velocity(v) {}
};

// generating a new quadtree everytime with 10000 points: 14(+- 2) FPS, baseline 17 fps
void main()
{
    auto window = sf::RenderWindow{ { 1000, 1000 }, "Quadtree" };
    //window.setFramerateLimit(144);
    sf::Rect windowRect(0, 0, 1000, 1000);
    quad<float> b(vec2<float>(500, 500), 500);

    float fsimTime = 0.0f;
    float fsimTimeAvg = 0.0f;
    sf::Clock dClock;

    sf::Font font;
    font.loadFromFile("K:\\PROGRAM\\Projects\\cmake-sfml-project\\src\\cour.ttf");

    std::vector<particle> particles;
    for (int i = 0; i < 100; i++)
    {
        vec2<float> p(float(rand() % 991), float(rand() % 991));
        vec2<float> v(float(((rand() % 11) - 5) / 10.0f), float(((rand() % 11) - 5) / 10.0f));
        v.normalize();
        particle pp(p, v);
        particles.emplace_back(pp);
    }

    int counter = 0;
    StaticQuadtree<float>* root = new StaticQuadtree(b, 1);

    ///////////////////
    Quadtree<float> dqt(b, 1);

    dqt.insert(vec2<float>(200.0f, 150.0f));
    dqt.insert(vec2<float>(200.0f, 151.0f));
    dqt.insert(vec2<float>(200.0f, 152.0f));

    auto res = dqt.query(circle<float>(vec2<float>(200.0f, 150.0f), 5.0f));
    //////////////////
    while (window.isOpen())
    {
        float dt = dClock.restart().asSeconds();
        for (auto event = sf::Event{}; window.pollEvent(event);)
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }

        window.clear();
        root = new StaticQuadtree(b, 1);

        sf::Vector2i mousePos = sf::Mouse::getPosition(window);

        for (auto& pt : particles)
        {

            if (pt.pos.x >= 990 || pt.pos.x <= 10) { pt.velocity.x *= -1.0f; }
            if (pt.pos.y >= 990 || pt.pos.y <= 10) { pt.velocity.y *= -1.0f; }

            pt.pos += pt.velocity * (dt * 50);

            sf::CircleShape p(2.0f, 16);
            p.setOrigin({ 1.0f, 1.0f });
            p.setFillColor(sf::Color::White);
            p.setPosition({ pt.pos.x, pt.pos.y });

            root->insert(pt.pos);

            window.draw(p);
        }

        drawQuadTree(window, root);

        counter++;
        fsimTime += dt;
        if (counter >= 50)
        {
            fsimTimeAvg = fsimTime * 20;
            counter = 0;
            fsimTime = 0;
        }

        std::string simTime = std::to_string(fsimTimeAvg) + "ms / " + std::to_string(1000.0 / fsimTimeAvg) + " fps";
        sf::Text simTimeText(simTime, font);
        window.draw(simTimeText);

        window.display();
        delete root;
    }
}
