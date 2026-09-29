#include <iostream>
#include <optional>
#include <vector>
#include <cassert>
#include <cmath>

#include <SFML/Graphics.hpp>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;
const int FPS_LIMIT = 30;

using Point2D = sf::Vector2f;

float pos = 0.f;
float vel = 0.01f;

void updatePosition() {
    if(0.f > pos+vel) {
        pos = 0.f;
        vel *= -1;
    }
    else if (pos+vel > 1.f) {
        pos = 1;
        vel *= -1;
    }
    else {pos += vel;}
    return;
}

// TODO: (Part 1) Define a function that samples a cubic Bezier curve at t in [0, 1].
Point2D getPoint(const std::vector<Point2D>& pts, float t) { 
    assert(pts.size() >= 4);  // t in [0,1]
    assert(0 <= t && t <= 1);   // cubic Bezier needs at least 4 points

    float x = pow(1-t, 3)*pts[0].x + 3*pow(1-t, 2)*t*pts[1].x + 3*(1-t)*t*t*pts[2].x + pow(t,3)*pts[3].x;
    float y = pow(1-t, 3)*pts[0].y + 3*pow(1-t, 2)*t*pts[1].y + 3*(1-t)*t*t*pts[2].y + pow(t,3)*pts[3].y;

    return Point2D{x,y}; 
}

// TODO: (Part 2) Define a function that returns the curve's slope at t in [0, 1].
Point2D getSlope(const std::vector<Point2D>& pts, float t) { 
    assert(pts.size() >= 4);  // t in [0,1]
    assert(0 <= t && t <= 1);   // cubic Bezier needs at least 4 points

    float slope_x = 3*pow(1-t,3)*(pts[1].x-pts[0].x) + 6*(1-t)*t*(pts[2].x-pts[1].x) + 3*t*t*(pts[3].x-pts[2].x);
    float slope_y = 3*pow(1-t,3)*(pts[1].y-pts[0].y) + 6*(1-t)*t*(pts[2].y-pts[1].y) + 3*t*t*(pts[3].y-pts[2].y);

    return Point2D{slope_x,slope_y}; 
}

// TODO: (Part 1) Store four control points for the curve.
std::vector<Point2D> ctrl_pts = {
    Point2D{10.f,10.f},
    Point2D{(WINDOW_WIDTH-20.f)*0.75f, (WINDOW_HEIGHT-20.f)*0.25f},
    Point2D{(WINDOW_WIDTH-20.f)*0.25f, (WINDOW_HEIGHT-20.f)*0.75f},
    Point2D{WINDOW_WIDTH-20.f,WINDOW_HEIGHT-20.f}
};
// TODO: (Part 2) Track animation time for the square moving along the curve.
// TODO: (Part 3) Track the index of the control point being dragged.

float getDistance(sf::Event::MouseButtonPressed* click, int i) {
    Point2D here = Point2D(click->position);
    return sqrt(pow(here.x - ctrl_pts[i].x, 2) + pow(here.y - ctrl_pts[i].y, 2));
}

void handleInput(sf::Window& window, bool& shouldQuit) {
    while (const std::optional<sf::Event> event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
            shouldQuit = true;
        } else if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>()) {
            // TODO: (Part 3) On left-click, select the closest control point
            if(mouse.button == sf::Mouse::Button::Left) {
                int handle;
                if(getDistance(mouse,1) > getDistance(mouse,2)) {
                    handle = 1;
                }
                else {handle = 2;}
            }
            // using mouse->position and start dragging it.
        } else if (const auto* mouse = event->getIf<sf::Event::MouseButtonReleased>()) {
            // TODO: (Part 3) On left-button release, stop dragging.
        } else if (const auto* mouse = event->getIf<sf::Event::MouseMoved>()) {
            // TODO: (Part 3) Move the selected control point to mouse->position.
            // TODO: (Part 4) Maintain matching slopes at shared endpoints.
            // When moving point 3, move point 5 without changing its distance
            // from point 4 (point numbers here start at 1).
        } else if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
            // TODO: (Part 4) '+' adds three control points; '-' removes three,
            // keeping at least four points.
        }
    }
}

void render(sf::RenderWindow& window) {
    window.clear(sf::Color::Black);
    // ====== ====== ======
    // TODO: (Part 1) Sample GetPoint over t in [0, 1] and connect samples using the line-drawing
    // code from your project. Draw all four control points as circles after drawing the curve.
    // ====== ====== ======
    // Get a set of vertices for the line
    auto line = std::vector<sf::Vertex>();
    line.push_back(sf::Vertex{ctrl_pts[0]});
    for( float t=0.f; t <= 1.f; t+=0.01f) {
        line.push_back(sf::Vertex{getPoint(ctrl_pts,t)});
    }
    // Draw vertices
    window.draw(line.data(), line.size(), sf::PrimitiveType::Lines);
    // Draw points
    for(auto pt : ctrl_pts) {
        sf::CircleShape point = sf::CircleShape(5.f);
        point.setFillColor(sf::Color::Red);
        point.setPosition(Point2D{pt.x-5.f, pt.y-5.f});
        window.draw(point);
    }

    // ====== ====== ======
    // TODO: (Part 2) Draw a small square moving repeatedly along the curve.
    // Use GetSlope to orient it to the curve at each time step.
    // ====== ====== ======

    // Prepare values
    sf::RectangleShape square(Point2D{10.f,10.f});
    square.setOrigin(Point2D{5.f,5.f});
    updatePosition();
    Point2D pt = getPoint(ctrl_pts,pos);
    Point2D slope = getSlope(ctrl_pts,pos);

    // std::cout << "position: (" << pt.x << ',' << pt.y << ")\n";

    // Set position along the line
    square.setPosition(Point2D{pt.x,pt.y});

    // Set angle at position
    sf::Angle angle = sf::radians(std::atan(slope.y/slope.x));
    square.setRotation(angle);
    // std::cout << "square: (" << square.getPosition().x << ',' << square.getPosition().y << ")\n";

    window.draw(square);

    // ====== ====== ======
    // TODO: (Part 3) Draw control handles from point 1 to 2 and point 3 to 4.
    // TODO: (Part 4) Draw all connected cubic Bezier segments and their handles.
    // ====== ====== ======
    

    // ====== ====== ======
    // TODO: (Bonus) Support multiple curves, a Galaga screen overlay at a 1:2 ratio, and exporting
    // curve points as C++ code for Project 1b.
    // ====== ====== ======

    window.display();
}

int main() {
    sf::RenderWindow window;

    try {
        // Initialize window
        window.create(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "Bezier Curve Editor");
        window.setFramerateLimit(FPS_LIMIT);
        // Prevent key repeats.
        window.setKeyRepeatEnabled(false);

        bool shouldQuit = false;
        // Main game loop
        while (window.isOpen()) {
            handleInput(window, shouldQuit);
            if (shouldQuit) {
                break;
            }
            render(window);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}
