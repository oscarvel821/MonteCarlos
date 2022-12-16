#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <random>
#include <omp.h>

#define LIMIT 250000
int seed[LIMIT];

int main()
{
    sf::RenderWindow window(sf::VideoMode(1000, 1000), "Monte Carlo");

    window.setFramerateLimit(60);

    int TotalDots = 0;
    int dotsInCircle = 0;
    int dotsInSquare = 0;
    double pi = 0;

    sf::VertexArray points(sf::Points, LIMIT);

    std::vector<std::pair<int, int>> dots;

    sf::RectangleShape rect;

    sf::Vector2f rectanglePosition(200, 200);

    rect.setPosition(rectanglePosition);

    rect.setSize(sf::Vector2f(600, 600));

    rect.setOutlineThickness(4);
    rect.setOutlineColor(sf::Color::Red);

    sf::CircleShape circle;

    sf::Vector2f circlePosition(200, 200);

    circle.setPosition(circlePosition);

    circle.setRadius(300);

    circle.setOutlineThickness(3);
    circle.setOutlineColor(sf::Color::Blue);

    sf::RectangleShape vertical;

    sf::Vector2f verticalPosition(500, 100);
    vertical.setPosition(verticalPosition);

    vertical.setSize(sf::Vector2f(1, 800));
    vertical.setOutlineThickness(1);
    vertical.setOutlineColor(sf::Color::Black);
    vertical.setFillColor(sf::Color::Black);

    sf::RectangleShape hor;

    sf::Vector2f horPosition(100, 500);
    hor.setPosition(horPosition);

    hor.setSize(sf::Vector2f(800, 1));
    hor.setOutlineThickness(1);
    hor.setOutlineColor(sf::Color::Black);
    hor.setFillColor(sf::Color::Black);

    std::cout << "Press space to generate circles" << std::endl;

    while (window.isOpen()) {
        sf::Event event;

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                window.close();

            if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Space) {
                dots.clear();
                dotsInCircle = 0;
                dotsInSquare = 0;
                pi = 0;

                for (int i = 0; i < LIMIT; i++)
                    seed[i] = rand();

#pragma omp parallel
                {
                    int id = omp_get_thread_num();
                    int nthreads = omp_get_num_threads();

                    std::vector<std::pair<int, int>> lst;
                    int in = 0;
                    int i;
                    for (i = id; i < LIMIT; i += nthreads) {
                        srand(seed[i]);
                        double xPos = rand() % 600 + 200;
                        double yPos = rand() % 600 + 200;

                        double dist = sqrt((xPos - 500) * (xPos - 500) + (yPos - 500) * (yPos - 500));

                        if (dist <= 300)
                            in++;

                        std::pair<int, int> coor(xPos, yPos);

                        lst.push_back(coor);

                    }
#pragma omp critical
                    {
                        dotsInCircle += in;
                        for (int j = 0; j < lst.size(); j++) {
                            dots.push_back(lst[j]);
                        }
                    }

                }
                pi = double(4 * dotsInCircle) / LIMIT;

                std::cout << dotsInCircle << std::endl;
                std::cout << "PI : " << pi << std::endl;
                std::cout << "Press space to generate circles" << std::endl;
            }
        }

        //render
        window.clear(sf::Color::White);
        window.draw(rect);
        window.draw(circle);
        window.draw(vertical);
        window.draw(hor);

        //slows down performance
       /* for (auto d : dots) {
            sf::CircleShape dot;

            sf::Vector2f dotPosition(d.first, d.second);

            dot.setPosition(dotPosition);

            dot.setRadius(2);

            double dist = sqrt((d.first - 500) * (d.first - 500) + (d.second - 500) * (d.second - 500));

            if (dist <= 300.0)
                dot.setFillColor(sf::Color(255, 165, 0));
            else
                dot.setFillColor(sf::Color::Green);
            window.draw(dot);
        }*/

        for (int i = 0; i < dots.size(); i++) {
            points[i].position = sf::Vector2f(dots[i].first, dots[i].second);

            double dist = sqrt((dots[i].first - 500) * (dots[i].first - 500) + (dots[i].second - 500) * (dots[i].second - 500));

            if (dist <= 300.0)
                points[i].color = sf::Color(255, 165, 0);
            else
                points[i].color = sf::Color::Green;
        }

        window.draw(points);

        window.display();
    }

    return 0;
}