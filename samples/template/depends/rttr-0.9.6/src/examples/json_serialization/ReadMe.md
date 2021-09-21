JSON Serialization
==================

This example demonstrate the usage of RTTR in order to serialize and deserialize an object with JSON.

Given is following class:
```cpp
enum class color
{
    red,
    green,
    blue
};

struct point2d
{
    point2d() {}
    point2d(int x_, int y_) : x(x_), y(y_) {}
    int x = 0;
    int y = 0;
};

struct shape
{
    shape(std::string n) : name(n) {}

    void set_visible(bool v) { visible = v; }
    bool get_visible() const { return visible; }

    color color_ = color::blue;
    std::string name = "";
    point2d position;
    std::map<color, point2d> dictionary;

    RTTR_ENABLE()
private:
    bool visible = false;
};

struct circle : shape
{
    circle(std::string n) : shape(n) {}

    double radius = 5.2;
    std::vector<point2d> points;

    int no_serialize = 100;

    RTTR_ENABLE(shape)
};
```
serialize it like this:
```cpp
int main(int argc, char** argv)
{
    std::string json_string;

    {
        circle c_1("Circle #1");
        shape& my_shape = c_1;

        c_1.set_visible(true);
        c_1.points = std::vector<point2d>(2, point2d(1, 1));
        c_1.points[1].x = 23;
        c_1.points[1].y = 42;

        c_1.position.x = 12;
        c_1.position.y = 66;

        c_1.radius = 5.123;
        c_1.color_ = color::red;

        c_1.dictionary = { { {color::green, {1, 2} }, {color::blue, {3, 4} }, {color::red, {5, 6} } } };

        c_1.no_serialize = 12345;

        json_string = io::to_json(my_shape); // serialize the circle to 'json_string'
    }

    std::cout << "Circle: c_1:\n" << json_string << std::endl;

    circle c_2("Circle #2"); // create a new empty circle

    io::from_json(json_string, c_2); // deserialize it with the content of 'c_1'
    std::cout << "\n############################################\n" << std::endl;

    std::cout << "Circle c_2:\n" << io::to_json(c_2) << std::endl;

    return 0;
}
```

Output:
```
Circle: c_1:
{
    "visible": true,
    "color": "red",
    "name": "Circle #1",
    "position": {
        "x": 12,
        "y": 66
    },
    "dictionary": [
        {
            "key": "red",
            "value": {
                "x": 5,
                "y": 6
            }
        },
        {
            "key": "green",
            "value": {
                "x": 1,
                "y": 2
            }
        },
        {
            "key": "blue",
            "value": {
                "x": 3,
                "y": 4
            }
        }
    ],
    "radius": 5.123,
    "points": [
        {
            "x": 1,
            "y": 1
        },
        {
            "x": 23,
            "y": 42
        }
    ]
}

############################################

Circle c_2:
{
    "visible": true,
    "color": "red",
    "name": "Circle #1",
    "position": {
        "x": 12,
        "y": 66
    },
    "dictionary": [
        {
            "key": "red",
            "value": {
                "x": 5,
                "y": 6
            }
        },
        {
            "key": "green",
            "value": {
                "x": 1,
                "y": 2
            }
        },
        {
            "key": "blue",
            "value": {
                "x": 3,
                "y": 4
            }
        }
    ],
    "radius": 5.123,
    "points": [
        {
            "x": 1,
            "y": 1
        },
        {
            "x": 23,
            "y": 42
        }
    ]
}
```
