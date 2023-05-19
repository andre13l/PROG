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
                //this command isn't passing the tests
                median_filter();
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
        // Each individual pixel (r,g,b) becomes (255-r,255-g,255-b) 
        for (int y = 0; y < image->height(); y++) {
            for (int x = 0; x < image->width(); x++) {
                image->at(x,y).red()= 255 - image->at(x,y).red();
                image->at(x,y).green()= 255 - image->at(x,y).green();
                image->at(x,y).blue()= 255 - image->at(x,y).blue();
            }
        }
    }
    void Script::to_gray_scale() {
        // Each individual pixel (r,g,b) becomes (v,v,v) where v = (r + g + b)/3
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
        // all (r1,  g1, b1) pixels become (r2,  g2, b2)
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
        // all pixels in the rectangle with top-left corner (x,y), width w, and height h
        // have their RGB values changed to (r,g,b)
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
        // mirror the image horizontally
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
        // mirror the image vertically
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
        // copy all pixels from image stored in PNG file filename
        // to the rectangle with top-left corner (x,y) of the current image
        // except pixels with "neutral" color (r,g,b)
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
        // Reduce the image to the the pixels in the rectangle defined by top-left corner (x,y), width w, and height h
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
        // Rotate image left by 90 degrees
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
        // Rotate image right by 90 degrees
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
        int windowSize;
        input >> windowSize;
        // Create a temporary image to store the filtered result
        Image* filteredImage = new Image(image->width(), image->height());

        int halfWindowSize = windowSize / 2;

        // Iterate over each pixel in the image
        for (int y = 0; y < image->height(); y++) {
            for (int x = 0; x < image->width(); x++) {
                std::vector<int> redValues;
                std::vector<int> greenValues;
                std::vector<int> blueValues;

                // Iterate over the pixels within the window centered at (x, y)
                for (int j = -halfWindowSize; j <= halfWindowSize; j++) {
                    for (int i = -halfWindowSize; i <= halfWindowSize; i++) {
                        int neighborX = std::max(0, std::min(image->width() - 1, x + i));
                        int neighborY = std::max(0, std::min(image->height() - 1, y + j));

                        Color& neighborColor = image->at(neighborX, neighborY);
                        if((y+j)>=0 && (x+i)>=0 && (y+j)<image->height() && (x+i)<image->width()){
                            redValues.push_back(neighborColor.red());
                            greenValues.push_back(neighborColor.green());
                            blueValues.push_back(neighborColor.blue());
                        }
                    }
                }

                // Sort the color values to find the median
                std::sort(redValues.begin(), redValues.end());
                std::sort(greenValues.begin(), greenValues.end());
                std::sort(blueValues.begin(), blueValues.end());

                // Get the median color value
                int medianRed = redValues[redValues.size() / 2];
                int medianGreen = greenValues[greenValues.size() / 2];
                int medianBlue = blueValues[blueValues.size() / 2];

                // Set the filtered color in the temporary image
                filteredImage->at(x,y).red() = medianRed;
                filteredImage->at(x,y).green() = medianGreen;
                filteredImage->at(x,y).blue() = medianBlue;
            }
        }

        // Replace the original image with the filtered image
        delete image;
        image = filteredImage;
    }
    void Script::xpm2_open(const std::string& filename) {
        // Replace the current image (if any) with the image read from the XPM2 file
        clear_image_if_any();
        image = loadFromXPM2(filename);
}

    void Script::xpm2_save(const std::string& filename) {
        // Save the current image to the XPM2 file
        saveToXPM2(filename, image);
}
}
 