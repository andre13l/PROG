#include <iostream>
#include <fstream>
#include "Script.hpp"
#include "PNG.hpp"
#include "XPM2.hpp"
#include <vector>
#include <algorithm>

using namespace std;

namespace prog {
    // Use to read color values from a script file.
    istream& operator>>(istream& input, Color& c) {
        int r, g, b;
        input >> r >> g >> b;
        c.red() = r;
        c.green() = g;
        c.blue() = b;
        return input;
    }

    Script::Script(const string& filename) :
            image(nullptr), input(filename) {

    }
    void Script::clear_image_if_any() {
        if (image != nullptr) {
            delete image;
            image = nullptr;
        }
    }
    Script::~Script() {
        clear_image_if_any();
    }

    void Script::run() {
        string command;
        while (input >> command) {
            cout << "Executing command '" << command << "' ..." << endl;
            if (command == "open") {
                open();
                continue;
            }
            if (command == "blank") {
                blank();
                continue;
            }
            // Other commands require an image to be previously loaded.
            if (command == "save") {
                save();
                continue;
            } 
            if (command == "invert"){
                invert();
                continue;
            }
            if (command == "to_gray_scale") {
                to_gray_scale();
                continue;
            }
            if (command == "replace") {
                replace();
                continue;
            }
            if (command == "fill") {
                fill();
                continue;
            }
            if (command == "h_mirror") {
                h_mirror();
                continue;
            }
            if (command == "v_mirror") {
                v_mirror();
                continue;
            }
            if (command == "add") {
                add();
                continue;
            }
            if (command == "crop") {
                crop();
                continue;
            }
            if (command == "rotate_left") {
                rotate_left();
                continue;
            }
            if (command == "rotate_right") {
                rotate_right();
                continue;
            }
            if (command == "median_filter") {
                median_filter();
                continue;
            }
            if (command == "xpm2_open") {
                xpm2_open();
                continue;
            }
            if (command == "xpm2_save") {
                xpm2_save();
                continue;
            }
        }

    }
    void Script::open() {
        // Replace current image (if any) with image read from PNG file.
        clear_image_if_any();
        string filename;
        input >> filename;
        image = loadFromPNG(filename);
    }
    void Script::blank() {
        // Replace current image (if any) with blank image.
        clear_image_if_any();
        int w, h;
        Color fill;
        input >> w >> h >> fill;
        image = new Image(w, h, fill);
    }
    void Script::save() {
        // Save current image to PNG file.
        string filename;
        input >> filename;
        saveToPNG(filename, image);
    }
    void Script::invert() {
        for (int y = 0; y < image->height(); y++) {
        for (int x = 0; x < image->width(); x++) {
            image->at(x,y).red()= 255 - image->at(x,y).red();
            image->at(x,y).green()= 255 - image->at(x,y).green();
            image->at(x,y).blue()= 255 - image->at(x,y).blue();
        }
        }
    }
    void Script::to_gray_scale() {
        for (int y = 0; y < image->height(); y++) {
            for (int x = 0; x < image->width(); x++) {
            int v = (image->at(x,y).red()+image->at(x,y).green()+image->at(x,y).blue())/3;
            image->at(x,y).red()= v;
            image->at(x,y).green()= v;
            image->at(x,y).blue()= v;
            }
        }
    }
    void Script::replace() {
        Color c1, c2;
        input >> c1 >> c2;
        for (int y = 0; y < image->height(); y++) {
            for (int x = 0; x < image->width(); x++) {
                if((image->at(x,y).red()==c1.red()) && (image->at(x,y).green()==c1.green()) && (image->at(x,y).blue()== c1.blue())){
                    image->at(x,y).red() = c2.red();
                    image->at(x,y).green() = c2.green();
                    image->at(x,y).blue() = c2.blue();
                }
            }
        }
    }
    void Script::fill() {
        int x,y,w,h;
        Color c1;
        input >> x >> y >> w >> h >> c1;
        for (int i = x; i < x+w; i++){
            for(int j = y; j< y+h; j++){
                image->at(i,j).red() = c1.red();
                image->at(i,j).green() = c1.green();
                image->at(i,j).blue() = c1.blue();
            }
        }
    }
    void Script::h_mirror() {
        Color c1,c2;;
        for(int i =0; i<(image->width())/2;i++){
            for(int j=0; j<image->height(); j++){
                c1=image->at(i,j);
                c2=image->at(image->width()-i-1,j);
                image->at(i,j)=c2;
                image->at(image->width()-i-1,j)=c1;
            }
        }
    }
    void Script::v_mirror() {
        Color c1,c2;;
        for(int i =0; i<image->width();i++){
            for(int j=0; j<(image->height())/2; j++){
                c1=image->at(i,j);
                c2=image->at(i,image->height()-1-j);
                image->at(i,j)=c2;
                image->at(i,image->height()-1-j)=c1;
            }
        }
    }

void Script::add() {
    string filename;
    Color c;
    int x, y;
    input >> filename >> c >> x >> y;
    Image* img = loadFromPNG(filename);
    int imgWidth = img->width();
    int imgHeight = img->height();

    for (int i = 0; i < imgWidth; i++) {
        for (int j = 0; j < imgHeight; j++) {
            Color pixel = img->at(i, j);
            if (!(pixel.red() == c.red() && pixel.green() == c.green() && pixel.blue() == c.blue())) {
                int destX = x + i;
                int destY = y + j;
                if (destX >= 0 && destX < image->width() && destY >= 0 && destY < image->height()) {
                    image->at(destX, destY) = pixel;
                }
            }
        }
    }
    delete img;  // Delete the loaded image after adding its pixels to the current image
}




    void Script::crop() {
        int x,y,w,h;
        input >> x >> y >> w >> h;
        Image* croppedImage = new Image(w, h);
        
        for(int i=0;i<w;i++){
            for(int j=0;j<h;j++){
                croppedImage->at(i, j) = image->at(i + x, j + y);
            }
        }
        delete image;  // Delete the previous image

        image = croppedImage;  // Assign the cropped image to the member variable

    }

    void Script::rotate_left() {
    Image* rotatedImage = new Image(image->height(), image->width(), Color());

    for (int y = 0; y < image->height(); y++) {
        for (int x = 0; x < image->width(); x++) {
            int rotatedX = y;
            int rotatedY = image->width() - x - 1;
            rotatedImage->at(rotatedX, rotatedY) = image->at(x, y);
        }
    }

    delete image;
    image = rotatedImage;
}


    void Script::rotate_right() {
    Image* rotatedImage = new Image(image->height(), image->width(), Color());

    for (int y = 0; y < image->height(); y++) {
        for (int x = 0; x < image->width(); x++) {
            int rotatedX = image->height() - y - 1;
            int rotatedY = x;
            rotatedImage->at(rotatedX, rotatedY) = image->at(x, y);
        }
    }

    delete image;
    image = rotatedImage;
}
    void Script::median_filter() {
        int ws;
        input >> ws;
        // Create a temporary image to store the filtered result
        Image* filteredImage = new Image(image->width(), image->height());

        int halfws = ws / 2;

        // Iterate over each pixel in the image
        for (int y = 0; y < image->height(); y++) {
            for (int x = 0; x < image->width(); x++) {
                vector<int> redValues;
                vector<int> greenValues;
                vector<int> blueValues;

                // Iterate over the pixels within the window centered at (x, y)
                for (int j = -halfws; j <= halfws; j++) {
                    for (int i = -halfws; i <= halfws; i++) {
                        int neighborX = x + i;
                        int neighborY = y + j;

                        // Check if the neighbor pixel is within the image bounds
                        if (neighborX >= 0 && neighborX < image->width() && neighborY >= 0 && neighborY < image->height()) {
                            Color& neighborColor = image->at(neighborX, neighborY);
                            redValues.push_back(neighborColor.red());
                            greenValues.push_back(neighborColor.green());
                            blueValues.push_back(neighborColor.blue());
                        }
                    }
                }

                // Sort the color values to find the median
                sort(redValues.begin(), redValues.end());
                sort(greenValues.begin(), greenValues.end());
                sort(blueValues.begin(), blueValues.end());

                // Get the median color value
                int medianRed = redValues[redValues.size() / 2];
                int medianGreen = greenValues[greenValues.size() / 2];
                int medianBlue = blueValues[blueValues.size() / 2];

                // Set the filtered color in the temporary image
                Color& filteredColor = filteredImage->at(x, y);
                filteredColor.red() = medianRed;
                filteredColor.green() = medianGreen;
                filteredColor.blue() = medianBlue;
        }
    }

        // Replace the original image with the filtered image
        delete image;
        image = filteredImage;
    }
    void Script::xpm2_open() {
        string filename;
        clear_image_if_any();
        image = loadFromXPM2(filename); 
    }
    void Script::xpm2_save() {
        string filename;
        // Save the current image to the XPM2 file
        saveToXPM2(filename, image);    
    }
}
 