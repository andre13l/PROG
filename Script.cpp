#include <iostream>
#include <fstream>
#include "Script.hpp"
#include "PNG.hpp"
#include "XPM2.hpp"

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
        int x,y;
        input >> filename >> c >> x >> y;
        Image *img = loadFromPNG(filename);
        for(int i = x; i < image->width();i++){
            for (int j = y; j < image->height(); j++)
            {
                if((img->at(i,j).red()!=c.red()) && (img->at(i,j).green()!=c.green()) && (img->at(i,j).blue()!=c.blue())){
                    image->at(i,j)=img->at(i,j);
                }
            }
            
        }
    }

}
 