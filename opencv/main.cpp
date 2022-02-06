//
//  main.cpp
//  opencv
//
//  Created by Gaspar Gyorgy on 30/05/16.
//  Copyright © 2016 Gaspar Gyorgy. All rights reserved.
//

#include<stdio.h>
#include<cstdlib>
#include<iostream>
#include <string>
#include<fstream>
#include<sstream>

#include <dirent.h>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

#include "boost/filesystem.hpp"

using namespace boost::filesystem;
using namespace cv;
namespace fs = boost::filesystem;

class ImageUtil{

public:
    static IplImage cvCreateImage_(const int& new_width, const int& new_height, IplImage& image) {

        IplImage * new_img;

        new_img = cvCreateImage(cvSize(new_width, new_height), image.depth, image.nChannels);

        return *new_img;
    }
    
    ImageUtil();
    
    ~ImageUtil();
    
};

class FileUtil{
    
public:
    static std::streampos fileSize( const char* filePath ){
        
        std::streampos fsize = 0;
        std::ifstream file( filePath, std::ios::binary );
        
        fsize = file.tellg();
        file.seekg( 0, std::ios::end );
        fsize = file.tellg() - fsize;
        file.close();
        
        return fsize;
    }
    
    static
    std::ifstream::pos_type filesize(const char* filename)
    {
        std::streampos fsize = 0;
        std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
        //std::streambuf* buf_ptr =  in.rdbuf();
        fsize = in.tellg() - fsize;

        // std::cout << "file size : " << fsize << " bytes" << std::endl;
        return fsize;
    }

    std::vector<std::string> list_of_filesnames;

    void listdir(const char* dirname, int lvl)
    {
        
        int i;
        DIR* directory;
        struct dirent* entry;
        char longest_name[4096];
        
        while( (directory = opendir(dirname)) == NULL) {
            fprintf(stderr, "Couldn't open directory: %s. Possible reason is the directory does not exist!\n", dirname);
            exit(-1);
        }
        
        while((entry=readdir(directory)) != NULL) {
            
            /* Don't descend up the tree or include the current directory */
            if(strncmp(entry->d_name, "..", 2) != 0 &&
               strncmp(entry->d_name, ".", 1) != 0) {
                
                /* If it's a directory print it's name and recurse into it */
                if (entry->d_type == DT_DIR) {
                    for(i=0; i < 2*lvl; i++) {
                        printf(" ");
                    }
                    printf("%s (d)\n", entry->d_name);
                    
                    /* Prepend the current directory and recurse */
                    strncpy(longest_name, dirname, 4095);
                    strncat(longest_name, "/", 4095);
                    strncat(longest_name, entry->d_name, 4095);
                    listdir(longest_name, lvl+1);
                }
                else {
                    /* Prints the filenames inside the directory */
                    /* Print some leading space depending on the directory level */
                    for(i=0; i < 2*lvl; i++) {
                        printf(" ");
                    }
                    /* We add the names of files to a vector to use it later */
                    list_of_filesnames.push_back(entry->d_name);
                   // printf("%s\n", entry->d_name);
                }
            }
        }
        
        closedir(directory);
        
        return;
    }
    
    FileUtil();
    
   ~FileUtil();

};

// Member functions definitions including constructor
FileUtil::FileUtil(void)
{
    std::cout << "Object is being created" << std::endl;
}

FileUtil::~FileUtil(void)
{
    std::cout << "Object is being deleted" << std::endl;
}

ImageUtil::ImageUtil(void){

    std::cout << "Object is being created" << std::endl;

}

ImageUtil::~ImageUtil(void){
    
    std::cout << "Object is being deleted" << std::endl;
    
}

int main(int argc, const char * argv[]) {
    
    std::cout << "Hello: What's your directory to look for pictures?" << std::endl;

    std::cout << "Example on MacOS: /Users/georgegaspar/Pictures/movies/\n";
    std::string name;
    std::cin >> name;

    //std::cout << name;
    
    std::string directoryName_ = name;
    const char * directoryName = directoryName_.c_str();

    FileUtil fileutil;
    
    std::cout << CV_VERSION << "\n";
    
    fileutil.listdir(directoryName, 0);
    
    std::string *moviename;
    std::streampos fsize_;
    std::streampos fsize2_;
    std::vector<const char *> filesToBeResized;
    Mat image;
    Mat dst;
    IplImage imageToResize;
    IplImage * new_img;
    IplImage * new_img2;

    
    for(auto i = 0; i != fileutil.list_of_filesnames.size(); i++) {
        
        moviename = &fileutil.list_of_filesnames.at(i);
        
        std::string a = directoryName_ + *moviename;
        const char *b = a.c_str();
        
        fsize_ = FileUtil::filesize(b);
        
        // TODO: add target size as param through cin
        // first pass of resize for every image bigger than 250000 bytes
        if (fsize_ > 50000) {
            
            imageToResize = *cvLoadImage(b, CV_LOAD_IMAGE_UNCHANGED);
            
            double width = 0.75;
            double height = 0.75;
            
            const int new_width = (int) ((float) imageToResize.width * width);
            const int new_height = (int) ((float) imageToResize.height * height);
            
            new_img = cvCreateImage(cvSize(new_width, new_height), imageToResize.depth, imageToResize.nChannels);
            cvResize(&imageToResize, new_img);
            
            cvSaveImage(b,new_img);
            
            // Local variable declaration:
            int a = 0;

            do{

                a = a + 1;

                imageToResize = *cvLoadImage(b, CV_LOAD_IMAGE_UNCHANGED);

                const int new_width = (int) ((float) imageToResize.width * 0.99);
                const int new_height = (int) ((float) imageToResize.height * 0.99);

                new_img2 = cvCreateImage(cvSize(new_width, new_height), new_img->depth, new_img->nChannels);
                cvResize(new_img, new_img2);
                cvSaveImage(b,new_img2);
                cvReleaseImage(&new_img2);

                fsize_ = FileUtil::filesize(b);

            
            } while (fsize_ > 50000);
            
            
            std::cout << b << " Passes made:" << a << std::endl;

        }
    }
    
    
    std::cout << "Done \n";
    
    /*
     IplImage* sorceImage = cvLoadImage( "/Users/georgegaspar/Pictures/movies/interstellar.jpg" );
     
     cvNamedWindow("Example1", CV_WINDOW_AUTOSIZE );
     cvShowImage("Example1", sorceImage );
     
     cvWaitKey(1000);
     cvReleaseImage( &sorceImage );
     cvDestroyWindow("Example1");
     */
    
    /*
     Mat gray_image;
     // grayscale conversion
     cvtColor( image, gray_image, CV_BGR2GRAY );
     
     imwrite( "/Users/georgegaspar/Pictures/movies/interstellar2.jpg", gray_image);
     */
    
    return 0;
    
}
